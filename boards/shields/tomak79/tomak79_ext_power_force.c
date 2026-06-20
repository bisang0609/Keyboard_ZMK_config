#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <drivers/ext_power.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static void tomak79_force_ext_power_work(struct k_work *work) {
    ARG_UNUSED(work);

    const struct device *ext_power = device_get_binding("EXT_POWER");
    if (ext_power == NULL) {
        LOG_ERR("Unable to retrieve EXT_POWER device");
        return;
    }

    int rc = ext_power_enable(ext_power);
    if (rc != 0) {
        LOG_ERR("Failed to enable EXT_POWER: %d", rc);
        return;
    }

    LOG_DBG("EXT_POWER forced on");
}

static K_WORK_DELAYABLE_DEFINE(tomak79_force_ext_power_dwork, tomak79_force_ext_power_work);

static int tomak79_force_ext_power_init(void) {
    /* Let settings restore finish first, then force P0.13 back on for MAX17048 power. */
    k_work_schedule(&tomak79_force_ext_power_dwork, K_MSEC(1500));
    return 0;
}

SYS_INIT(tomak79_force_ext_power_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
