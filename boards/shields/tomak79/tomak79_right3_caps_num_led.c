#include <hal/nrf_gpio.h>
#include <hal/nrf_pwm.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#ifdef OUT
#undef OUT
#endif

LOG_MODULE_REGISTER(tomak79_right3_caps_num_led, CONFIG_ZMK_LOG_LEVEL);

#define CAPS_NUM_LED_PIN 9
#define CAPS_NUM_LED_COUNT 3
#define LED_BRIGHTNESS 30

/*
 * Match the Adafruit_NeoPixel nRF52 EasyDMA path that worked in Arduino:
 * 16 MHz PWM clock, 1.25 us period, GRB byte order.
 */
#define MAGIC_T0H (0x8000 | 6U)
#define MAGIC_T1H (0x8000 | 13U)
#define PWM_COUNTERTOP_VALUE 20U
#define PWM_SEQ_LEN ((CAPS_NUM_LED_COUNT * 3U * 8U) + 2U)
#define LED_RESET_TIME_US 80U

struct rgb_color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

static struct rgb_color leds[CAPS_NUM_LED_COUNT];
static uint16_t pwm_sequence[PWM_SEQ_LEN];
static NRF_PWM_Type *const caps_num_pwm = NRF_PWM0;
static int64_t rainbow_start_ms;

static void tomak79_right3_caps_num_led_tick(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(caps_num_led_tick_work, tomak79_right3_caps_num_led_tick);

static void caps_num_pwm_init(void) {
    caps_num_pwm->ENABLE = 0;
    caps_num_pwm->PSEL.OUT[0] = NRF_GPIO_PIN_MAP(0, CAPS_NUM_LED_PIN);
    caps_num_pwm->PSEL.OUT[1] = 0xFFFFFFFFUL;
    caps_num_pwm->PSEL.OUT[2] = 0xFFFFFFFFUL;
    caps_num_pwm->PSEL.OUT[3] = 0xFFFFFFFFUL;

    caps_num_pwm->MODE = PWM_MODE_UPDOWN_Up << PWM_MODE_UPDOWN_Pos;
    caps_num_pwm->PRESCALER =
        PWM_PRESCALER_PRESCALER_DIV_1 << PWM_PRESCALER_PRESCALER_Pos;
    caps_num_pwm->COUNTERTOP = PWM_COUNTERTOP_VALUE << PWM_COUNTERTOP_COUNTERTOP_Pos;
    caps_num_pwm->LOOP = PWM_LOOP_CNT_Disabled << PWM_LOOP_CNT_Pos;
    caps_num_pwm->DECODER = (PWM_DECODER_LOAD_Common << PWM_DECODER_LOAD_Pos) |
                            (PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);
    caps_num_pwm->SEQ[0].PTR = (uint32_t)pwm_sequence;
    caps_num_pwm->SEQ[0].CNT = PWM_SEQ_LEN << PWM_SEQ_CNT_CNT_Pos;
    caps_num_pwm->SEQ[0].REFRESH = 0;
    caps_num_pwm->SEQ[0].ENDDELAY = 0;
}

static void fill_pwm_sequence(const struct rgb_color *pixels, size_t count) {
    uint32_t pos = 0;

    for (size_t i = 0; i < count; i++) {
        const uint8_t bytes[3] = {
            pixels[i].g,
            pixels[i].r,
            pixels[i].b,
        };

        for (size_t j = 0; j < ARRAY_SIZE(bytes); j++) {
            for (uint8_t mask = 0x80; mask > 0; mask >>= 1) {
                pwm_sequence[pos++] = (bytes[j] & mask) ? MAGIC_T1H : MAGIC_T0H;
            }
        }
    }

    pwm_sequence[pos++] = 0x8000;
    pwm_sequence[pos++] = 0x8000;
}

static void write_pixels(const struct rgb_color *pixels, size_t count) {
    fill_pwm_sequence(pixels, count);

    /* Hold the line low long enough for the SK6812 latch before each frame. */
    nrf_gpio_cfg_output(CAPS_NUM_LED_PIN);
    nrf_gpio_pin_clear(CAPS_NUM_LED_PIN);
    k_busy_wait(LED_RESET_TIME_US);

    caps_num_pwm->EVENTS_SEQEND[0] = 0;
    caps_num_pwm->ENABLE = 1;
    caps_num_pwm->TASKS_SEQSTART[0] = 1;

    while (!caps_num_pwm->EVENTS_SEQEND[0]) {
        k_yield();
    }

    caps_num_pwm->EVENTS_SEQEND[0] = 0;
    caps_num_pwm->ENABLE = 0;

    /* Reclaim the pin as GPIO low so the LEDs can latch the transmitted frame. */
    nrf_gpio_cfg_output(CAPS_NUM_LED_PIN);
    nrf_gpio_pin_clear(CAPS_NUM_LED_PIN);
    k_busy_wait(LED_RESET_TIME_US);
}

static struct rgb_color hsv_to_rgb(uint16_t hue_deg, uint8_t value) {
    const uint16_t region = (hue_deg / 60U) % 6U;
    const uint16_t remainder = hue_deg % 60U;
    const uint16_t rising = (uint16_t)((value * remainder) / 60U);
    const uint16_t falling = (uint16_t)((value * (60U - remainder)) / 60U);
    struct rgb_color color = {0};

    switch (region) {
    case 0:
        color.r = value;
        color.g = rising;
        break;
    case 1:
        color.r = falling;
        color.g = value;
        break;
    case 2:
        color.g = value;
        color.b = rising;
        break;
    case 3:
        color.g = falling;
        color.b = value;
        break;
    case 4:
        color.r = rising;
        color.b = value;
        break;
    default:
        color.r = value;
        color.b = falling;
        break;
    }

    return color;
}

static void update_rainbow_frame(void) {
    const int64_t now_ms = k_uptime_get();
    const uint32_t elapsed_ms = (uint32_t)(now_ms - rainbow_start_ms);
    const uint16_t base_hue = (uint16_t)((elapsed_ms % 12000U) * 360U / 12000U);

    for (size_t i = 0; i < CAPS_NUM_LED_COUNT; i++) {
        /* Make the 3 LEDs look like one short strip with a moving rainbow gradient. */
        const uint16_t hue = (uint16_t)((base_hue + (i * 40U)) % 360U);
        leds[i] = hsv_to_rgb(hue, LED_BRIGHTNESS);
    }

    write_pixels(leds, CAPS_NUM_LED_COUNT);
}

static void tomak79_right3_caps_num_led_tick(struct k_work *work) {
    ARG_UNUSED(work);

    update_rainbow_frame();
    k_work_schedule(&caps_num_led_tick_work, K_MSEC(40));
}

static int tomak79_right3_caps_num_led_init(void) {
    nrf_gpio_cfg_output(CAPS_NUM_LED_PIN);
    nrf_gpio_pin_clear(CAPS_NUM_LED_PIN);

    caps_num_pwm_init();
    rainbow_start_ms = k_uptime_get();
    update_rainbow_frame();
    k_work_schedule(&caps_num_led_tick_work, K_MSEC(40));
    return 0;
}

SYS_INIT(tomak79_right3_caps_num_led_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
