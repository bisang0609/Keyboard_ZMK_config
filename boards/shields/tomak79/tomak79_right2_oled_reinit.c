#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(tomak79_right2_oled_reinit, CONFIG_ZMK_LOG_LEVEL);

#define OLED_NODE DT_NODELABEL(oled)

#if DT_NODE_HAS_STATUS(OLED_NODE, okay)

#define OLED_ADDR DT_REG_ADDR(OLED_NODE)
#define OLED_BUS_NODE DT_BUS(OLED_NODE)
#define OLED_WIDTH DT_PROP(OLED_NODE, width)
#define OLED_HEIGHT DT_PROP(OLED_NODE, height)
#define OLED_PAGES (OLED_HEIGHT / 8)

#define OLED_CTL_CMD 0x00
#define OLED_CTL_DATA 0x40

static const struct i2c_dt_spec oled_i2c = I2C_DT_SPEC_GET(OLED_NODE);
static struct k_work_delayable oled_reinit_work;

static int oled_write_cmds(const uint8_t *cmds, size_t len)
{
    return i2c_burst_write_dt(&oled_i2c, OLED_CTL_CMD, cmds, len);
}

static int oled_write_data(const uint8_t *data, size_t len)
{
    return i2c_burst_write_dt(&oled_i2c, OLED_CTL_DATA, data, len);
}

static void oled_clear_gddram(void)
{
    static uint8_t blank[OLED_WIDTH];

    for (uint8_t page = 0; page < OLED_PAGES; page++) {
        const uint8_t page_cmds[] = {
            0x21, 0x00, OLED_WIDTH - 1,
            0x22, page, page,
        };

        if (oled_write_cmds(page_cmds, sizeof(page_cmds)) != 0) {
            LOG_WRN("OLED page %u command write failed", page);
            return;
        }

        if (oled_write_data(blank, sizeof(blank)) != 0) {
            LOG_WRN("OLED page %u clear write failed", page);
            return;
        }
    }
}

static void oled_reinit_worker(struct k_work *work)
{
    ARG_UNUSED(work);

    if (!device_is_ready(oled_i2c.bus)) {
        LOG_WRN("OLED I2C bus not ready");
        return;
    }

    const uint8_t init1[] = {
        0xAE,
        0xD5, 0x80,
        0xA8, OLED_HEIGHT - 1,
        0x40,
        0x8D, 0x14,
        0x20, 0x00,
        0xA1,
        0xC8,
        0xD3, 0x00,
        0xDA, 0x02,
        0x81, 0x7F,
        0xD9, 0xF1,
        0xDB, 0x20,
        0xA4,
        0xA6,
        0x2E,
        0xAF,
    };

    int err = oled_write_cmds(init1, sizeof(init1));
    if (err != 0) {
        LOG_WRN("OLED reinit failed: %d", err);
        return;
    }

    oled_clear_gddram();
    LOG_INF("OLED reinit sequence sent on bus %s addr 0x%02x",
            oled_i2c.bus->name, OLED_ADDR);
}

static int tomak79_right2_oled_reinit_init(void)
{
    k_work_init_delayable(&oled_reinit_work, oled_reinit_worker);
    k_work_schedule(&oled_reinit_work, K_MSEC(350));
    return 0;
}

SYS_INIT(tomak79_right2_oled_reinit_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif
