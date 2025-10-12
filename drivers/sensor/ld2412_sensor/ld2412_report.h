#pragma once

#include <stdint.h>

#define LD2412_REPORT_GATE_NUM CONFIG_LD2412_SENSOR_GATE_NUM
#define LD2412_REPORT_HEAD 0xAA
#define LD2412_REPORT_END 0x55
#define LD2412_REPORT_CHECK 0x00

typedef enum {
    LD2412_REPORT_DATA_TYPE_ENGINEER = 0x01,
    LD2412_REPORT_DATA_TYPE_TARGET = 0x02,
} LD2412ReportDataType;

typedef struct __attribute__((packed, scalar_storage_order("little-endian"))) {
    uint8_t data_type;
    uint8_t head;
} LD2412ReportHeader;

typedef struct __attribute__((packed, scalar_storage_order("little-endian"))) {
    uint8_t end;
    uint8_t check;
} LD2412ReportFooter;

typedef enum {
    LD2412_REPORT_TARGET_STATE_NONE = 0x00,
    LD2412_REPORT_TARGET_STATE_MOVEMENT = 0x01,
    LD2412_REPORT_TARGET_STATE_STATIONARY = 0x02,
} LD2412ReportTargetState;

typedef struct __attribute__((packed, scalar_storage_order("little-endian"))) {
    uint8_t target_state;
    uint16_t movement_target_distance; // cm
    uint8_t movement_target_energy;
    uint16_t stationary_target_distance; // cm
    uint8_t stationary_target_energy;
} LD2412ReportTargetData;

typedef struct __attribute__((packed, scalar_storage_order("little-endian"))) {
    uint8_t movement_gate_num;
    uint8_t stationary_gate_num;
    uint8_t movement_gate_energy[LD2412_REPORT_GATE_NUM];
    uint8_t stationary_gate_energy[LD2412_REPORT_GATE_NUM];
} LD2412ReportTargetEngineerData;

uint32_t ld2412_report_parse(uint8_t *data, uint32_t data_len);