#pragma once

#include <stdint.h>

uint32_t ld2412_encode(uint8_t *buf, uint32_t buf_len, uint32_t *out_len,
    uint32_t frame_type, const uint8_t *data, uint32_t data_len);