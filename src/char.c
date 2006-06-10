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
#else
	#include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#ifdef	USE_BINARY_DAT
	static uint16* arrPhone = NULL;
	static int *begin = NULL;
	static char *phone_data_buf = NULL;
	int phone_num;
#else
	static uint16 arrPhone[ PHONE_NUM + 1 ];
	static int begin[ PHONE_NUM + 1 ];
	const int phone_num = PHONE_NUM;
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

static int CompUint16( const uint16 *pa, const uint16 *pb )
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
#ifndef	WIN32
	const char* DIRPATH_SEP_FILENAME = "%s/%s";
#else
	const char* DIRPATH_SEP_FILENAME = "%s\\%s";
#endif

	char filename[ 100 ];

#ifndef	USE_BINARY_DAT
	int i;
	FILE* indexfile;
#else	/* Use binary data format */
	long file_size;

	#ifdef WIN32
		HANDLE hIndexFile;  	 	 
        DWORD read_len;
	#else
		int indexfile;
		struct stat file_stat;
	#endif
#endif	/* USE_BINARY_DAT */

	sprintf( filename, DIRPATH_SEP_FILENAME, prefix, CHAR_FILE );
	dictfile = fopen( filename, "r" );

	sprintf( filename, DIRPATH_SEP_FILENAME, prefix, CHAR_INDEX_FILE );

#ifdef	USE_BINARY_DAT
	#ifdef WIN32
		hIndexFile = CreateFile( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
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
		indexfile = open( filename, O_RDONLY );
		if ( ! dictfile || -1 == indexfile )
			return 0;
		fstat( indexfile, &file_stat );
		file_size = (long)file_stat.st_size;

		phone_num = file_size / (sizeof(int) + sizeof(uint16));
		phone_data_buf = malloc( file_size );

		if( !phone_data_buf ||
			!read( indexfile, phone_data_buf, file_size ) )
		{
			close(indexfile);
			return 0;
		}

		begin = ((int*)phone_data_buf);
		arrPhone = (uint16*)(begin + phone_num);

		close(indexfile);
	#endif /* Win32 */
#else	/*	Use plain text data file	*/
	indexfile = fopen( filename, "r" );
	if ( ! dictfile || ! indexfile )
		return 0;
	for ( i = 0; i <= phone_num; i++ )
		fscanf( indexfile, "%hu %d", &arrPhone[ i ], &begin[ i ] );
	fclose( indexfile );
#endif

	addTerminateService( TerminateChar );
	return 1;
}

static void Str2Word( Word *wrd_ptr )
{
	char buf[ 1000 ];
	uint16 sh;

	fgettab( buf, 1000, dictfile );
	/* only read 6 bytes to wrd_ptr->word avoid buffer overflow */
	sscanf( buf, "%hu %6s", &sh, wrd_ptr->word );
	assert(wrd_ptr->word[0] != '\0');
}

int GetCharFirst( Word *wrd_ptr, uint16 phoneid )
{
	uint16 *pinx;

	pinx = (uint16 *) bsearch( 
		&phoneid, arrPhone, phone_num, 
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

