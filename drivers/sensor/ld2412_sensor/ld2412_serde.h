#pragma once

#include <stdint.h>

#define LD2412_FRAME_DATA_MAX_LEN CONFIG_LD2412_SENSOR_FRAME_DATA_MAX_SIZE

#define LD2412_FRAME_TYPE_CMD 0xFAFBFCFD
#define LD2412_FRAME_TYPE_REPORT 0xF1F2F3F4

uint32_t ld2412_get_frame_type_end(uint32_t frame_type);

typedef struct __attribute__((packed, scalar_storage_order("little-endian"))) {
    uint32_t type;
    uint16_t data_len;
} LD2412FrameHeader;

typedef struct __attribute__((packed, scalar_storage_order("little-endian"))) {
    uint32_t end;
} LD2412FrameFooter;
