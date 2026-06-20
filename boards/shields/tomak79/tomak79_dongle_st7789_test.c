#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <stdio.h>
#include <string.h>

LOG_MODULE_REGISTER(tomak79_st7789, CONFIG_ZMK_LOG_LEVEL);

#define LCD_WIDTH 240
#define LCD_HEIGHT 280
#define BLOCK_HEIGHT 20

#define RGB565_RED 0xF800
#define RGB565_GREEN 0x07E0
#define RGB565_BLUE 0x001F
#define RGB565_WHITE 0xFFFF
#define RGB565_BLACK 0x0000
#define RGB565_YELLOW 0xFFE0
#define RGB565_CYAN 0x07FF

#define FONT_W 5
#define FONT_H 7
#define FONT_SCALE 3

static uint8_t line_buf[LCD_WIDTH * BLOCK_HEIGHT * 2];
static uint8_t fb[LCD_WIDTH * LCD_HEIGHT * 2];

static const struct device *const display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
static const struct device *const gpio0_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));

static const uint8_t font_5x7[][5] = {
    [' '] = {0x00, 0x00, 0x00, 0x00, 0x00},
    ['0'] = {0x3E, 0x51, 0x49, 0x45, 0x3E},
    ['1'] = {0x00, 0x42, 0x7F, 0x40, 0x00},
    ['2'] = {0x42, 0x61, 0x51, 0x49, 0x46},
    ['3'] = {0x21, 0x41, 0x45, 0x4B, 0x31},
    ['4'] = {0x18, 0x14, 0x12, 0x7F, 0x10},
    ['7'] = {0x01, 0x71, 0x09, 0x05, 0x03},
    ['8'] = {0x36, 0x49, 0x49, 0x49, 0x36},
    ['9'] = {0x06, 0x49, 0x49, 0x29, 0x1E},
    ['A'] = {0x7E, 0x11, 0x11, 0x11, 0x7E},
    ['E'] = {0x7F, 0x49, 0x49, 0x49, 0x41},
    ['F'] = {0x7F, 0x09, 0x09, 0x09, 0x01},
    ['H'] = {0x7F, 0x08, 0x08, 0x08, 0x7F},
    ['I'] = {0x00, 0x41, 0x7F, 0x41, 0x00},
    ['M'] = {0x7F, 0x02, 0x0C, 0x02, 0x7F},
    ['P'] = {0x7F, 0x09, 0x09, 0x09, 0x06},
    ['R'] = {0x7F, 0x09, 0x19, 0x29, 0x46},
    ['S'] = {0x46, 0x49, 0x49, 0x49, 0x31},
    ['T'] = {0x01, 0x01, 0x7F, 0x01, 0x01},
    ['U'] = {0x3F, 0x40, 0x40, 0x40, 0x3F},
    ['V'] = {0x1F, 0x20, 0x40, 0x20, 0x1F},
    ['X'] = {0x63, 0x14, 0x08, 0x14, 0x63},
    ['Y'] = {0x07, 0x08, 0x70, 0x08, 0x07},
};

static void put_rgb565_be_buf(uint8_t *buf, int width, int x, int y, uint16_t color) {
    int idx = (y * width + x) * 2;

    buf[idx] = (color >> 8) & 0xFF;
    buf[idx + 1] = color & 0xFF;
}

static void fb_pixel(int x, int y, uint16_t color) {
    if (x < 0 || x >= LCD_WIDTH || y < 0 || y >= LCD_HEIGHT) {
        return;
    }

    put_rgb565_be_buf(fb, LCD_WIDTH, x, y, color);
}

static void fb_fill(uint16_t color) {
    for (int y = 0; y < LCD_HEIGHT; y++) {
        for (int x = 0; x < LCD_WIDTH; x++) {
            fb_pixel(x, y, color);
        }
    }
}

static void draw_char(int x, int y, char c, uint16_t fg, uint16_t bg) {
    const uint8_t *glyph;

    if ((unsigned char)c >= ARRAY_SIZE(font_5x7)) {
        c = ' ';
    }

    glyph = font_5x7[(unsigned char)c];

    for (int col = 0; col < FONT_W; col++) {
        uint8_t bits = glyph[col];

        for (int row = 0; row < FONT_H; row++) {
            uint16_t color = (bits & BIT(row)) ? fg : bg;

            for (int sy = 0; sy < FONT_SCALE; sy++) {
                for (int sx = 0; sx < FONT_SCALE; sx++) {
                    fb_pixel(x + col * FONT_SCALE + sx, y + row * FONT_SCALE + sy, color);
                }
            }
        }
    }
}

static void draw_text(int x, int y, const char *text, uint16_t fg, uint16_t bg) {
    while (*text != '\0') {
        draw_char(x, y, *text, fg, bg);
        x += (FONT_W + 1) * FONT_SCALE;
        text++;
    }
}

static int flush_fb(void) {
    for (int y = 0; y < LCD_HEIGHT; y += BLOCK_HEIGHT) {
        int h = BLOCK_HEIGHT;

        if ((y + h) > LCD_HEIGHT) {
            h = LCD_HEIGHT - y;
        }

        for (int row = 0; row < h; row++) {
            memcpy(&line_buf[row * LCD_WIDTH * 2], &fb[((y + row) * LCD_WIDTH) * 2],
                   LCD_WIDTH * 2);
        }

        struct display_buffer_descriptor desc = {
            .width = LCD_WIDTH,
            .height = h,
            .pitch = LCD_WIDTH,
            .buf_size = LCD_WIDTH * h * 2,
        };

        int ret = display_write(display_dev, 0, y, &desc, line_buf);
        if (ret != 0) {
            return ret;
        }
    }

    return 0;
}

static void draw_test_screen(void) {
    fb_fill(RGB565_BLACK);

    draw_text(18, 40, "ST7789V3", RGB565_GREEN, RGB565_BLACK);
    draw_text(18, 80, "240X280", RGB565_CYAN, RGB565_BLACK);
    draw_text(18, 120, "ZMK TEST", RGB565_YELLOW, RGB565_BLACK);

    for (int y = 190; y < 240; y++) {
        for (int x = 20; x < 70; x++) {
            fb_pixel(x, y, RGB565_RED);
        }
        for (int x = 75; x < 125; x++) {
            fb_pixel(x, y, RGB565_GREEN);
        }
        for (int x = 130; x < 180; x++) {
            fb_pixel(x, y, RGB565_BLUE);
        }
        for (int x = 185; x < 225; x++) {
            fb_pixel(x, y, RGB565_WHITE);
        }
    }
}

static int backlight_on(void) {
    if (!device_is_ready(gpio0_dev)) {
        return -ENODEV;
    }

    gpio_pin_configure(gpio0_dev, 10, GPIO_OUTPUT_ACTIVE);
    gpio_pin_set(gpio0_dev, 10, 1);
    return 0;
}

static void tomak79_st7789_render_work(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(st7789_render_work, tomak79_st7789_render_work);

static void tomak79_st7789_render_work(struct k_work *work) {
    ARG_UNUSED(work);

    if (!device_is_ready(display_dev)) {
        LOG_ERR("Display not ready");
        return;
    }

    int ret = backlight_on();
    if (ret != 0) {
        LOG_ERR("Backlight failed: %d", ret);
        return;
    }

    ret = display_set_pixel_format(display_dev, PIXEL_FORMAT_RGB_565);
    if (ret != 0 && ret != -ENOTSUP) {
        LOG_ERR("Pixel format failed: %d", ret);
        return;
    }

    ret = display_blanking_off(display_dev);
    if (ret != 0) {
        LOG_ERR("Blanking off failed: %d", ret);
        return;
    }

    draw_test_screen();

    ret = flush_fb();
    if (ret != 0) {
        LOG_ERR("flush_fb failed: %d", ret);
        return;
    }

    LOG_INF("ST7789 test screen drawn");
}

static int tomak79_st7789_init(void) {
    k_work_schedule(&st7789_render_work, K_MSEC(500));
    return 0;
}

SYS_INIT(tomak79_st7789_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
