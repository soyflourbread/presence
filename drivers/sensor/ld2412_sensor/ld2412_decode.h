#pragma once

#include <stdint.h>
#include <zephyr/kernel.h>
#include "ld2412_serde.h"

typedef enum {
    LD2412_DECODE_STATE_INIT = 0,
    LD2412_DECODE_STATE_HEADER,
    LD2412_DECODE_STATE_DATA,
    LD2412_DECODE_STATE_FOOTER,
    LD2412_DECODE_STATE_FILLED,
    LD2412_DECODE_STATE_MAX,
} LD2412DecodeStates;

typedef struct {
    LD2412DecodeStates state;
    uint32_t cursor;

    LD2412FrameHeader header;
    LD2412FrameFooter footer;
    uint8_t data[LD2412_FRAME_DATA_MAX_LEN];
} LD2412Decoder;

typedef enum {
    LD2412_DECODE_EVT_ACCEPTING_TOKEN = 0,
    LD2412_DECODE_EVT_FILLED,
    LD2412_DECODE_EVT_AWAITING_BUF,
    LD2412_DECODE_EVT_HEADER_TYPE_MALFORMED,
    LD2412_DECODE_EVT_HEADER_FOOTER_MISMATCH,
    LD2412_DECODE_EVT_DATA_LEN_ILLEGAL,
    LD2412_DECODE_EVT_MAX,
} LD2412DecodeEvt;

LD2412DecodeEvt ld2412_decoder_feed(LD2412Decoder *self, struct ring_buf *ring_buf);
void ld2412_decoder_drain(LD2412Decoder *self);