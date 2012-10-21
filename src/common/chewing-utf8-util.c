/**
 * chewing-utf8-util.c
 *
 * Copyright (c) 2005, 2006, 2012
 *	libchewing Core Team. See ChangeLog for details.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "chewing-utf8-util.h"

/* Table of UTF-8 length */
static char utf8len_tab[256] =
{
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /*bogus*/
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /*bogus*/
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1,
};

/* Return length of UTF-8 string */
int ueStrLen( const char *str )
{
	int length = 0;
	const char *strptr = str;

	while ( strptr[ 0 ] != '\0' ) {
		strptr += ueBytesFromChar( strptr[0] );
		++length;
	}
	return length;
}

/* Return bytes of a UTF-8 character */
int ueBytesFromChar( unsigned char b )
{
	return utf8len_tab[ b ];
}

/* Return byets of a UTF-8 string until n position */
int ueStrNBytes( const char *str, int n )
{
	int i = 0, len = 0;
	char *iter = (char *) str;
	for ( i = 0; i < n; i++ ) {
		len += ueBytesFromChar( iter[ len ] );
	}
	return len;
}

/* Return how many bytes was copied */
int ueStrNCpy( char dest[], const char *src, size_t n, int end )
{
	int len = 0;
	len = ueStrNBytes( src, n );
	memcpy( dest, src, len );
	if ( end == STRNCPY_CLOSE )
		dest[ len ] = '\0';
	return len;
}

char *ueStrSeek( char *src, size_t n )
{
	size_t i = 0;
	char *iter = src;
	for ( i = 0; i < n; i++ ) {
		iter += ueBytesFromChar( iter[0] );
	}
	return iter;
}

/* Locate a UTF-8 substring from UTF-8 string */
const char *ueStrStr( const char *str, size_t lstr,
                      const char *substr, size_t lsub )
{
	const char *p = str;
	size_t ub;
	if ( lstr < lsub )
		return NULL;
	ub = lstr - lsub;
	for ( ; p-str <= ub; p++ ) {
		if ( !strncmp( p, substr, lsub ) )
			return p;
	}
	return NULL;
}

static inline unsigned char mask( const char c, const int bits )
{
	unsigned char ret = c;
	unsigned char mask = 0xFF;
	mask <<= bits;
	mask >>= bits;
	return (ret & mask);
}

uint32_t u8tou32( const char *chr )
{
	assert( chr );
	int bytes = ueBytesFromChar( chr[0] );
	uint32_t ret = 0;

	switch (bytes) {
	case 1:
		ret = chr[0];
		break;
	case 2:
	case 3:
	case 4:
		ret = mask( chr[0], bytes + 1 );
		break;
	case 5:
	case 6:
	default:
		bytes = 0;
		ret = 0xfffd;
		break;
	}
	for (int i = 1; i < bytes; i++) {
		ret = (ret << 6) + mask( chr[i], 2 );
	}
	return ret;
}

int u32tou8( const uint32_t chr, char *out )
{
	if ( chr > 0x1FFFFFU )
		return -1;

	if ( chr <= 0x7FU ) {
		out[0] = chr;
		out[1] = '\0';
	} else if ( chr <= 0x7FFU ) {
		out[0] = 0xC0 + (chr >> 6);
		out[1] = 0x80 + (chr & 0x3F);
		out[2] = '\0';
	} else if ( chr <= 0xFFFFU ) {
		out[0] = 0xE0 + ((chr >> 12) & 0x3F);
		out[1] = 0x80 + ((chr >> 6)  & 0x3F);
		out[2] = 0x80 + (chr & 0x3F);
		out[3] = '\0';
	} else if ( chr <= 0x1FFFFFU ) {
		out[0] = 0xF0 + ((chr >> 18) & 0x3F);
		out[1] = 0x80 + ((chr >> 12) & 0x3F);
		out[2] = 0x80 + ((chr >> 6)  & 0x3F);
		out[3] = 0x80 + (chr & 0x3F);
		out[4] = '\0';
	}

	return 0;
}
