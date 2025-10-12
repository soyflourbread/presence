#include "ld2412_serde.h"

uint32_t ld2412_get_frame_type_end(uint32_t frame_type)
{
    switch (frame_type) {
    case LD2412_FRAME_TYPE_CMD:
        return 0x01020304;
    case LD2412_FRAME_TYPE_REPORT:
        return 0xF5F6F7F8;
    default:
        return 0;
    }
}
