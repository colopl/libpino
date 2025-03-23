/*
 * libpino tests - handler_spl1.h
 * 
 * Copyright (c) 2025 Go Kudo
 *
 * This library is licensed under the BSD 3-Clause License.
 * For license details, please refer to the LICENSE file.
 *
 * SPDX-FileCopyrightText: Go Kudo <zeriyoshi@gmail.com>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PINO_TESTS_HANDLER_SPL1_H
#define PINO_TESTS_HANDLER_SPL1_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <pino.h>
#include <pino/handler.h>

typedef uint32_t spl1_size_t;

PH_BEGIN(spl1);

PH_DEF_STATIC_FIELDS_STRUCT(spl1) {
    spl1_size_t size;
};

PH_DEF_STRUCT(spl1) {
    PH_DEF_STRUCT_COMMON_FIELDS(spl1);
    uint8_t *data;
};

PH_DEFUN_SERIALIZE_SIZE(spl1) {
    return PH_SIZE_STATIC(spl1) + (size_t)PH_THIS_STATIC(spl1).size;
}

PH_DEFUN_SERIALIZE(spl1) {
    PH_SERIALIZE_STATIC(spl1);
    PH_SERIALIZE_DATA(spl1, PH_THIS(spl1)->data, (size_t)PH_THIS_STATIC(spl1).size);

    return true;
}

PH_DEFUN_UNSERIALIZE(spl1) {
    PH_UNSERIALIZE_STATIC(spl1);
    PH_UNSERIALIZE_DATA(spl1, data, (size_t)PH_THIS_STATIC(spl1).size);

    return PH_THIS(spl1);
}

PH_DEFUN_PACK(spl1) {
    PH_THIS_STATIC(spl1).size = PH_ARG_SIZE;
    PH_MEMCPY(PH_THIS(spl1)->data, PH_ARG_SRC, (size_t)PH_THIS_STATIC(spl1).size);

    return PH_THIS(spl1);
}

PH_DEFUN_UNPACK_SIZE(spl1) {
    return (size_t)PH_THIS_STATIC(spl1).size;
}

PH_DEFUN_UNPACK(spl1) {
    PH_MEMCPY(PH_ARG_DST, PH_THIS(spl1)->data, (size_t)PH_THIS_STATIC(spl1).size);

    return true;
}

PH_DEFUN_CREATE(spl1) {
    PH_CREATE_THIS(spl1);

    /* for memory manager test */
    PH_THIS(spl1)->data = (uint8_t *)PH_MALLOC(spl1, PH_ARG_SIZE);
    PH_FREE(spl1, PH_THIS(spl1)->data);

    PH_THIS(spl1)->data = (uint8_t *)PH_CALLOC(spl1, 1, PH_ARG_SIZE);
    if (!PH_THIS(spl1)->data) {
        PH_DESTROY_THIS(spl1);
        return NULL;
    }


    PH_THIS_STATIC(spl1).size = PH_ARG_SIZE;

    return PH_THIS(spl1);
}

PH_DEFUN_DESTROY(spl1) {
    PH_FREE(spl1, PH_THIS(spl1)->data);
    /* Accidentally memleak, but it's destroyed by memory manager.
    PH_FREE(spl1, PH_THIS(spl1)); 
    */
}

PH_END(spl1);

#endif  /* PINO_TESTS_HANDLER_SPL1_H */
