/*
 * libpino tests - util.h
 * 
 * Copyright (c) 2025 Go Kudo
 *
 * This library is licensed under the BSD 3-Clause License.
 * For license details, please refer to the LICENSE file.
 *
 * SPDX-FileCopyrightText: Go Kudo <zeriyoshi@gmail.com>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PINO_TESTS_UTIL_H
#define PINO_TESTS_UTIL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

static inline void generate_random_str(uint8_t *out, size_t size)
{
    size_t i;
    const uint8_t charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const size_t charset_size = sizeof(charset) - 1;

    if (size > 0) {
        for (i = 0; i < size - 1; i++) {
            out[i] = charset[rand() % charset_size];
        }
        out[size - 1] = '\0';
    }
}

static inline void generate_fixed_data(uint8_t *out, size_t size)
{
    size_t i;

    for (i = 0; i < size; i++) {
        out[i] = (uint8_t)(i % UINT8_MAX);
    }
}

static inline void generate_random_data(uint8_t *out, size_t size)
{
    size_t i;

    srand((unsigned int)time(NULL));

    for (i = 0; i < size; i++) {
        out[i] = (uint8_t)(rand() % 256);
    }
}

static inline bool break_data(uint8_t *data, size_t data_size, uint32_t count)
{
    uint32_t corrupted_count;
    size_t pos;

    if (!data || data_size == 0 || count == 0) {
        return false;
    }

    if (count > data_size) {
        count = data_size;
    }
    
    bool *corrupted = (bool *)calloc(data_size, sizeof(bool));
    if (!corrupted) {
        return false;
    }

    srand((unsigned int)time(NULL));
    
    corrupted_count = 0;
    while (corrupted_count < count) {
        pos = rand() % data_size;
        pos %= data_size;
        
        if (!corrupted[pos]) {
            data[pos] ^= 0xFF;
            corrupted[pos] = true;
            corrupted_count++;
        }
    }
    
    free(corrupted);

    return true;
}

static inline void print_hex(uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        printf("%02X", data[i]);
    }
    printf("\n");
}

#endif  /* PINO_TESTS_UTIL_H */
