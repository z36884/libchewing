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

#ifdef WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include "dict.h"

#ifdef	USE_BINARY_DAT
	static int *begin = NULL;
#else
	static int begin[ PHONE_PHRASE_NUM + 1 ];
#endif
static FILE *dictfile;
static int end_pos;

/* hialan:
 * function proto defined in chewingio.c 
 */
int addTerminateService( void (*callback)() );

static char* fgettab( char *buf, int maxlen, FILE *fp )
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
#ifndef	WIN32
	const char* DIRPATH_SEP_FILENAME = "%s/%s";
#else
	const char* DIRPATH_SEP_FILENAME = "%s\\%s";
#endif

#ifndef	USE_BINARY_DAT
	FILE* indexfile;
	int i;
#else	/* Use binary data format */
	#ifdef WIN32
		HANDLE hInFile;  	 	 
        DWORD read_len; 		 
        DWORD file_size;
	#else
		int indexfile;
		struct stat file_stat;
	#endif
#endif	/* USE_BINARY_DAT */

	char filename[ 100 ];

	sprintf( filename, DIRPATH_SEP_FILENAME, prefix, DICT_FILE );
	dictfile = fopen( filename, "r" );

	sprintf( filename, DIRPATH_SEP_FILENAME, prefix, PH_INDEX_FILE );

#ifdef	USE_BINARY_DAT
	#ifdef WIN32 /* Win32 */
		hInFile = CreateFile( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );

		if( hInFile == INVALID_HANDLE_VALUE )  	 	 
			return 0; 		 
  		 
		file_size = GetFileSize( hInFile, NULL); 		 
		if( begin = (int*)malloc(file_size + sizeof(int)) ) 		 
			ReadFile( hInFile, begin, file_size, &read_len, NULL); 		 
  		 
		CloseHandle(hInFile);
	#else
		indexfile = open( filename, O_RDONLY);

		if( indexfile == -1 )
			return 0;
		fstat( indexfile, &file_stat );

		if(	begin = (int*)malloc( (size_t)file_stat.st_size + sizeof(int)) )
			read( indexfile, begin, (size_t)file_stat.st_size );

		close( indexfile );
	#endif /* Win32 */
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

static void Str2Phrase( Phrase *phr_ptr )
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

