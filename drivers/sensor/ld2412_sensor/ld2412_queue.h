#pragma once

#include <stdint.h>
#include <zephyr/kernel.h>

#define LD2412_QUEUE_THREAD_STACK_SIZE CONFIG_LD2412_SENSOR_THREAD_STACK_SIZE
#define LD2412_QUEUE_THREAD_PRIORITY 7
#define LD2412_QUEUE_LEN 12
#define LD2412_QUEUE_MSG_DATA_LEN 4
#define LD2412_SENSOR_UART_RING_BUF_SIZE CONFIG_LD2412_SENSOR_UART_RING_BUF_SIZE

typedef enum {
    LD2412_QUEUE_MSG_UART_RECV,
    LD2412_QUEUE_MSG_MAX,
} LD2412QueueMsgType;

typedef struct {
    LD2412QueueMsgType msgType;
    uint32_t data[LD2412_QUEUE_MSG_DATA_LEN];
} LD2412QueueMsg;

/**
 * @brief Initialize LD2412 work queue.
 * 
 * @param dev LD2412 presence sensor device.
 */
void ld2412_queue_init(const struct device *dev);

uint32_t ld2412_handle_rx(const struct device *dev, uint8_t *buf, uint32_t len);
