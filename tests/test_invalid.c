/*
 * libpino test - test_invalid.c
 * 
 * Copyright (c) 2025 Go Kudo
 *
 * This library is licensed under the BSD 3-Clause License.
 * For license details, please refer to the LICENSE file.
 *
 * SPDX-FileCopyrightText: Go Kudo <zeriyoshi@gmail.com>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <pino.h>
#include <pino/handler.h>
#include <pino/endianness.h>

#include "handler_spl1.h"

#include "util.h"

#include "unity.h"

#define TEST_DATA_SIZE 1024

typedef char test_t[129];

void setUp(void)
{
    if (!pino_init() || !PH_REG(spl1)) {
        TEST_FAIL();
    }
}

void tearDown(void)
{
    if (!PH_UNREG(spl1)) {
        TEST_FAIL();
    }

    pino_free();
}

void test_pino(void)
{
    TEST_ASSERT_EQUAL_size_t(0, pino_serialize_size(NULL));
    TEST_ASSERT_FALSE(pino_serialize(NULL, NULL));
    TEST_ASSERT_NULL(pino_unserialize(NULL, 0));
    /* todo: pino_unserialize: invalid magic */
    /* todo: pino_unserialize: unregistered magic */
    TEST_ASSERT_NULL(pino_pack("abc\0", NULL, 0));
    /* todo: pino_pack: pino_create fail */
    /* todo: pino_pack: handler->pack fail */
    pino_destroy(NULL);
}

void test_endianness(void)
{
    char i = 1, j = 0, b[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, u[10] = {0};

    pino_endianness_memcpy_le2native(&i, &j, sizeof(char));
    pino_endianness_memcpy_be2native(&i, &j, sizeof(char));
    pino_endianness_memcpy_native2le(&i, &j, sizeof(char));
    pino_endianness_memcpy_native2be(&i, &j, sizeof(char));

    pino_endianness_memcpy_le2native(u + 1, b, 2);
    pino_endianness_memcpy_be2native(u + 1, b, 2);
    pino_endianness_memcpy_native2le(u + 1, b, 2);
    pino_endianness_memcpy_native2be(u + 1, b, 2);

    TEST_ASSERT_EQUAL_CHAR(0, i);
    TEST_ASSERT_EQUAL_CHAR(0, j);
}

void test_handler(void)
{
    TEST_ASSERT_FALSE(PH_REG(spl1)); /* already registered */
    TEST_ASSERT_FALSE(pino_handler_unregister("a\0\0\0"));
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_pino);
    RUN_TEST(test_endianness);
    RUN_TEST(test_handler);

    return UNITY_END();
}
