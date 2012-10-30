/**
 * test-utf8.c
 *
 * Copyright (c) 2005
 *	libchewing Core Team. See ChangeLog for details.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file.
 */

#include <stdio.h>
#include <string.h>

#include "test_harness.h"
#include "chewing-utf8-util.h"

int main (int argc, char *argv[])
{
	char *u8string;
	int u8len;

	u8string = "HelloWorld";
	u8len = ueStrLen(u8string);
	int len = strlen(u8string);
	ok (u8len == len, "ueStrLen");

	u8string = "測試計算長度";
	u8len = ueStrLen(u8string);
	ok (u8len == 6, "ueStrLen");

	u8string = "測試計算長度";
	char u8string2[16];
	ueStrNCpy(u8string2, u8string, 3, STRNCPY_CLOSE);
	ok (!strcmp(u8string2, "測試計"), "ueStrNCpy");

	u8string = "測試計算長度";
	u8string = ueStrSeek(u8string, 3);
	ok (!strcmp(u8string, "算長度"), "ueStrSeek");

	u8string = "測試計算長度";
	u8string = ueStrSeek(u8string, 0);
	ok (!strcmp(u8string, "測試計算長度"), "ueStrSeek");

	u8string = "a";
	uint32_t u32 = u8tou32( u8string );
	ok (u32 == 0x61, "u8tou32 on unibyte string");

	u8string = "測";
	u32 = u8tou32( u8string );
	ok (u32 == 0x6E2C, "u8tou32 on multibyte string");

	char invalid_u8string[] = { 0xFC, 0x80, 0x80, 0x80, 0x80, 0x80 };
	u32 = u8tou32( invalid_u8string );
	ok (u32 == 0xFFFD, "u8tou32 on invalid multibyte string");

	u32 = 0x61;
	char out[6];
	int ret = u32tou8( u32, out );
	ok (!ret, "u32tou8 on unibyte string");
	ok (!strcmp(out, "a"), "u32tou8 on unibyte string");

	u32 = 0x100;
	ret = u32tou8( u32, out );
	ok (!ret, "u32tou8 on multibyte string");
	ok (!strcmp(out, "Ā"), "u32tou8 on two byte utf8 string");

	u32 = 0x6E2C;
	ret = u32tou8( u32, out );
	ok (!ret, "u32tou8 on multibyte string");
	ok (!strcmp(out, "測"), "u32tou8 on three byte utf8 string");

	u32 = 0x10000;
	ret = u32tou8( u32, out );
	ok (!ret, "u32tou8 on multibyte string");
	ok (!strcmp(out, "𐀀"), "u32tou8 on four byte utf8 string");

	u32 = 0x200000;
	ret = u32tou8( u32, out );
	ok (ret < 0, "u32tou8 on invalid unicode");

	u8string = "測試";
	uint32_t u32out[2];
	u8tou32cpy( u32out, u8string, 2 );
	ok (u32out[0] == 0x6E2C, "u8tou32cpy copied first character");
	ok (u32out[1] == 0x8A66, "u8tou32cpy copied second character");

	uint32_t u32string[] = { 0x6E2C, 0x8A66 };
	char u8out[ 6 * 2 + 1 ];
	u32tou8cpy( u8out, u32string, 0, 2 );
	ok (!strcmp( u8out, "測試" ), "u32tou8cpy copied a UTF-32 string");

	uint32_t zu32string[] = { 0x6E2C, 0x8A66, 0 };
	ok (u32len( zu32string ) == 2, "u32len could calculate the string length");

	return exit_status();
}
