#include "ld2412_queue.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "ld2412_sensor.h"
#include "ld2412_decode.h"

LOG_MODULE_REGISTER(LD2412Queue, CONFIG_LD2412_SENSOR_LOG_LEVEL);

uint32_t ld2412_proc_queue(LD2412Data *data, LD2412Cfg *cfg, LD2412QueueMsg *msg)
{
    switch (msg->msgType) {
    case LD2412_QUEUE_MSG_UART_RECV:
        while (true) {
            LD2412DecodeEvt evt = ld2412_decoder_feed(&data->decoder, data->ring_buf);
            switch (evt) {
            case LD2412_DECODE_EVT_ACCEPTING_TOKEN:
                break;
            case LD2412_DECODE_EVT_AWAITING_BUF:
                return 0; // buffer exhausted
            case LD2412_DECODE_EVT_FILLED:
                ld2412_decoder_drain(&data->decoder);
                break;
            default: // default to error
                ring_buf_reset(data->ring_buf);
                return 1;
            }
        }
        break;
    default:
        break;
    }
    return 0;
}

void ld2412_run_queue(void *p1, void *p2, void *p3)
{
    const struct device *dev = (const struct device *)p1;
    LD2412Data *data = (LD2412Data *)dev->data;
    LD2412Cfg *cfg = (LD2412Cfg *)dev->config;
    LD2412QueueMsg msg = {0};
    while (true) {
        k_msgq_get(data->msgq, &msg, K_FOREVER);
        if (ld2412_proc_queue(data, cfg, &msg) != 0) {
            LOG_ERR("proc msg failed, msgType=%u", msg.msgType);
        }
    }
}

void ld2412_queue_init(const struct device *dev)
{
    LD2412Data *data = (LD2412Data *)dev->data;
    k_thread_create(data->thread, data->thread_stack, LD2412_QUEUE_THREAD_STACK_SIZE,
        ld2412_run_queue, dev, NULL, NULL, LD2412_QUEUE_THREAD_PRIORITY, 0, K_NO_WAIT);
}

uint32_t ld2412_handle_rx(const struct device *dev, uint8_t *buf, uint32_t len)
{
    LD2412Data *data = (LD2412Data *)dev->data;
    uint32_t count = ring_buf_put(data->ring_buf, buf, len);
    if (count != len) {
        // TODO: partial copy
        LOG_ERR("pushed partial copy");
    }
    LD2412QueueMsg msg = {0};
    msg.msgType = LD2412_QUEUE_MSG_UART_RECV;
    msg.data[0] = count;
    if (k_msgq_put(data->msgq, &msg, K_NO_WAIT) != 0) {
        LOG_ERR("push to queue failed");
        return 1;
    }
    return 0;
}
