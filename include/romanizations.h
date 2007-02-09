/**
 * romanizations.h
 *
 * Copyright (c) 2007
 *      libchewing Core Team. See ChangeLog for details.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file.
 */

/* @(#)romanizations.h
 */

#ifndef _ROMANIZATIONS_H
#define _ROMANIZATIONS_H

/*
  This is a key-sequense map.
  From pinyin sequence to a default-layout sequence.
  Eg: Zhang -> {"zh","5"}, {"ang",";"}
 */
typedef struct {
	char pinyin[ 5 ];
	char zuin[ 4 ];
} PinYinZuinMap;

int HanyuPinYinToZuin( char *pinyinKeySeq, char *zuinKeySeq );

#endif /* _HANYUPINYING_H */

