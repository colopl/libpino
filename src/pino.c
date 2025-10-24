/*
 * libpino - pino.c
 * 
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
    pino->static_fields_size = handler->static_fields_size;
    pino->static_fields = pcalloc(1, pino->static_fields_size);
    if (!pino->static_fields) {
        /* LCOV_EXCL_START */
        pfree(pino);
        return NULL;
        /* LCOV_EXCL_STOP */
    }
    pino->handler = handler;
    pino->this = handler->create(size, pino->static_fields);
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

    PINO_SUPRTF("magic: %.4s, serialize_size: %zu, magic_size: %zu", pino->magic, pino->handler->serialize_size(pino->this, pino->static_fields), sizeof(pino_magic_t));

    return pino->handler->serialize_size(pino->this, pino->static_fields) + sizeof(pino_magic_t) + sizeof(pino_static_fields_size_t) + pino->static_fields_size;
}

extern bool pino_serialize(const pino_t *pino, void *dest)
{
    if (!pino || !dest) {
        return false;
    }

    pmemcpy(dest, pino->magic, sizeof(pino_magic_t));
    pmemcpy_n2l(((char *)dest) + sizeof(pino_magic_t), &pino->static_fields_size, sizeof(pino_static_fields_size_t));

    /* fields always use LE */
    pmemcpy(((char *)dest) + sizeof(pino_magic_t) + sizeof(pino_static_fields_size_t), pino->static_fields, pino->static_fields_size);

    return pino->handler->serialize(pino->this, pino->static_fields, ((char *)dest) + sizeof(pino_magic_t) + sizeof(pino_static_fields_size_t) + pino->handler->static_fields_size);
}

extern pino_t *pino_unserialize(const void *src, size_t size)
{
    pino_t *pino;
    pino_handler_t *handler;
    pino_magic_safe_t magic;
    pino_static_fields_size_t fields_size;

    if (!src) {
        return NULL;
    }

    if (size < sizeof(pino_magic_t) + sizeof(pino_static_fields_size_t)) {
        return NULL;
    }

    pmemcpy(magic, src, sizeof(pino_magic_t));
    pmemcpy_l2n(&fields_size, ((char *)src) + sizeof(pino_magic_t), sizeof(pino_static_fields_size_t));

    if (size < sizeof(pino_magic_t) + sizeof(pino_static_fields_size_t) + fields_size) {
        return NULL;
    }

    handler = pino_handler_find(magic);
    if (!handler) {
        return NULL;
    }

    pino = pino_create(magic, handler, size - sizeof(pino_magic_t) - sizeof(pino_static_fields_size_t) - fields_size);
    if (!pino) {
        return NULL; /* LCOV_EXCL_LINE */
    }

    /* always LE */
    pmemcpy(pino->static_fields, ((char *)src) + sizeof(pino_magic_t) + sizeof(pino_static_fields_size_t), fields_size);
    if (!handler->unserialize(pino->this, pino->static_fields, ((char *)src) + sizeof(pino_magic_t) + sizeof(pino_static_fields_size_t) + fields_size, size)) {
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

    if (!handler->pack(pino->this, pino->static_fields, src, size)) {
        pino_destroy(pino);
        PINO_SUPRTF("handler->pack failed");
        return NULL;
    }

    return pino;
}

extern size_t pino_unpack_size(const pino_t *pino)
{
    PINO_SUPRTF("magic: %.4s, unpack_size: %zu", pino->magic, pino->handler->unpack_size(pino->this, pino->static_fields));

    return pino->handler->unpack_size(pino->this, pino->static_fields);
}

extern bool pino_unpack(const pino_t *pino, void *dest)
{
    return pino->handler->unpack(pino->this, pino->static_fields, dest);
}

extern void pino_destroy(pino_t *pino)
{
    if (!pino) {
        return;
    }

    if (pino->this) {
        pino->handler->destroy(pino->this, pino->static_fields);
    }

    if (pino->static_fields) {
        pfree(pino->static_fields);
    }

    pfree(pino);
}

extern uint32_t pino_version_id()
{
    return (uint32_t)PINO_VERSION_ID;
}

extern pino_buildtime_t pino_buildtime()
{
    return (pino_buildtime_t)PINO_BUILDTIME;
}
