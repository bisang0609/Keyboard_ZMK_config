#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#include <zmk/battery.h>
#include <zmk/hid_indicators.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <zephyr/usb/class/hid.h>

LOG_MODULE_REGISTER(tomak79_right2_oled_status, CONFIG_ZMK_LOG_LEVEL);

#define OLED_WIDTH 128
#define OLED_HEIGHT 32
#define GLYPH_WIDTH 5
#define GLYPH_HEIGHT 7
#define BLOCK_SIZE 8
#define GRID_COLS (OLED_WIDTH / BLOCK_SIZE)
#define GRID_ROWS (OLED_HEIGHT / BLOCK_SIZE)
#define REFRESH_MS 500

static const struct device *const oled = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
static uint8_t last_blocks[GRID_ROWS][GRID_COLS][BLOCK_SIZE];
static bool screen_initialized;

struct glyph_entry {
    char ch;
    uint8_t rows[GLYPH_HEIGHT];
};

static const struct glyph_entry glyphs[] = {
    {' ', {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {'%', {0x19, 0x19, 0x02, 0x04, 0x08, 0x13, 0x13}},
    {':', {0x00, 0x04, 0x04, 0x00, 0x04, 0x04, 0x00}},
    {'0', {0x0e, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0e}},
    {'1', {0x04, 0x0c, 0x04, 0x04, 0x04, 0x04, 0x0e}},
    {'2', {0x0e, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1f}},
    {'3', {0x1f, 0x02, 0x04, 0x02, 0x01, 0x11, 0x0e}},
    {'4', {0x02, 0x06, 0x0a, 0x12, 0x1f, 0x02, 0x02}},
    {'5', {0x1f, 0x10, 0x1e, 0x01, 0x01, 0x11, 0x0e}},
    {'6', {0x06, 0x08, 0x10, 0x1e, 0x11, 0x11, 0x0e}},
    {'7', {0x1f, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08}},
    {'8', {0x0e, 0x11, 0x11, 0x0e, 0x11, 0x11, 0x0e}},
    {'9', {0x0e, 0x11, 0x11, 0x0f, 0x01, 0x02, 0x1c}},
    {'A', {0x0e, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x11}},
    {'C', {0x0e, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0e}},
    {'F', {0x1f, 0x10, 0x10, 0x1e, 0x10, 0x10, 0x10}},
    {'L', {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1f}},
    {'M', {0x11, 0x1b, 0x15, 0x15, 0x11, 0x11, 0x11}},
    {'N', {0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11}},
    {'O', {0x0e, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e}},
    {'P', {0x1e, 0x11, 0x11, 0x1e, 0x10, 0x10, 0x10}},
    {'R', {0x1e, 0x11, 0x11, 0x1e, 0x14, 0x12, 0x11}},
    {'T', {0x1f, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}},
    {'U', {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e}},
    {'Y', {0x11, 0x11, 0x0a, 0x04, 0x04, 0x04, 0x04}},
};

static const uint8_t *glyph_rows_for(char ch) {
    for (size_t i = 0; i < ARRAY_SIZE(glyphs); i++) {
        if (glyphs[i].ch == ch) {
            return glyphs[i].rows;
        }
    }

    return glyphs[0].rows;
}

static int write_block(uint16_t x, uint16_t y, const uint8_t block[BLOCK_SIZE]) {
    struct display_buffer_descriptor desc = {
        .buf_size = BLOCK_SIZE,
        .width = BLOCK_SIZE,
        .height = BLOCK_SIZE,
        .pitch = BLOCK_SIZE,
    };

    return display_write(oled, x, y, &desc, block);
}

static int clear_screen(void) {
    uint8_t block[BLOCK_SIZE] = {0};

    for (uint16_t y = 0; y < OLED_HEIGHT; y += BLOCK_SIZE) {
        for (uint16_t x = 0; x < OLED_WIDTH; x += BLOCK_SIZE) {
            int ret = write_block(x, y, block);

            if (ret != 0) {
                return ret;
            }
        }
    }

    return 0;
}

static void draw_char(uint8_t screen[GRID_ROWS][GRID_COLS][BLOCK_SIZE], uint8_t col, uint8_t row,
                      char ch) {
    const uint8_t *rows = glyph_rows_for(ch);
    uint8_t block[BLOCK_SIZE] = {0};

    if (row >= GRID_ROWS || col >= GRID_COLS) {
        return;
    }

    for (uint8_t glyph_row = 0; glyph_row < GLYPH_HEIGHT; glyph_row++) {
        for (uint8_t glyph_col = 0; glyph_col < GLYPH_WIDTH; glyph_col++) {
            if ((rows[glyph_row] & BIT(GLYPH_WIDTH - 1 - glyph_col)) == 0U) {
                continue;
            }

            block[glyph_col + 1] |= BIT(glyph_row);
        }
    }

    memcpy(screen[row][col], block, BLOCK_SIZE);
}

static void draw_text(uint8_t screen[GRID_ROWS][GRID_COLS][BLOCK_SIZE], uint8_t col, uint8_t row,
                      const char *text) {
    while (*text != '\0' && col < GRID_COLS) {
        draw_char(screen, col, row, *text);
        col++;
        text++;
    }
}

static void draw_battery_value(uint8_t screen[GRID_ROWS][GRID_COLS][BLOCK_SIZE], uint8_t col,
                               uint8_t row, uint8_t percent, bool known) {
    char text[5];

    if (!known) {
        draw_text(screen, col, row, "NC");
        return;
    }

    snprintf(text, sizeof(text), "%u%%", percent);
    draw_text(screen, col, row, text);
}

static void draw_on_off(uint8_t screen[GRID_ROWS][GRID_COLS][BLOCK_SIZE], uint8_t col, uint8_t row,
                        bool enabled) {
    draw_text(screen, col, row, enabled ? "ON" : "OFF");
}

static void build_status_screen(uint8_t screen[GRID_ROWS][GRID_COLS][BLOCK_SIZE]) {
    const zmk_hid_indicators_t indicators = zmk_hid_indicators_get_current_profile();
    const bool caps_on = (indicators & HID_KBD_LED_CAPS_LOCK) != 0U;
    const bool num_on = (indicators & HID_KBD_LED_NUM_LOCK) != 0U;
    const uint8_t self_battery = zmk_battery_state_of_charge();

    memset(screen, 0, sizeof(uint8_t) * GRID_ROWS * GRID_COLS * BLOCK_SIZE);

    draw_text(screen, 0, 0, "LT:");
    draw_battery_value(screen, 3, 0, 0, false);

    draw_text(screen, 8, 0, "CAP:");
    draw_on_off(screen, 12, 0, caps_on);

    draw_text(screen, 0, 1, "RT:");
    draw_battery_value(screen, 3, 1, self_battery, self_battery > 0U);

    draw_text(screen, 8, 1, "NUM:");
    draw_on_off(screen, 12, 1, num_on);
}

static int render_oled(void) {
    uint8_t screen[GRID_ROWS][GRID_COLS][BLOCK_SIZE];
    int ret;

    if (!device_is_ready(oled)) {
        return -ENODEV;
    }

    ret = display_set_pixel_format(oled, PIXEL_FORMAT_MONO10);
    if (ret != 0 && ret != -ENOTSUP) {
        return ret;
    }

    ret = display_blanking_off(oled);
    if (ret != 0) {
        return ret;
    }

    if (!screen_initialized) {
        ret = clear_screen();
        if (ret != 0) {
            return ret;
        }
    }

    build_status_screen(screen);

    for (uint8_t row = 0; row < GRID_ROWS; row++) {
        for (uint8_t col = 0; col < GRID_COLS; col++) {
            if (screen_initialized &&
                memcmp(last_blocks[row][col], screen[row][col], BLOCK_SIZE) == 0) {
                continue;
            }

            ret = write_block(col * BLOCK_SIZE, row * BLOCK_SIZE, screen[row][col]);
            if (ret != 0) {
                return ret;
            }

            memcpy(last_blocks[row][col], screen[row][col], BLOCK_SIZE);
        }
    }

    screen_initialized = true;
    return 0;
}

static void tomak79_right2_oled_render_work(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(oled_render_work, tomak79_right2_oled_render_work);

static void tomak79_right2_oled_render_work(struct k_work *work) {
    ARG_UNUSED(work);

    int ret = render_oled();
    if (ret != 0) {
        LOG_ERR("OLED render failed: %d", ret);
    }

    k_work_schedule(&oled_render_work, K_MSEC(REFRESH_MS));
}

static int tomak79_right2_oled_status_init(void) {
    k_work_schedule(&oled_render_work, K_MSEC(600));
    return 0;
}

SYS_INIT(tomak79_right2_oled_status_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
