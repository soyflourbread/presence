#include "ld2412_decode.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "ld2412_report.h"

LOG_MODULE_REGISTER(LD2412Decode, CONFIG_LD2412_SENSOR_LOG_LEVEL);

void ld2412_decoder_transition(LD2412Decoder *self, LD2412DecodeStates state_next)
{
    if (self->state == state_next) {
        return;
    }
    LOG_DBG("decode FSM transition, state=0x%x, state_next=0x%x", self->state, state_next);
    self->state = state_next;
    self->cursor = 0;
}

LD2412DecodeEvt ld2412_decoder_feed(LD2412Decoder *self, struct ring_buf *ring_buf)
{
    switch (self->state) {
    case LD2412_DECODE_STATE_INIT:
        ld2412_decoder_transition(self, LD2412_DECODE_STATE_HEADER);
    case LD2412_DECODE_STATE_HEADER: {
        if (ring_buf_size_get(ring_buf) < sizeof(self->header)) {
            return LD2412_DECODE_EVT_AWAITING_BUF;
        }
        uint32_t count = ring_buf_get(ring_buf, (uint8_t *)&self->header, sizeof(self->header));
        __ASSERT_NO_MSG(count == sizeof(self->header));
        if (self->header.type != LD2412_FRAME_TYPE_CMD && self->header.type != LD2412_FRAME_TYPE_REPORT) {
            LOG_ERR("malformed frame type, type=0x%08x", self->header.type);
            ld2412_decoder_transition(self, LD2412_DECODE_STATE_INIT);
            return LD2412_DECODE_EVT_HEADER_TYPE_MALFORMED;
        }
        if (self->header.data_len > LD2412_FRAME_DATA_MAX_LEN) {
            LOG_ERR("invalid data len, data_len=%u", self->header.data_len);
            ld2412_decoder_transition(self, LD2412_DECODE_STATE_INIT);
            return LD2412_DECODE_EVT_DATA_LEN_ILLEGAL;
        }
        ld2412_decoder_transition(self, LD2412_DECODE_STATE_DATA);
        break;
    }
    case LD2412_DECODE_STATE_DATA: {
        uint32_t count_expect = self->header.data_len - self->cursor;
        uint32_t count = ring_buf_get(ring_buf, self->data + self->cursor, count_expect);
        self->cursor += count;
        if (self->cursor < self->header.data_len) {
            return LD2412_DECODE_EVT_AWAITING_BUF;
        }
        ld2412_decoder_transition(self, LD2412_DECODE_STATE_FOOTER);
        break;
    }
    case LD2412_DECODE_STATE_FOOTER: {
        if (ring_buf_size_get(ring_buf) < sizeof(self->footer)) {
            return LD2412_DECODE_EVT_AWAITING_BUF;
        }
        uint32_t count = ring_buf_get(ring_buf, (uint8_t *)&self->footer, sizeof(self->footer));
        __ASSERT_NO_MSG(count == sizeof(self->footer));
        if (ld2412_get_frame_type_end(self->header.type) != self->footer.end) {
            LOG_ERR("header type/footer end mismatch, type=0x%08x, end=0x%08x",
                self->header.type, self->footer.end);
            ld2412_decoder_transition(self, LD2412_DECODE_STATE_INIT);
            return LD2412_DECODE_EVT_HEADER_FOOTER_MISMATCH;
        }
        ld2412_decoder_transition(self, LD2412_DECODE_STATE_FILLED);
        return LD2412_DECODE_EVT_FILLED;
    }
    default:
        break;
    }
    return LD2412_DECODE_EVT_ACCEPTING_TOKEN;
}

uint32_t ld2412_decoder_parse_data(uint32_t frame_type, uint8_t *data, uint32_t data_len)
{
    switch (frame_type) {
    case LD2412_FRAME_TYPE_REPORT:
        return ld2412_report_parse(data, data_len);
    default:
        return 1;
    }
    return 0;
}

void ld2412_decoder_drain(LD2412Decoder *self)
{
    if (self->state != LD2412_DECODE_STATE_FILLED) {
        LOG_ERR("nothing to drain");
        return;
    }
    LOG_DBG("draining frame, type=0x%08x, data_len=%u, end=0x%08x",
        self->header.type, self->header.data_len, self->footer.end);
    LOG_HEXDUMP_DBG(self->data, self->header.data_len, "FRAME_DATA");
    ld2412_decoder_parse_data(self->header.type, self->data, self->header.data_len);
    ld2412_decoder_transition(self, LD2412_DECODE_STATE_INIT);
}