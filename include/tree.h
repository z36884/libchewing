/**
 * char.h
 *
 * Copyright (c) 2005
 *	libchewing Core Team. See ChangeLog for details.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file.
 */

#ifndef TREE_H
#define TREE_H

#define PHONE_TREE_FILE		"fonetree.dat"

typedef struct {
	uint16 phone_id;
	int phrase_id;
	int child_begin, child_end;
} TreeType;

/* tree.c */
int Phrasing( PhrasingOutput *ppo, uint16 phoneSeq[], int nPhoneSeq, 
		char selectStr[][ MAX_PHONE_SEQ_LEN * 2 + 1 ], 
		IntervalType selectInterval[], int nSelect, 
		int bArrBrkpt[], int bUserArrCnnct[] );
int IsContain( IntervalType, IntervalType );
int IsIntersect( IntervalType, IntervalType );
void ReadTree( const char * );
int TreeFindPhrase( int begin, int end, const uint16 *phoneSeq );

#endif
