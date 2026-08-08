#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zmk/events/position_state_changed.h>
#include <zmk/rgb_underglow.h>

LOG_MODULE_REGISTER(tomak79_right_underglow_resume, CONFIG_ZMK_LOG_LEVEL);

static void tomak79_right_underglow_restart_work_cb(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(tomak79_right_underglow_restart_work,
                               tomak79_right_underglow_restart_work_cb);

static void tomak79_right_underglow_restart_work_cb(struct k_work *work) {
    ARG_UNUSED(work);

    int err = zmk_rgb_underglow_on();
    if (err != 0) {
        LOG_WRN("Underglow delayed resume failed: %d", err);
    }
}

static int tomak79_right_underglow_resume_listener(const zmk_event_t *eh) {
    const struct zmk_position_state_changed *position_ev = as_zmk_position_state_changed(eh);
    if (position_ev == NULL || !position_ev->state) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    bool underglow_on = false;
    int err = zmk_rgb_underglow_get_state(&underglow_on);
    if (err != 0) {
        LOG_WRN("Underglow state read failed: %d", err);
        return ZMK_EV_EVENT_BUBBLE;
    }

    if (underglow_on) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    err = zmk_rgb_underglow_off();
    if (err != 0) {
        LOG_WRN("Underglow pre-reset failed: %d", err);
        return ZMK_EV_EVENT_BUBBLE;
    }

    err = k_work_reschedule(&tomak79_right_underglow_restart_work, K_MSEC(20));
    if (err < 0) {
        LOG_WRN("Underglow restart schedule failed: %d", err);
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(tomak79_right_underglow_resume_listener, tomak79_right_underglow_resume_listener);
ZMK_SUBSCRIPTION(tomak79_right_underglow_resume_listener, zmk_position_state_changed);
