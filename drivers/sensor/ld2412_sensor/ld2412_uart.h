#pragma once

#include <stdint.h>
#include <zephyr/device.h>

/**
 * @brief Initializes the uart bus for the UART peripheral.
 * 
 * @param dev LD2412 device.
 * @return 0 if successful, negative errno value otherwise.
 */
int ld2412_uart_init(const struct device *dev);
