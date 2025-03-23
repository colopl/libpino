/*
 * libpino - pino.c
 * 
 * Copyright (c) 2025 Go Kudo
 *
 * This library is licensed under the BSD 3-Clause License.
 * For license details, please refer to the LICENSE file.
 *
 * SPDX-FileCopyrightText: Go Kudo <zeriyoshi@gmail.com>
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <pino.h>
#include <pino/handler.h>

#include <pino_internal.h>

static inline pino_t *pino_create(pino_magic_safe_t magic, pino_handler_t *handler, size_t size)
{
    pino_t *pino;

    pino = pmalloc(sizeof(pino_t));
    if (!pino) {
        return NULL; /* LCOV_EXCL_LINE */
    }

    pmemcpy(pino->magic, magic, sizeof(pino_magic_t));
    pino->magic[sizeof(pino_magic_t)] = '\0';
    pino->handler = handler;
    pino->this = handler->create(size);
    if (!pino->this) {
        PINO_SUPRTF("handler->create failed");
        pfree(pino);
        return NULL;
    }

    return pino;
}

extern bool pino_init(void)
{
    return pino_handler_init(HANDLER_STEP);
}

extern void pino_free(void)
{
    pino_handler_free();
}

extern size_t pino_serialize_size(const pino_t *pino)
{
    if (!pino) {
        return 0;
    }

    PINO_SUPRTF("magic: %.4s, serialize_size: %zu, magic_size: %zu", pino->magic, pino->handler->serialize_size(pino->this), sizeof(pino_magic_t));

    return pino->handler->serialize_size(pino->this) + sizeof(pino_magic_t);
}

extern bool pino_serialize(const pino_t *pino, void *dest)
{
    if (!pino || !dest) {
        return false;
    }

    pmemcpy_n2l(dest, pino->magic, sizeof(pino_magic_t));

    return pino->handler->serialize(pino->this, ((char *)dest) + sizeof(pino_magic_t));
}

extern pino_t *pino_unserialize(const void *src, size_t size)
{
    pino_t *pino;
    pino_handler_t *handler;
    pino_magic_safe_t magic;

    if (!src || size < sizeof(pino_magic_t)) {
        return NULL;
    }

    pmemcpy_l2n(magic, src, sizeof(pino_magic_t));
    magic[sizeof(pino_magic_t)] = '\0';

    handler = pino_handler_find(magic);
    if (!handler) {
        return NULL;
    }

    pino = pino_create(magic, handler, size - sizeof(pino_magic_t));
    if (!pino) {
        return NULL; /* LCOV_EXCL_LINE */
    }

    if (!handler->unserialize(pino->this, ((char *)src) + sizeof(pino_magic_t))) {
        pino_destroy(pino);
        return NULL;
    }

    return pino;
}

extern pino_t *pino_pack(pino_magic_safe_t magic, const void *src, size_t size)
{
    pino_t *pino;
    pino_handler_t *handler;

    handler = pino_handler_find(magic);
    if (!handler) {
        PINO_SUPRTF("handler not found : %s", magic);
        return NULL;
    }

    pino = pino_create(magic, handler, size);
    if (!pino) {
        PINO_SUPRTF("pino_create failed");
        return NULL;
    }

    if (!handler->pack(pino->this, src, size)) {
        pino_destroy(pino);
        PINO_SUPRTF("handler->pack failed");
        return NULL;
    }

    return pino;
}

extern size_t pino_unpack_size(const pino_t *pino)
{
    PINO_SUPRTF("magic: %.4s, unpack_size: %zu", pino->magic, pino->handler->unpack_size(pino->this));

    return pino->handler->unpack_size(pino->this);
}

extern bool pino_unpack(const pino_t *pino, void *dest)
{
    return pino->handler->unpack(pino->this, dest);
}

extern void pino_destroy(pino_t *pino)
{
    if (!pino) {
        return;
    }

    if (pino->this) {
        pino->handler->destroy(pino->this);
    }

    pfree(pino);
}
