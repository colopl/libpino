/*
 * libpino test - test_basic.c
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

#include "../src/pino_internal.h"

#include "handler_spl1.h"
#include "util.h"

#include "unity.h"

#define TEST_DATA_SIZE 1024

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

void test_register(void)
{
    TEST_ASSERT_TRUE(pino_handler_register("spl2", &g_ph_handler_spl1_obj));
    TEST_ASSERT_TRUE(pino_handler_register("spl3", &g_ph_handler_spl1_obj));

    TEST_ASSERT_TRUE(pino_handler_unregister("spl2"));
    TEST_ASSERT_TRUE(pino_handler_register("spl2", &g_ph_handler_spl1_obj));
    TEST_ASSERT_TRUE(pino_handler_unregister("spl2"));

    TEST_ASSERT_TRUE(pino_handler_register("spl4", &g_ph_handler_spl1_obj));

    TEST_ASSERT_TRUE(pino_handler_unregister("spl3"));
    TEST_ASSERT_TRUE(pino_handler_unregister("spl4"));
}

void test_register_fail(void)
{
    TEST_ASSERT_FALSE(pino_handler_register("spl1", &g_ph_handler_spl1_obj));
    TEST_ASSERT_FALSE(pino_handler_register("spl2", NULL));
    TEST_ASSERT_FALSE(pino_handler_register(NULL, &g_ph_handler_spl1_obj));
    TEST_ASSERT_FALSE(pino_handler_register("sapporo", &g_ph_handler_spl1_obj));
    TEST_ASSERT_FALSE(pino_handler_register("tky\0", &g_ph_handler_spl1_obj));
    TEST_ASSERT_FALSE(pino_handler_register("\0abc", &g_ph_handler_spl1_obj));
    TEST_ASSERT_FALSE(pino_handler_register("a\0b\0", &g_ph_handler_spl1_obj));
}

void test_register_unregistered(void)
{
    TEST_ASSERT_TRUE(pino_handler_register("spl2", &g_ph_handler_spl1_obj));
    TEST_ASSERT_TRUE(pino_handler_register("spl3", &g_ph_handler_spl1_obj));
    TEST_ASSERT_TRUE(pino_handler_register("spl4", &g_ph_handler_spl1_obj));
    TEST_ASSERT_TRUE(pino_handler_register("spl5", &g_ph_handler_spl1_obj));
}

void test_register_glowing(void)
{
    pino_magic_safe_t magic;
    size_t i;

    for (i = 0; i < 1000; i++) {
        sprintf(magic, "%04zu", i);
        TEST_ASSERT_TRUE(pino_handler_register(magic, &g_ph_handler_spl1_obj));
    }
}

void test_pack(void)
{
    pino_t *pino;
    uint8_t *data, *unpacked_data;

    data = (uint8_t *)malloc(TEST_DATA_SIZE);
    TEST_ASSERT_NOT_NULL(data);

    generate_random_data(data, TEST_DATA_SIZE);

    pino = pino_pack("spl1", data, TEST_DATA_SIZE);
    TEST_ASSERT_NOT_NULL(pino);

    TEST_ASSERT_EQUAL_size_t(TEST_DATA_SIZE, pino_unpack_size(pino));

    unpacked_data = (uint8_t *)malloc(pino_unpack_size(pino));
    TEST_ASSERT_NOT_NULL(unpacked_data);

    TEST_ASSERT_TRUE(pino_unpack(pino, unpacked_data));
    TEST_ASSERT_EQUAL_MEMORY(data, unpacked_data, TEST_DATA_SIZE);

    pino_destroy(pino);
    free(data);
    free(unpacked_data);
}

void test_pack_fail(void)
{
    pino_t *pino;
    uint8_t *data;

    data = (uint8_t *)malloc(TEST_DATA_SIZE);
    TEST_ASSERT_NOT_NULL(data);

    generate_random_data(data, TEST_DATA_SIZE);

    pino = pino_pack("spl2", data, TEST_DATA_SIZE);
    TEST_ASSERT_NULL(pino);

    pino = pino_pack("spl3", data, TEST_DATA_SIZE);
    TEST_ASSERT_NULL(pino);

    pino = pino_pack("spl4", data, TEST_DATA_SIZE);
    TEST_ASSERT_NULL(pino);

    free(data);
}

void test_pack_glowing(void)
{
    pino_t **pinos;
    uint8_t *data;
    size_t i;

    data = (uint8_t *)malloc(TEST_DATA_SIZE);
    TEST_ASSERT_NOT_NULL(data);

    generate_random_data(data, TEST_DATA_SIZE);

    pinos = (pino_t **)malloc(sizeof(pino_t *) * 1000);
    TEST_ASSERT_NOT_NULL(pinos);

    for (i = 0; i < 1000; i++) {
        pinos[i] = pino_pack("spl1", data, TEST_DATA_SIZE);
        TEST_ASSERT_NOT_NULL(pinos[i]);
    }

    for (i = 0; i < 1000; i++) {
        pino_destroy(pinos[i]);
    }

    free(data);
    free(pinos);
}

void test_pino_serialize(void)
{
    pino_t *pino, *unserialized_pino;
    uint8_t *data, *serialized_data, *unserialized_data;
    size_t serialize_size;

    data = (uint8_t *)malloc(TEST_DATA_SIZE);
    TEST_ASSERT_NOT_NULL(data);

    generate_random_data(data, TEST_DATA_SIZE);

    pino = pino_pack("spl1", data, TEST_DATA_SIZE);
    TEST_ASSERT_NOT_NULL(pino);

    serialize_size = pino_serialize_size(pino);
    TEST_ASSERT_GREATER_THAN_size_t(TEST_DATA_SIZE, serialize_size);

    serialized_data = (uint8_t *)malloc(serialize_size);
    TEST_ASSERT_NOT_NULL(serialized_data);

    TEST_ASSERT_TRUE(pino_serialize(pino, serialized_data));

    unserialized_pino = pino_unserialize(serialized_data, serialize_size);
    TEST_ASSERT_NOT_NULL(unserialized_pino);

    TEST_ASSERT_EQUAL_size_t(TEST_DATA_SIZE, pino_unpack_size(unserialized_pino));

    unserialized_data = (uint8_t *)malloc(pino_unpack_size(unserialized_pino));
    TEST_ASSERT_NOT_NULL(unserialized_data);

    TEST_ASSERT_TRUE(pino_unpack(unserialized_pino, unserialized_data));
    TEST_ASSERT_EQUAL_MEMORY(data, unserialized_data, TEST_DATA_SIZE);

    pino_destroy(pino);
    pino_destroy(unserialized_pino);
    free(data);
    free(serialized_data);
    free(unserialized_data);
}

void test_version_id(void)
{
    TEST_ASSERT_EQUAL_UINT32(PINO_VERSION_ID, pino_version_id());
}

void test_buildtime(void)
{
    TEST_ASSERT_GREATER_THAN(0, pino_buildtime());
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_register);
    RUN_TEST(test_register_fail);
    RUN_TEST(test_register_unregistered);
    RUN_TEST(test_register_glowing);
    RUN_TEST(test_pack);
    RUN_TEST(test_pack_fail);
    RUN_TEST(test_pack_glowing);
    RUN_TEST(test_pino_serialize);

    RUN_TEST(test_version_id);
    RUN_TEST(test_buildtime);

    return UNITY_END();
}
