/*
 * libpino header - pino/handler.h
 * 
 * Copyright (c) 2025 Go Kudo
 *
 * This library is licensed under the BSD 3-Clause License.
 * For license details, please refer to the LICENSE file.
 *
 * SPDX-FileCopyrightText: Go Kudo <zeriyoshi@gmail.com>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PINO_HANDLER_H
#define PINO_HANDLER_H

#include <string.h>

#include <pino.h>
#include <pino/endianness.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef size_t (*pino_handler_serialize_size_t)(const void *);
typedef bool (*pino_handler_serialize_t)(const void *, void *);
typedef bool (*pino_handler_unserialize_t)(void *, const void *);
typedef bool (*pino_handler_pack_t)(void *, const void *, size_t);
typedef size_t (*pino_handler_unpack_size_t)(const void *);
typedef bool (*pino_handler_unpack_t)(const void *, void *);
typedef void *(*pino_handler_create_t)(size_t);
typedef void (*pino_handler_destroy_t)(void *);

struct _pino_handler_t {
    pino_handler_serialize_size_t serialize_size;
    pino_handler_serialize_t serialize;
    pino_handler_unserialize_t unserialize;
    pino_handler_pack_t pack;
    pino_handler_unpack_size_t unpack_size;
    pino_handler_unpack_t unpack;
    pino_handler_create_t create;
    pino_handler_destroy_t destroy;
    void *entry;
};

bool pino_handler_register(pino_magic_safe_t magic, pino_handler_t *handler);
bool pino_handler_unregister(pino_magic_safe_t magic);

void *pino_memory_manager_malloc(void *entry, size_t size);
void *pino_memory_manager_calloc(void *entry, size_t count, size_t size);
void pino_memory_manager_free(void *entry, void *ptr);

#define PH_NAME_HANDLER(name)                   g_ph_handler_##name##_obj
#define PH_NAME_REG(name)                       _ph_handler_##name##_register
#define PH_NAME_UNREG(name)                     _ph_handler_##name##_unregister
#define PH_NAME_STRUCT(name)                    _ph_handler_##name##_struct
#define PH_NAME_STATIC_FIELDS_STRUCT(name)      _ph_handler_##name##_static_fields_struct
#define PH_NAME_FUNC_SERIALIZE_SIZE(name)       _ph_handler_##name##_serialize_size
#define PH_NAME_FUNC_SERIALIZE(name)            _ph_handler_##name##_serialize
#define PH_NAME_FUNC_UNSERIALIZE(name)          _ph_handler_##name##_unserialize
#define PH_NAME_FUNC_PACK(name)                 _ph_handler_##name##_pack
#define PH_NAME_FUNC_UNPACK_SIZE(name)          _ph_handler_##name##_unpack_size
#define PH_NAME_FUNC_UNPACK(name)               _ph_handler_##name##_unpack
#define PH_NAME_FUNC_CREATE(name)               _ph_handler_##name##_create
#define PH_NAME_FUNC_DESTROY(name)              _ph_handler_##name##_destroy

#define PH_ARG_THIS                             __this
#define PH_ARG_DATA                             __data
#define PH_ARG_SIZE                             __size
#define PH_ARG_SRC                              __src
#define PH_ARG_DST                              __dest
#define PH_ARG_STATIC_FIELDS                    __static_fields
#define PH_ARG_MAGIC                            __magic

#define PH_DEF_STRUCT(name)                     struct PH_NAME_STRUCT(name)
#define PH_DEF_STATIC_FIELDS_STRUCT(name)       struct PH_NAME_STATIC_FIELDS_STRUCT(name)
#define PH_DEF_STRUCT_COMMON_FIELDS(name)       PH_DEF_STATIC_FIELDS_STRUCT(name) PH_ARG_STATIC_FIELDS

#define PH_DEFUN_SERIALIZE_SIZE(name)           static size_t PH_NAME_FUNC_SERIALIZE_SIZE(name)(const void *PH_ARG_THIS)
#define PH_DEFUN_SERIALIZE(name)                static bool PH_NAME_FUNC_SERIALIZE(name)(const void *PH_ARG_THIS, void *PH_ARG_DST)
#define PH_DEFUN_UNSERIALIZE(name)              static bool PH_NAME_FUNC_UNSERIALIZE(name)(void *PH_ARG_THIS, const void *PH_ARG_SRC)
#define PH_DEFUN_PACK(name)                     static bool PH_NAME_FUNC_PACK(name)(void *PH_ARG_THIS, const void *PH_ARG_SRC, size_t PH_ARG_SIZE)
#define PH_DEFUN_UNPACK_SIZE(name)              static size_t PH_NAME_FUNC_UNPACK_SIZE(name)(const void *PH_ARG_THIS)
#define PH_DEFUN_UNPACK(name)                   static bool PH_NAME_FUNC_UNPACK(name)(const void *PH_ARG_THIS, void *PH_ARG_DST)
#define PH_DEFUN_CREATE(name)                   static void *PH_NAME_FUNC_CREATE(name)(size_t PH_ARG_SIZE)
#define PH_DEFUN_DESTROY(name)                  static void PH_NAME_FUNC_DESTROY(name)(void *PH_ARG_THIS)

#define PH_THIS_P(name, ptr)                    ((struct PH_NAME_STRUCT(name) *)ptr)
#define PH_THIS_STATIC_P(name, ptr)             ((PH_THIS_P(name, ptr))->PH_ARG_STATIC_FIELDS)
#define PH_THIS(name)                           (PH_THIS_P(name, PH_ARG_THIS))
#define PH_THIS_STATIC(name)                    (PH_THIS_STATIC_P(name, PH_ARG_THIS))
#define PH_PINO_P(name, pino)                   (PH_THIS_P(name, ((pino_t *)pino)->this))
#define PH_PINO_STATIC_P(name, pino)            (PH_PINO_P(name, pino)->PH_ARG_STATIC_FIELDS)

#define PH_SIZE_THIS(name)                      (sizeof(struct PH_NAME_STRUCT(name)))
#define PH_SIZE_STATIC(name)                    (sizeof(struct PH_NAME_STATIC_FIELDS_STRUCT(name)))

#define PH_MALLOC(name, size)                   pino_memory_manager_malloc(PH_NAME_HANDLER(name).entry, size)
#define PH_CALLOC(name, count, size)            pino_memory_manager_calloc(PH_NAME_HANDLER(name).entry, count, size)
#define PH_FREE(name, ptr)                      pino_memory_manager_free(PH_NAME_HANDLER(name).entry, ptr)

#define PH_MEMCPY(dst, src, size)               memcpy(dst, src, size)
#define PH_MEMCPY_N2L(dst, src, size)           pino_endianness_memcpy_native2le(dst, src, size)
#define PH_MEMCPY_N2B(dst, src, size)           pino_endianness_memcpy_native2be(dst, src, size)
#define PH_MEMCPY_L2N(dst, src, size)           pino_endianness_memcpy_le2native(dst, src, size)
#define PH_MEMCPY_B2N(dst, src, size)           pino_endianness_memcpy_be2native(dst, src, size)

#define PH_CREATE_THIS(name) \
    void *PH_ARG_THIS = PH_MALLOC(name, PH_SIZE_THIS(name)); \
    if (!PH_THIS(name)) { \
        return NULL; \
    }

#define PH_DESTROY_THIS(name) do { \
    PH_FREE(name, PH_ARG_THIS); \
} while (0);

#define PH_SERIALIZE_STATIC(name)               pino_endianness_memcpy_native2le(((char *)PH_ARG_DST), &PH_THIS_STATIC(name), PH_SIZE_STATIC(name))
#define PH_SERIALIZE_DATA(name, src, size)      pino_endianness_memcpy_native2le(((char *)PH_ARG_DST) + PH_SIZE_STATIC(name), src, size)

#define PH_UNSERIALIZE_STATIC(name)             pino_endianness_memcpy_le2native(&PH_THIS_STATIC(name), PH_ARG_SRC, PH_SIZE_STATIC(name))
#define PH_UNSERIALIZE_DATA(name, dest, size)   do { \
    pino_endianness_memcpy_le2native(PH_THIS(name)->dest, ((char *)PH_ARG_SRC) + PH_SIZE_STATIC(name), size); \
} while (0);

#define PH_BEGIN(name) \
    static pino_handler_t PH_NAME_HANDLER(name); \
    PH_DEFUN_SERIALIZE_SIZE(name); \
    PH_DEFUN_SERIALIZE(name); \
    PH_DEFUN_UNSERIALIZE(name); \
    PH_DEFUN_PACK(name); \
    PH_DEFUN_UNPACK_SIZE(name); \
    PH_DEFUN_UNPACK(name); \
    PH_DEFUN_CREATE(name); \
    PH_DEFUN_DESTROY(name);

#define PH_END(name) \
    static pino_handler_t PH_NAME_HANDLER(name) = { \
        .serialize_size = PH_NAME_FUNC_SERIALIZE_SIZE(name), \
        .serialize = PH_NAME_FUNC_SERIALIZE(name), \
        .unserialize = PH_NAME_FUNC_UNSERIALIZE(name), \
        .pack = PH_NAME_FUNC_PACK(name), \
        .unpack_size = PH_NAME_FUNC_UNPACK_SIZE(name), \
        .unpack = PH_NAME_FUNC_UNPACK(name), \
        .create = PH_NAME_FUNC_CREATE(name), \
        .destroy = PH_NAME_FUNC_DESTROY(name), \
        .entry = NULL \
    }; \
    static inline bool PH_NAME_REG(name)(void) { \
        return pino_handler_register(#name, &PH_NAME_HANDLER(name)); \
    } \
    static inline bool PH_NAME_UNREG(name)(void) { \
        return pino_handler_unregister(#name); \
    }

#define PH_REG(name)     PH_NAME_REG(name)()
#define PH_UNREG(name)   PH_NAME_UNREG(name)()

#ifdef __cplusplus
}
#endif

#endif  /* PINO_HANDLER_H */
