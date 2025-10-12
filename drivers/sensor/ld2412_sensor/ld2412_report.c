#include "ld2412_report.h"

#include <string.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(LD2412Report, CONFIG_LD2412_SENSOR_LOG_LEVEL);

uint32_t ld2412_report_validate_header(const LD2412ReportHeader *header)
{
    switch (header->data_type) {
    case LD2412_REPORT_DATA_TYPE_ENGINEER:
    case LD2412_REPORT_DATA_TYPE_TARGET:
        break;
    default:
        LOG_ERR("unknown data_type, data_type=0x%02x", header->data_type);
        return 1;
    }
    if (header->head != LD2412_REPORT_HEAD) {
        LOG_ERR("unknown head, head=0x%02x", header->head);
        return 1;
    }
    return 0;
}

uint32_t ld2412_report_parse(uint8_t *data, uint32_t data_len)
{
    uint32_t cursor = 0;
    LD2412ReportHeader header = {0};
    if (cursor + sizeof(header) > data_len) {
        return 1;
    }
    memcpy(&header, data + cursor, sizeof(header));
    cursor += sizeof(header);
    LOG_DBG("header, data_type=0x%02x, head=0x%02x", header.data_type, header.head);
    if (ld2412_report_validate_header(&header) != 0) {
        return 1;
    }
    
    LD2412ReportTargetData target = {0};
    if (cursor + sizeof(target) > data_len) {
        return 1;
    }
    memcpy(&target, data + cursor, sizeof(target));
    cursor += sizeof(target);
    LOG_ERR("target, state=0x%02x, move=(%u, %u), sta=(%u, %u)",
        target.target_state,
        target.movement_target_distance, target.movement_target_energy,
        target.stationary_target_distance, target.stationary_target_energy);
    
    if (header.data_type != LD2412_REPORT_DATA_TYPE_ENGINEER) {
        return 0;
    }

    LD2412ReportTargetEngineerData engineer = {0};
    if (cursor + sizeof(engineer) > data_len) {
        return 1;
    }
    memcpy(&engineer, data + cursor, sizeof(engineer));
    cursor += sizeof(engineer);
    LOG_ERR("engineer, gate=(%u, %u)", engineer.movement_gate_num, engineer.stationary_gate_num);
    LOG_HEXDUMP_ERR(engineer.movement_gate_energy, sizeof(engineer.movement_gate_num), "MOVE_GATE");
    LOG_HEXDUMP_ERR(engineer.stationary_gate_energy, sizeof(engineer.stationary_gate_energy), "STA_GATE");
    return 0;
}