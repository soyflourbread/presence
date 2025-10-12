#include "ld2412_sensor.h"

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
#include "ld2412_queue.h"
#include "ld2412_uart.h"

#define DT_DRV_COMPAT hilink_ld2412_sensor
#define LD2412_SENSOR_INIT_PRIORITY 41

LOG_MODULE_REGISTER(LD2412, CONFIG_LD2412_SENSOR_LOG_LEVEL);

typedef struct {
    struct mpsc io_q;
} LD2412Producer;

/**
 * @brief Initializes LD2412 presence sensor.
 * 
 * @param dev LD2412 presence sensor device.
 * @return 0 on success, negative error code otherwise.
 */
static int ld2412_init(const struct device *dev)
{
    LOG_ERR("My UART peripheral init");
    LD2412Cfg *cfg = (LD2412Cfg *)dev->config;
    if (!device_is_ready(cfg->uart_dev))
    {
        __ASSERT(false, "UART device not ready");
        return -ENODEV;
    }
    ld2412_queue_init(dev);
    if (ld2412_uart_init(dev))
    {
        __ASSERT(false, "Failed to initialize UART device");
        return -ENODEV;
    }
    LOG_DBG("My UART peripheral initialized");
    return 0;
}

static void ld2412_producer_submit(struct rtio_iodev_sqe *iodev_sqe)
{
	struct mpsc *producer_ioq = iodev_sqe->sqe.iodev->data;
	mpsc_push(producer_ioq, &iodev_sqe->q);
}

#define LD2412_RTIO_DEFINE(inst)                                    \
    static LD2412Producer ld2412_producer_data_##inst = {0}; \
    const struct rtio_iodev_api ld2412_producer_api_##inst = { \
        .submit = ld2412_producer_submit, \
    }; \
	RTIO_IODEV_DEFINE(ld2412_iodev_##inst, \
        &ld2412_producer_api_##inst, &ld2412_producer_data_##inst); \
	RTIO_DEFINE(ld2412_rtio_ctx_##inst, 8, 8);

#define INIT_LD2412_SENSOR_PERIPHERAL(inst)                       \
    static struct k_thread ld2412_queue_thread_##inst; \
    K_THREAD_STACK_DEFINE(ld2412_queue_thread_stack_##inst, LD2412_QUEUE_THREAD_STACK_SIZE); \
    K_MSGQ_DEFINE(ld2412_queue_##inst, sizeof(LD2412QueueMsg), LD2412_QUEUE_LEN, 1); \
    RING_BUF_DECLARE(ld2412_uart_buf_##inst, LD2412_SENSOR_UART_RING_BUF_SIZE);    \
    static LD2412Data ld2412_sensor_data_##inst = {               \
        .thread = &ld2412_queue_thread_##inst, \
        .thread_stack = ld2412_queue_thread_stack_##inst, \
        .msgq = &ld2412_queue_##inst, \
        .ring_buf = &ld2412_uart_buf_##inst, \
        .rx_buf = {{0}, {0}},                                     \
        .rx_buf_slot = 0,                                         \
        .decoder = {0}, \
    };                                                            \
    static LD2412Cfg ld2412_sensor_cfg_##inst = { \
        .uart_dev = DEVICE_DT_GET(DT_INST_BUS(inst)),             \
        .timeout = 100, \
    };                                                            \
    DEVICE_DT_INST_DEFINE(inst,                                   \
                          ld2412_init,                \
                          NULL,                                   \
                          &ld2412_sensor_data_##inst,        \
                          &ld2412_sensor_cfg_##inst,        \
                          POST_KERNEL,                            \
                          LD2412_SENSOR_INIT_PRIORITY,            \
                          NULL);

DT_INST_FOREACH_STATUS_OKAY(INIT_LD2412_SENSOR_PERIPHERAL);