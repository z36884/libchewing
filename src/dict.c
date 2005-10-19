/**
 * dict.h
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

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <windows.h>

#include "dict.h"

#ifdef	USE_BINARY_DAT
	static int *begin = NULL;
#else
	static int begin[ PHONE_PHRASE_NUM + 1 ];
#endif
static FILE *dictfile;
static int end_pos;

char* fgettab( char *buf, int maxlen, FILE *fp )
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

static void TerminateDict()
{
	if ( dictfile )
		fclose( dictfile );

#ifdef	USE_BINARY_DAT
	if( begin )
		free( begin );
#endif
}

int InitDict( const char *prefix )
{
	FILE *indexfile;
	int i;
	char filename[ 100 ];

#ifndef WIN32
	sprintf( filename, "%s/%s", prefix, DICT_FILE );
#else
	HANDLE hInFile;
	DWORD read_len;
	DWORD file_size;
	sprintf( filename, "%s\\%s", prefix, DICT_FILE );
#endif
	dictfile = fopen( filename, "r" );

	#ifndef WIN32
		sprintf( filename, "%s/%s", prefix, PH_INDEX_FILE );
	#else
		sprintf( filename, "%s\\%s", prefix, PH_INDEX_FILE );
	#endif

#ifdef	USE_BINARY_DAT
	hInFile = CreateFile( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );

	if( hInFile == INVALID_HANDLE_VALUE  )
		return 0;

	file_size = GetFileSize( hInFile, NULL);
	if(	begin = (int*)malloc(file_size + sizeof(int)) )
		ReadFile( hInFile, begin, file_size, &read_len, NULL);

	CloseHandle(hInFile);
#else
	indexfile = fopen( filename, "r" );
	assert( dictfile && indexfile );
	i = 0;
	while ( !feof( indexfile ) )
		fscanf( indexfile, "%d", &begin[ i++ ] );
	fclose( indexfile );
#endif
	addTerminateService( TerminateDict );
	return 1;
}

void Str2Phrase( Phrase *phr_ptr )
{
	char buf[ 1000 ];

	fgettab( buf, 1000, dictfile );
	sscanf( buf, "%s %d", phr_ptr->phrase, &( phr_ptr->freq ) );
}

int GetPhraseFirst( Phrase *phr_ptr, int phone_phr_id )
{
	assert( ( 0 <= phone_phr_id ) && ( phone_phr_id < PHONE_PHRASE_NUM ) );

	fseek( dictfile, begin[ phone_phr_id ], SEEK_SET );
	end_pos = begin[ phone_phr_id + 1 ];
	Str2Phrase( phr_ptr );
	return 1;
}

int GetPhraseNext( Phrase *phr_ptr )
{
	if ( ftell( dictfile ) >= end_pos )
		return 0;
	Str2Phrase( phr_ptr );
	return 1;
}

