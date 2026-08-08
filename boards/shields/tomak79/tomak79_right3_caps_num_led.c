#include <hal/nrf_gpio.h>
#include <hal/nrf_pwm.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/hid.h>
#include <zmk/hid_indicators.h>

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

struct rgb_color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

static struct rgb_color leds[CAPS_NUM_LED_COUNT];
static uint16_t pwm_sequence[PWM_SEQ_LEN];
static NRF_PWM_Type *const caps_num_pwm = NRF_PWM0;

static void tomak79_right3_caps_num_led_update_work(struct k_work *work);
static K_WORK_DEFINE(caps_num_led_update_work, tomak79_right3_caps_num_led_update_work);

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

    caps_num_pwm->EVENTS_SEQEND[0] = 0;
    caps_num_pwm->ENABLE = 1;
    caps_num_pwm->TASKS_SEQSTART[0] = 1;

    while (!caps_num_pwm->EVENTS_SEQEND[0]) {
        k_yield();
    }

    caps_num_pwm->EVENTS_SEQEND[0] = 0;
    caps_num_pwm->ENABLE = 0;
}

static void update_caps_num_leds(zmk_hid_indicators_t indicators) {
    const bool caps_on = (indicators & HID_KBD_LED_CAPS_LOCK) != 0U;
    const bool num_on = (indicators & HID_KBD_LED_NUM_LOCK) != 0U;

    memset(leds, 0, sizeof(leds));

    if (caps_on) {
        leds[0].r = LED_BRIGHTNESS;
    }

    if (num_on) {
        leds[2].b = LED_BRIGHTNESS;
    }

    if (caps_on && num_on) {
        leds[1].r = LED_BRIGHTNESS;
        leds[1].b = LED_BRIGHTNESS;
    } else if (caps_on) {
        leds[1].r = LED_BRIGHTNESS;
    } else if (num_on) {
        leds[1].b = LED_BRIGHTNESS;
    }

    write_pixels(leds, CAPS_NUM_LED_COUNT);
}

static void tomak79_right3_caps_num_led_update_work(struct k_work *work) {
    ARG_UNUSED(work);

    update_caps_num_leds(zmk_hid_indicators_get_current_profile());
}

static int tomak79_right3_caps_num_led_listener(const zmk_event_t *eh) {
    const struct zmk_hid_indicators_changed *ev = as_zmk_hid_indicators_changed(eh);

    if (ev == NULL) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    k_work_submit(&caps_num_led_update_work);
    return ZMK_EV_EVENT_BUBBLE;
}

static int tomak79_right3_caps_num_led_init(void) {
    nrf_gpio_cfg_output(CAPS_NUM_LED_PIN);
    nrf_gpio_pin_clear(CAPS_NUM_LED_PIN);

    caps_num_pwm_init();
    update_caps_num_leds(zmk_hid_indicators_get_current_profile());
    return 0;
}

ZMK_LISTENER(tomak79_right3_caps_num_led_listener, tomak79_right3_caps_num_led_listener);
ZMK_SUBSCRIPTION(tomak79_right3_caps_num_led_listener, zmk_hid_indicators_changed);

SYS_INIT(tomak79_right3_caps_num_led_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
