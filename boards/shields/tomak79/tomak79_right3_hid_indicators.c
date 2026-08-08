#include <zephyr/logging/log.h>

#include <zmk/endpoints_types.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/hid_indicators.h>

LOG_MODULE_REGISTER(tomak79_right3_hid_indicators, CONFIG_ZMK_LOG_LEVEL);

static zmk_hid_indicators_t current_indicators;

zmk_hid_indicators_t zmk_hid_indicators_get_current_profile(void) { return current_indicators; }

zmk_hid_indicators_t zmk_hid_indicators_get_profile(struct zmk_endpoint_instance endpoint) {
    ARG_UNUSED(endpoint);
    return current_indicators;
}

void zmk_hid_indicators_set_profile(zmk_hid_indicators_t indicators,
                                    struct zmk_endpoint_instance endpoint) {
    ARG_UNUSED(endpoint);
    current_indicators = indicators;
}

void zmk_hid_indicators_process_report(struct zmk_hid_led_report_body *report,
                                       struct zmk_endpoint_instance endpoint) {
    ARG_UNUSED(endpoint);
    current_indicators = (zmk_hid_indicators_t)report->leds;
}

static int tomak79_right3_hid_indicators_listener(const zmk_event_t *eh) {
    const struct zmk_hid_indicators_changed *ev = as_zmk_hid_indicators_changed(eh);

    if (ev == NULL) {
        return 0;
    }

    current_indicators = ev->indicators;
    return 0;
}

ZMK_LISTENER(tomak79_right3_hid_indicators_listener, tomak79_right3_hid_indicators_listener);
ZMK_SUBSCRIPTION(tomak79_right3_hid_indicators_listener, zmk_hid_indicators_changed);
