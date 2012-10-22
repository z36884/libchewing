/*
 * chewing-utf8-util.h
 *
 * Copyright (c) 2005, 2006, 2008, 2012
 *	libchewing Core Team. See ChangeLog for details.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file.
 */

#ifndef CHEWING_UTF8_UTILS_H
#define CHEWING_UTF8_UTILS_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef HAVE_INTTYPES_H
#  include <inttypes.h>
#elif defined HAVE_STDINT_H
#  include <stdint.h>
#endif

#include <stddef.h>

/* Return length of UTF-8 string */
int ueStrLen( const char *str );

/* Return bytes of a UTF-8 character */
int ueBytesFromChar( unsigned char b );

/* Return byets of a UTF-8 string until len position */
int ueStrNBytes( const char *, int );

#define STRNCPY_CLOSE 1
#define STRNCPY_NOT_CLOSE 0

/*!
 * Return how many bytes was copied
 * @param[out] dest 	The memory area to copy to.
 * @param[in] src 	The memory area to copy from.
 * @param[in] n 	The number to copy.
 * @param[in] end
 */
int ueStrNCpy( char dest[], const char *src, size_t n, int end );

/* Return address from n length after src */
char *ueStrSeek( char *src, size_t n );

/*!
 * Locate a UTF-8 substring from UTF-8 string
 * @param[in] str     UTF-8 string from which substr is located
 * @param[in] lstr    Length of str in bytes.
 * @param[in] substr  UTF-8 string that is located in str.
 * @param[in] lsub    Length of substr in bytes.
 */
const char* ueStrStr( const char *str, size_t lstr,
                      const char *substr, size_t lsub );

/**
 * Convert a UTF-8 encoded character to a UTF-32 encoded character
 *
 * @param str UTF-8 encoded character
 * @return a UTF-32 encoded character in uint32_t type
 */
uint32_t u8tou32( const char *chr );

/**
 * Convert a UTF-32 encoded character to a UTF-8 encoded character
 *
 * @param chr a UTF-32 encoded character
 * @param out a pointer to a at least 4 bytes long array
 * @return 0 on success, 1 on failure
 */
int u32tou8( const uint32_t chr, char *out );

/**
 * Copy a UTF-32 encoded string to a UTF-8 encoded string
 *
 * @param dest UTF-32 encoded output string
 * @param src UTF-8 encoded src string
 * @param n How many UTF-32 character to copy
 */
void u8tou32cpy( uint32_t *dest, char * const src, size_t n );

/**
 * Copy a UTF-8 encoded string to a UTF-32 encoded string
 *
 * @param dest UTF-8 encoded output string
 * @param src UTF-32 encoded src string
 * @param start offset of src
 * @param n How many UTF-32 character to copy
 */
void u32tou8cpy( char *dest, const uint32_t src[], size_t start, size_t n );

#endif /* CHEWING_UTF8_UTILS_H */
