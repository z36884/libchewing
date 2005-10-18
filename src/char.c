/**
 * char.c
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

/** 
 * @file char.c
 * @brief word data file
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "char.h"
#include "private.h"

#ifdef WIN32
	#include <windows.h>
#endif

#ifdef	USE_BINARY_DAT
	static uint16* arrPhone = NULL;
	static int *begin = NULL;
	static char *phone_data_buf = NULL;
#else
	static uint16 arrPhone[ PHONE_NUM + 1 ];
	static int begin[ PHONE_NUM + 1 ];
#endif
static FILE *dictfile;
static int end_pos;

static char *fgettab( char *buf, int maxlen, FILE *fp )
{
	int i;

	for ( i = 0; i < maxlen; i++ ) {
		buf[ i ] = (char) fgetc( fp );
		if ( feof( fp ) )
			break;
		if ( buf[ i ] == '\t' )
			break;
	}
	if ( feof( fp ) )
		return 0;
	buf[ i ] = '\0';
	return buf;
}

int CompUint16( const uint16 *pa, const uint16 *pb )
{
	return ( (*pa) - (*pb) );
}

static void TerminateChar()
{
	if ( dictfile )
		fclose( dictfile );

#ifdef USE_BINARY_DAT
	if( phone_data_buf )
		free(phone_data_buf);
#endif
}

int InitChar( const char *prefix )
{
	FILE *indexfile;
	char filename[ 100 ];
	int i;
	long file_size;
	int phone_num;

#ifndef WIN32
	sprintf( filename, "%s/%s", prefix, CHAR_FILE );
#else
	HANDLE hIndexFile;
	DWORD read_len;
	sprintf( filename, "%s\\%s", prefix, CHAR_FILE );
#endif

	dictfile = fopen( filename, "r" );

#ifndef WIN32
	sprintf( filename, "%s/%s", prefix, CHAR_INDEX_FILE );
#else
	sprintf( filename, "%s\\%s", prefix, CHAR_INDEX_FILE );
#endif

#ifdef	USE_BINARY_DAT
	hIndexFile = CreateFile( filename, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
	if ( ! dictfile || hIndexFile == INVALID_HANDLE_VALUE )
		return 0;
	file_size = GetFileSize( hIndexFile, NULL);
	phone_num = file_size / (sizeof(int) + sizeof(uint16));
	phone_data_buf = malloc( file_size );

	if( !phone_data_buf || 
		! ReadFile( hIndexFile, phone_data_buf, file_size, &read_len, NULL) )
	{
		CloseHandle(hIndexFile);
		return 0;
	}

	begin = ((int*)phone_data_buf);
	arrPhone = (uint16*)(begin + phone_num);

	CloseHandle(hIndexFile);
#else
	indexfile = fopen( filename, "r" );
	if ( ! dictfile || ! indexfile )
		return 0;
	for ( i = 0; i <= PHONE_NUM; i++ )
		fscanf( indexfile, "%hu %d", &arrPhone[ i ], &begin[ i ] );
	fclose( indexfile );
#endif

	addTerminateService( TerminateChar );
	return 1;
}

void Str2Word( Word *wrd_ptr )
{
	char buf[ 1000 ];
	uint16 sh;

	fgettab( buf, 1000, dictfile );
	sscanf( buf, "%hu %s", &sh, wrd_ptr->word );
}

int GetCharFirst( Word *wrd_ptr, uint16 phoneid )
{
	uint16 *pinx;

	pinx = (uint16 *) bsearch( 
		&phoneid, arrPhone, PHONE_NUM, 
		sizeof( uint16 ), (CompFuncType) CompUint16 );
	if ( ! pinx )
		return 0;

	fseek( dictfile, begin[ pinx - arrPhone ], SEEK_SET );
	end_pos = begin[ pinx - arrPhone + 1 ];
	Str2Word( wrd_ptr );
	return 1;
}

int GetCharNext( Word *wrd_ptr )
{
	if ( ftell( dictfile ) >= end_pos )
		return 0;
	Str2Word( wrd_ptr );
	return 1;
}

