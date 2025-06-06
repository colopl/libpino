/*
 * libpino header - pino/endianness.h
 * 
 * Copyright (c) 2025 Go Kudo
 *
 * This library is licensed under the BSD 3-Clause License.
 * For license details, please refer to the LICENSE file.
 *
 * SPDX-FileCopyrightText: Go Kudo <zeriyoshi@gmail.com>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PINO_ENDIANNESS_H
#define PINO_ENDIANNESS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *pino_endianness_memcpy_le2native(void *dest, const void *src, size_t size);
void *pino_endianness_memcpy_be2native(void *dest, const void *src, size_t size);
void *pino_endianness_memcpy_native2le(void *dest, const void *src, size_t size);
void *pino_endianness_memcpy_native2be(void *dest, const void *src, size_t size);

void *pino_endianness_memmove_le2native(void *dest, const void *src, size_t size);
void *pino_endianness_memmove_be2native(void *dest, const void *src, size_t size);
void *pino_endianness_memmove_native2le(void *dest, const void *src, size_t size);
void *pino_endianness_memmove_native2be(void *dest, const void *src, size_t size);

int pino_endianness_memcmp_le2native(const void *s1, const void *s2, size_t size);
int pino_endianness_memcmp_be2native(const void *s1, const void *s2, size_t size);
int pino_endianness_memcmp_native2le(const void *s1, const void *s2, size_t size);
int pino_endianness_memcmp_native2be(const void *s1, const void *s2, size_t size);

#ifdef __cplusplus
}
#endif

#endif  /* PINO_ENDIANNESS_H */
