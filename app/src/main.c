/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/devicetree.h>

#include <app/drivers/blink.h>
#include <app/drivers/ld2412.h>

#include <app_version.h>

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

#define BLINK_PERIOD_MS_STEP 100U
#define BLINK_PERIOD_MS_MAX  1000U

struct counter
{
    int strings;
    int overflows;
};

static struct counter tracker = {0};

void peripheral_callback(const struct device *dev, char *data, size_t length, bool is_string, void *user_data)
{
    struct counter *c = (struct counter *)user_data;
    if (is_string)
    {
        printk("Recieved string \"%s\"\n", data);
        c->strings++;
    } else {
        printk("Buffer full. Recieved fragment %.*s\n", length, data);
        c->overflows++;
    }
    printk("Strings: %d\nOverflows: %d\n", c->strings, c->overflows);
}

int main(void)
{
	int ret;
	unsigned int period_ms = BLINK_PERIOD_MS_MAX;
	const struct device *sensor, *blink;
	struct sensor_value last_val = { 0 }, val;

	printk("Zephyr Example Application %s\n", APP_VERSION_STRING);

	sensor = DEVICE_DT_GET(DT_NODELABEL(example_sensor));
	if (!device_is_ready(sensor)) {
		LOG_ERR("Sensor not ready");
		return 0;
	}

	blink = DEVICE_DT_GET(DT_NODELABEL(blink_led));
	if (!device_is_ready(blink)) {
		LOG_ERR("Blink LED not ready");
		return 0;
	}

	const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(presence_sensor));

	while (1) {
		k_sleep(K_MSEC(100));
	}

	return 0;
}

