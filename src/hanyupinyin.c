/**
 * hanyupinyin.c
 *
 * Copyright (c) 2005, 2006
 *      libchewing Core Team. See ChangeLog for details.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file.
 */

/* @(#)hanyupinyin.c
 */

#include <stdio.h>
#include <string.h>
#include "hanyupinyin.h"
#include "hash.h"
#include "private.h"

/*
 according to http://humanum.arts.cuhk.edu.hk/Lexis/Lindict/syllabary/
       http://cls.admin.yzu.edu.tw/pronounce.htm
 */
PinYinZuinMap *hanyuCommonInitialsMap, *hanyuCommonFinalsMap, *hanyuSpecialsMap;
int HANYU_COMMONINITIALS, HANYU_COMMONFINALS, HANYU_SPECIALS, INIT_FLAG = 0;

static PinYinZuinMap* CommonInitialsMap()
{
	static PinYinZuinMap map[] = {
		{"zh","5"}, {"ch","t"}, {"sh","g"},
		{"b","1"}, {"p","q"}, {"m","a"}, {"f","z"},
		{"d","2"}, {"t","w"}, {"n","s"}, {"l","x"},
		{"g","e"}, {"k","d"}, {"h","c"},
		{"j","r"}, {"q","f"}, {"x","v"},
		{"r","b"}, {"z","y"}, {"c","h"}, {"s","n"}
	};
	HANYU_COMMONINITIALS = (sizeof(map)/sizeof(map[0]));
	return map;
}

static PinYinZuinMap* CommonFinalsMap()
{
	static PinYinZuinMap map[] = {
		{"uang","j;"},
		{"iang","u;"}, {"iong","m/"},
		{"iao","ul"}, {"ian","u0"}, {"ing","u/"},
		{"ang",";"},
		{"eng","/"},
		{"ong","j/"},
		{"uai","j9"}, {"uan","j0"},
		{"ia","u8"}, {"ie","u,"},
		{"iu","u."}, {"in","up"},
		{"ua","j8"}, {"uo","ji"},
		{"ui","jo"}, {"un","jp"},
		{"ue","m,"},
		{"er","-"},
		{"ai","9"},
		{"ei","o"},
		{"ao","l"},
		{"ou","."},
		{"an","0"},
		{"en","p"},
		{"a","8"},
		{"o","i"},
		{"e","k"},
		{"i","u"},
		{"u","j"},
      };
      HANYU_COMMONFINALS = (sizeof(map)/sizeof(map[0]));
      return map;
}

static PinYinZuinMap* SpecialsMap()
{
	static PinYinZuinMap map[] = {
		{"wang","j;"}, {"weng","j/"},
		{"yang","u;"}, {"ying","u/"}, {"yong","m/"},
		{"yuan","m0"},
		{"ang",";"},
		{"zhi","5"}, {"chi","t"}, {"shi","g"},
		{"yao","ul"}, {"you","u."},
		{"yan","u0"}, {"yin","up"},
		{"wai","j9"}, {"wei","jo"},
		{"wan","j0"}, {"wen","jp"},
		{"yue","m,"}, {"yun","mp"},
		{"er","-"},
		{"ai","9"},
		{"ei","o"},
		{"ao","l"},
		{"ou","."},
		{"an","0"},
		{"en","p"},
		{"yi","u"}, {"ya","u8"}, {"ye","u,"},
		{"wu","j"}, {"wa","j8"}, {"wo","ji"},
		{"yu","m"},
		//special for hamyupinyin-£z£º £{£º
		{"nv","sm"},{"lv","xm"},
		//special for hamyupinyin-£®
		{"ea",","},
		{"ri","b"}, {"zi","y"}, {"ci","h"}, {"si","n"},
		{"a","8"}, {"o","i"}, {"e","k"}
	};
	HANYU_SPECIALS = (sizeof(map)/sizeof(map[0]));
	return map;
}

static void FreeMap()
{
}

static void InitMap()
{
	hanyuSpecialsMap = SpecialsMap();
	hanyuCommonInitialsMap = CommonInitialsMap();
	hanyuCommonFinalsMap = CommonFinalsMap();
}

static int pinyin_lookup(PinYinZuinMap *table,
			  int table_size,
			  char *target)
{
	char	*p;
	int	i;
	for ( i = 0; i < table_size; i++ ) {
		p = strstr( target, table[ i ].pinyin );
		if ( p == target ) {
			return	i;
		}
	}
	return	table_size;
}

/*
 0: Success
 Non-Zero: Fail to fully convert
 1: Failed fo lookup initals
 2: Failed fo lookup finals
 Map pinyin key-sequence to Zuin key-sequence.
 Caller should allocate char zuin[4].
 */
int HanyuPinYinToZuin( char *pinyinKeySeq, char *zuinKeySeq )
{
	/*
	* pinyinKeySeq[] should have at most 6 letters (Shuang)
	* zuinKeySeq[] has at most 3 letters.
	*/
	char *p = 0, *cursor = 0, *initial = "", *final = "";
	int i, j;

	if ( ! INIT_FLAG ) {
		InitMap();
		INIT_FLAG = 1;
	}

	i = pinyin_lookup(hanyuSpecialsMap,
			  HANYU_SPECIALS,
			  pinyinKeySeq);
	if ( i != HANYU_SPECIALS ) {
		initial = hanyuSpecialsMap[ i ].zuin;
		final="\0";
	}
	else {
		i = pinyin_lookup(hanyuCommonInitialsMap,
			  HANYU_COMMONINITIALS, pinyinKeySeq);
		if ( i == HANYU_COMMONINITIALS ) {
			return	1;
		}

		initial = hanyuCommonInitialsMap[ i ].zuin;
		cursor = pinyinKeySeq +
			strlen( hanyuCommonInitialsMap[ i ].pinyin );
		
		j = pinyin_lookup(hanyuCommonFinalsMap,
			  HANYU_COMMONFINALS, cursor);
		if ( j != HANYU_COMMONFINALS ) {
			final = hanyuCommonFinalsMap[ j ].zuin;
		}
	}
	
	if (	!strcmp( final, "j0" ) ||
		!strcmp( final, "jp" ) ||
		!strcmp( final, "j" ) ) {
		if (	!strcmp( initial, "r" ) ||
			!strcmp( initial, "f" ) ||
			!strcmp( initial, "v" ) ) {
			final[0] = 'm';
		}
	}
	
	sprintf( zuinKeySeq, "%s%s\0", initial, final );
	return 0;
}
