# Copyright (c) 2021 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_sources(ld2412_sensor.c
    ld2412_uart.c
    ld2412_queue.c
    ld2412_serde.c
    ld2412_decode.c
    ld2412_encode.c
    ld2412_report.c
)
