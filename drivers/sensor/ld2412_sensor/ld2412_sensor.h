#pragma once

#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>

#include "ld2412_decode.h"

#define LD2412_SENSOR_RX_BUF_SLOT_NUM 2

/**
 * @brief Contains runtime mutable data for the peripheral.
 */
typedef struct {
    // Thread runtime data
    struct k_thread *thread;
    k_thread_stack_t *thread_stack;
    // Message queue runtime data
    struct k_msgq *msgq;
    // UART queue runtime data
    struct ring_buf *ring_buf;
    // UART IRQ runtime data
    uint8_t rx_buf[LD2412_SENSOR_RX_BUF_SLOT_NUM][CONFIG_LD2412_SENSOR_RX_BUF_SIZE];
    uint32_t rx_buf_slot;
    // Decoder FSM data
    LD2412Decoder decoder;
} LD2412Data;

/**
 * @brief Build time configurations for the peripheral.
 */
typedef struct {
    const struct device *uart_dev;
    uint32_t timeout;
} LD2412Cfg;
