#include "ld2412_encode.h"

#include <string.h>
#include <zephyr/logging/log.h>
#include "ld2412_serde.h"

LOG_MODULE_REGISTER(LD2412Encode, CONFIG_LD2412_SENSOR_LOG_LEVEL);

uint32_t ld2412_encode(uint8_t *buf, uint32_t buf_len, uint32_t *out_len,
    uint32_t frame_type, const uint8_t *data, uint32_t data_len)
{
    if (data_len > LD2412_FRAME_DATA_MAX_LEN) {
        LOG_ERR("invalid data len, data_len=%u", data_len);
        return 1;
    }

    uint32_t cursor = 0;
    LD2412FrameHeader header = {0};
    header.type = frame_type;
    header.data_len = data_len;
    if (cursor + sizeof(header) > buf_len) {
        return 1;
    }
    memcpy(buf + cursor, &header, sizeof(header));
    cursor += sizeof(header);

    if (cursor + data_len > buf_len) {
        return 1;
    }
    memcpy(buf + cursor, data, data_len);
    cursor += data_len;

    LD2412FrameFooter footer = {0};
    footer.end = ld2412_get_frame_type_end(frame_type);
    if (cursor + sizeof(footer) > buf_len) {
        return 1;
    }
    memcpy(buf + cursor, &footer, sizeof(footer));
    cursor += sizeof(footer);

    *out_len = cursor;
    return 0;
}