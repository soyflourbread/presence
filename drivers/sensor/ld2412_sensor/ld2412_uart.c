#include "ld2412_uart.h"

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
#include "ld2412_sensor.h"
#include "ld2412_queue.h"

LOG_MODULE_REGISTER(LD2412Uart, CONFIG_LD2412_SENSOR_LOG_LEVEL);

uint8_t *ld2412_get_rx_buf(LD2412Data *data)
{
    return data->rx_buf[data->rx_buf_slot];
}

void ld2412_push_rx_buf(LD2412Data *data)
{
    data->rx_buf_slot = 1 - data->rx_buf_slot;
}

void ld2412_uart_callback(const struct device *uart_dev, struct uart_event *evt, void *user_data)
{
    const struct device *dev = user_data;
    LD2412Data *data = (LD2412Data *)dev->data;
    LD2412Cfg *cfg = (LD2412Cfg *)dev->config;

	switch (evt->type) {
	case UART_RX_RDY:
        if (evt->data.rx.len < 1) {
            return;
        }
		// LOG_HEXDUMP_DBG(evt->data.rx.buf + evt->data.rx.offset, evt->data.rx.len, "RX_RDY");
        if (ld2412_handle_rx(dev, evt->data.rx.buf + evt->data.rx.offset, evt->data.rx.len) != 0) {
            LOG_ERR("queue handle rx err");
        }
        break;

	case UART_RX_BUF_REQUEST:
        int rc = uart_rx_buf_rsp(uart_dev, ld2412_get_rx_buf(data), CONFIG_LD2412_SENSOR_RX_BUF_SIZE);
		__ASSERT_NO_MSG(rc == 0);
        ld2412_push_rx_buf(data);
		break;

	case UART_RX_DISABLED:
        uart_rx_enable(uart_dev, ld2412_get_rx_buf(data),
            CONFIG_LD2412_SENSOR_RX_BUF_SIZE, cfg->timeout);
        ld2412_push_rx_buf(data);
		break;

	default:
		break;
	}
}

int ld2412_uart_init(const struct device *dev)
{
    LD2412Data *data = (LD2412Data *)dev->data;
    LD2412Cfg *cfg = (LD2412Cfg *)dev->config;
    int err = uart_callback_set(cfg->uart_dev, ld2412_uart_callback, (void*)dev);
    if (err) {
        LOG_ERR("uart callback set err %u", err);
        return -ENODEV;
    }
    uart_rx_enable(cfg->uart_dev, ld2412_get_rx_buf(data),
        CONFIG_LD2412_SENSOR_RX_BUF_SIZE, cfg->timeout);
    ld2412_push_rx_buf(data);
    return 0;
}
