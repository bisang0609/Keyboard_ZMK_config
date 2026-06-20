#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zmk/battery.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>

#if IS_ENABLED(CONFIG_BT_BAS)
#include <zephyr/bluetooth/services/bas.h>
#endif

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_CHOSEN(zmk_battery)
static const struct device *const tomak79_battery = DEVICE_DT_GET(DT_CHOSEN(zmk_battery));
#else
static const struct device *const tomak79_battery = NULL;
#endif

static void tomak79_battery_refresh_work_cb(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(tomak79_battery_refresh_work, tomak79_battery_refresh_work_cb);

static void tomak79_battery_refresh_schedule(void) {
    k_work_reschedule(&tomak79_battery_refresh_work,
                      K_SECONDS(CONFIG_ZMK_BATTERY_REPORT_INTERVAL));
}

static void tomak79_battery_refresh_work_cb(struct k_work *work) {
    ARG_UNUSED(work);

    if (tomak79_battery == NULL || !device_is_ready(tomak79_battery)) {
        tomak79_battery_refresh_schedule();
        return;
    }

    struct sensor_value state_of_charge;
    int rc = sensor_sample_fetch_chan(tomak79_battery, SENSOR_CHAN_GAUGE_STATE_OF_CHARGE);
    if (rc != 0) {
        LOG_DBG("Battery refresh fetch failed: %d", rc);
        tomak79_battery_refresh_schedule();
        return;
    }

    rc = sensor_channel_get(tomak79_battery, SENSOR_CHAN_GAUGE_STATE_OF_CHARGE, &state_of_charge);
    if (rc != 0) {
        LOG_DBG("Battery refresh channel get failed: %d", rc);
        tomak79_battery_refresh_schedule();
        return;
    }

    uint8_t current = (uint8_t)state_of_charge.val1;

#if IS_ENABLED(CONFIG_SHIELD_TOMAK79_LEFT)
    LOG_INF("LEFT battery: %u%%", current);
#elif IS_ENABLED(CONFIG_SHIELD_TOMAK79_RIGHT)
    LOG_INF("RIGHT battery: %u%%", current);
#endif

    if (current != zmk_battery_state_of_charge()) {
        raise_zmk_battery_state_changed(
            (struct zmk_battery_state_changed){.state_of_charge = current});

#if IS_ENABLED(CONFIG_BT_BAS)
        if (bt_bas_get_battery_level() != current) {
            bt_bas_set_battery_level(current);
        }
#endif
    }

    tomak79_battery_refresh_schedule();
}

static int tomak79_battery_refresh_init(void) {
    tomak79_battery_refresh_schedule();
    return 0;
}

SYS_INIT(tomak79_battery_refresh_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
