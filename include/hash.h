/**
 * hash.h
 *
 * Copyright (c) 1999, 2000, 2001
 *	Lu-chuan Kung and Kang-pen Chen.
 *	All rights reserved.
 *
 * Copyright (c) 2004
 *	libchewing Core Team. See ChangeLog for details.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file.
 */

#ifndef HASH_H
#define HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "global.h"
#include "userphrase.h"

#define FIELD_SIZE (125)
#define N_HASH_BIT (14)
#define BIN_HASH_SIG "CBiH"
#ifdef	WIN32
  /* Change file name to uhash.dat to make the newer 
     UTF8-encoded file distinguishable from the older one */
#define HASH_FILE  "uhash.dat"
#else
#define HASH_FILE  "hash.dat"
#endif	/* WIN32 */

#define HASH_TABLE_SIZE (1<<N_HASH_BIT)

#ifdef __MacOSX__
#define CHEWING_HASH_PATH "/Library/ChewingOSX"
#else
#define CHEWING_HASH_PATH "/.chewing"
#endif

typedef struct tag_HASH_ITEM {
	int item_index;
	UserPhraseData data;
	struct tag_HASH_ITEM *next;
} HASH_ITEM;

HASH_ITEM *HashFindPhone( const uint16 phoneSeq[] );
HASH_ITEM *HashFindEntry( const uint16 phoneSeq[], const char wordSeq[] );
HASH_ITEM *HashInsert( UserPhraseData *pData );
HASH_ITEM *HashFindPhonePhrase( const uint16 phoneSeq[], HASH_ITEM *pHashLast );
void HashModify( HASH_ITEM *pItem );
int AlcUserPhraseSeq( UserPhraseData *pData, int len );
int ReadHash( char *path );
void FreeHashTable( void );

void HashItem2String(char *str, HASH_ITEM *pItem);

char* _load_hash_file(const char *filename, int *size);

int ReadHashItem_bin(const char *srcbuf, HASH_ITEM *pItem, int item_index );

void HashItem2Binary( char *str, HASH_ITEM *pItem );

void HashModify( HASH_ITEM *pItem);


#ifdef __cplusplus
}
#endif

#endif

