/**
 * hanyupinying.c
 *
 * Copyright (c) 2005
 *      libchewing Core Team. See ChangeLog for details.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file.
 */

/* @(#)hanyupinying.c
 */

#include <stdio.h>
#include <string.h>
#include "hanyupinying.h"
#include "hash.h"
#include "private.h"

/*
 according to http://humanum.arts.cuhk.edu.hk/Lexis/Lindict/syllabary/
       http://cls.admin.yzu.edu.tw/pronounce.htm
 */
PinYingZuinMap *hanyuCommonInitialsMap, *hanyuCommonFinalsMap, *hanyuSpecialsMap;
int HANYU_COMMONINITIALS, HANYU_COMMONFINALS, HANYU_SPECIALS, INIT_FLAG = 0;

static PinYingZuinMap* CommonInitialsMap()
{
      static PinYingZuinMap map[] = {
                          {"zh", "5"}, {"ch", "t"}, {"sh", "g"},
              {"b" , "1"}, {"p" , "q"}, {"m" , "a"}, {"f" ,"z"},
              {"d" , "2"}, {"t" , "w"}, {"n" , "s"}, {"l" ,"x"},
              {"g" , "e"}, {"k" , "d"}, {"h" , "c"}, {"j" , "r"},
              {"q" , "f"}, {"x" , "v"},
              {"r" ,"b"}, {"z" , "y"}, {"c" , "h"}, {"s" , "n"}
      };
      HANYU_COMMONINITIALS = sizeof(map)/sizeof(map[0]);
      return map;
}

static PinYingZuinMap* CommonFinalsMap()
{
      static PinYingZuinMap map[] = {
              {"uang","j;"},
              {"weng","j/"},
              {"iong","m/"},
              {"iang","u;"},
              {"ing","u/"},
              {"iao","ul"},
              {"iu","u."},
              {"ian","u0"},
              {"ang",";"},
              {"eng","/"},
              {"uai","j9"},
              {"ui","jo"},
              {"uan","j0"},
              {"un","jp"},
              {"ong","j/"},
              {"van","m0"},
              {"er","-"},
              {"ai","9"},
              {"ei","o"},
              {"ao","l"},
              {"ou","."},
              {"an","0"},
              {"en","p"},
              {"ia","u8"},
              {"ie","u,"},
              {"in","up"},
              {"ua","j8"},
              {"uo","ji"},
              {"ve","m,"},
              {"vn","mp"},
              {"a","8"},
              {"e","k"},
              {"i","u"},
              {"o","i"},
              {"v","m"},
              {"u","j"},
              {"E",","}
      };
      HANYU_COMMONFINALS = sizeof(map)/sizeof(map[0]);
      return map;
}

static PinYingZuinMap* SpecialsMap()
{
      static PinYingZuinMap map[] = {
              {"wang","j;"},
              {"weng","j/"},
              {"ying","u/"},
              {"yong","m/"},
              {"yang","u;"},
              {"yuan","m0"},
                          {"zhi","5"},
                          {"chi","t"},
                          {"shi","g"},
              {"yao","ul"},
              {"yun","mp"},
              {"you","u."},
              {"yan","u0"},
              {"yin","up"},
              {"yue","m,"},
              {"wai","j9"},
              {"wei","jo"},
              {"wan","j0"},
              {"wen","jp"},
                          {"ri","b"},
                          {"zi","y"},
                          {"ci","h"},
                          {"si","n"},
              {"yi","u"},
              {"ya","u8"},
              {"ye","u,"},
              {"wu","j"},
              {"wa","j8"},
              {"wo","ji"},
              {"yu","m"}
      };
      HANYU_SPECIALS = sizeof(map)/sizeof(map[0]);
      return map;
}

static void FreeMap()
{
       free( hanyuSpecialsMap );
       free( hanyuCommonInitialsMap );
       free( hanyuCommonFinalsMap );
}

static void InitMap()
{
//      int i;
//      FILE *fd;
/*      INIT_FLAG = 1;
       if ( getenv( "HOME" ) )
       {
               char *filedir = strcat( getenv( "HOME" ), CHEWING_HASH_PATH );
               char *filepath = strcat( filedir, "/pinyin.tab" );

               fd = fopen( filepath, "r" );
               if ( fd ) {
                       addTerminateService( FreeMap );
                       fscanf( fd, "%d", &HANYU_COMMONINITIALS );
                       ++HANYU_COMMONINITIALS;
                       hanyuCommonInitialsMap = ALC( PinYingZuinMap, HANYU_COMMONINITIALS );
                       for ( i = 0; i < HANYU_COMMONINITIALS - 1; i++ )
                               fscanf( fd, "%s %s",
                                       hanyuCommonInitialsMap[ i ].pinying,
                                       hanyuCommonInitialsMap[ i ].zuin );
                       fscanf( fd, "%d", &HANYU_COMMONFINALS );
                       ++HANYU_COMMONFINALS;
                       hanyuCommonFinalsMap = ALC( PinYingZuinMap, HANYU_COMMONFINALS );
                       for ( i = 0; i < HANYU_COMMONFINALS - 1; i++ )
                               fscanf( fd, "%s %s",
                                       hanyuCommonFinalsMap[ i ].pinying,
                                       hanyuCommonFinalsMap[ i ].zuin );
                       fclose( fd );
                       return;
               }
       }*/

       hanyuSpecialsMap = SpecialsMap();
       hanyuCommonInitialsMap = CommonInitialsMap();
       hanyuCommonFinalsMap = CommonFinalsMap();
}
/*
 0: Success
 Non-Zero: Fail to fully convert
 1: Failed fo lookup initals
 2: Failed fo lookup finals
 Map pinyin key-sequence to Zuin key-sequence.
 Caller should allocate char zuin[4].
 */

int HanyuPinYingToZuin( char *pinyingKeySeq, char *zuinKeySeq )
{
       /*
        * pinyinKeySeq[] should have at most 6 letters (Shuang)
        * zuinKeySeq[] has at most 3 letters.
        */
       char *p, *cursor;
       char *initial = 0;
       char *final = 0;
       int i,j;

       if ( ! INIT_FLAG )
               InitMap();

       for ( i = 0; i < HANYU_SPECIALS; i++ )
       {
               p = strstr( pinyingKeySeq, hanyuSpecialsMap[ i ].pinying );
               if ( p == pinyingKeySeq )
               {
                       initial = hanyuSpecialsMap[ i ].zuin;
                       final="\0";
                       break;
               }
       }

       if( i == HANYU_SPECIALS )
       {
               for ( j = 0; j < HANYU_COMMONINITIALS; j++ )
               {
                       p = strstr( pinyingKeySeq, hanyuCommonInitialsMap[ j ].pinying );
                       if ( p == pinyingKeySeq )
                       {
                               initial = hanyuCommonInitialsMap[ j ].zuin;
                               cursor = pinyingKeySeq +
                                       strlen( hanyuCommonInitialsMap[ j ].pinying );
                               break;
                       }
               }
               if ( j == HANYU_COMMONINITIALS )        return 1;


               if ( cursor )
               {
                       for ( i = 0; i < HANYU_COMMONFINALS; i++ )
                       {
                               p = strstr( cursor, hanyuCommonFinalsMap[ i ].pinying );
                               if ( p == cursor )
                               {
                                       final = hanyuCommonFinalsMap[ i ].zuin;
                                       break;
                               }
                       }
                       if ( i == HANYU_COMMONFINALS ) return 2;
               }
       }

       /*if ( ! strcmp( final, "j0" ) ) {
               if (
                       ! strcmp( initial, "f" ) ||
                       ! strcmp( initial, "r" ) ||
                       ! strcmp( initial, "v" ) ) {
                       final = "m0";
               }
       }*/
       sprintf( zuinKeySeq, "%s%s\0", initial, final );
       return 0;
}
