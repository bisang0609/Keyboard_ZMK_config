#include <zephyr/device.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zmk/events/hid_indicators_changed.h>

#include <zephyr/usb/class/hid.h>

LOG_MODULE_REGISTER(tomak79_right3_caps_led, CONFIG_ZMK_LOG_LEVEL);

#define CAPS_LED_COUNT 4

static const struct device *const caps_led_strip = DEVICE_DT_GET(DT_NODELABEL(caps_led_strip));

static int update_caps_leds(bool enabled) {
    struct led_rgb pixels[CAPS_LED_COUNT] = {0};

    if (!device_is_ready(caps_led_strip)) {
        return -ENODEV;
    }

    if (enabled) {
        for (size_t i = 0; i < CAPS_LED_COUNT; i++) {
            pixels[i].r = 0x30;
            pixels[i].g = 0x30;
            pixels[i].b = 0x30;
        }
    }

    return led_strip_update_rgb(caps_led_strip, pixels, CAPS_LED_COUNT);
}

static void boot_test_caps_leds(void) {
    int err = update_caps_leds(true);
    if (err != 0) {
        LOG_WRN("Caps LED boot-on failed: %d", err);
        return;
    }

    k_msleep(500);

    err = update_caps_leds(false);
    if (err != 0) {
        LOG_WRN("Caps LED boot-off failed: %d", err);
    }
}

static int tomak79_right3_caps_led_listener(const zmk_event_t *eh) {
    const struct zmk_hid_indicators_changed *ev = as_zmk_hid_indicators_changed(eh);

    if (ev == NULL) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    const bool caps_on = (ev->indicators & HID_KBD_LED_CAPS_LOCK) != 0U;
    int err = update_caps_leds(caps_on);
    if (err != 0) {
        LOG_WRN("Caps LED update failed: %d", err);
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(tomak79_right3_caps_led_listener, tomak79_right3_caps_led_listener);
ZMK_SUBSCRIPTION(tomak79_right3_caps_led_listener, zmk_hid_indicators_changed);

static int tomak79_right3_caps_led_init(void) {
    int err = update_caps_leds(false);
    if (err != 0 && err != -ENODEV) {
        LOG_WRN("Caps LED init failed: %d", err);
    }

    if (err == 0) {
        boot_test_caps_leds();
    }

    return 0;
}

SYS_INIT(tomak79_right3_caps_led_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
