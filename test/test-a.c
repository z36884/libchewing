#ifdef HAVE_CONFIG_H
#    include <config.h>
#endif

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "chewing.h"
#include "plat_types.h"
#include "testhelper.h"

#include "sqlite3.h"
#include "chewing-private.h"
#include "curl/curl.h"

int partition(void *fp, int n, char **pts, char **apts) {
    int i, j, k, numofword;
    char str[50], word[36];

    for (i = 0; i < n; i++) {
	strcpy(str, pts[i]);
	numofword = ueStrLen(str);
        fprintf((FILE*)fp, "%s =>", str);

	for(j = numofword; j >= 1; j--) {

	    for(k = 1; k <= j; k++) {
		memset(word, 0, sizeof(word));
		strncat(word, str, 3*k);
		fprintf((FILE*)fp, " %s ", word);
	    }
	    
	    for(k = 3; k < strlen(str); k++)
		str[k - 3] = str[k];
	    str[3*(j-1)] = '\0';
	    
	}
	fprintf((FILE*)fp, "\n");
    }
    return 0;
}

int main(int argc, char *argv[])
{
    ChewingContext *ctx;
    int ret;
    FILE *ffp = fopen("a.out", "w");

    const char phrase[] = "\xE6\xB8\xAC\xE8\xA9\xA6" /* 測試 */ ;
    const char bopomofo[] = "\xE3\x84\x98\xE3\x84\x9C\xCB\x8B \xE3\x84\x95\xCB\x8B";    /* ㄘㄜˋ ㄕˋ */

    const char phrase_in_limit[] =
        "\xE9\x87\x91\xE7\xAA\xA9\xE9\x8A\x80\xE7\xAA\xA9\xE4\xB8\x8D\xE5\xA6\x82\xE8\x87\xAA\xE5\xB7\xB1\xE7\x9A\x84\xE7\x8B\x97\xE7\xAA\xA9";
    /* 金窩銀窩不如自己的狗窩 */
    const char bopomofo_in_limit[] =
        "\xE3\x84\x90\xE3\x84\xA7\xE3\x84\xA3\x20\xE3\x84\xA8\xE3\x84\x9B\x20\xE3\x84\xA7\xE3\x84\xA3\xCB\x8A\x20\xE3\x84\xA8\xE3\x84\x9B\x20\xE3\x84\x85\xE3\x84\xA8\xCB\x8B\x20\xE3\x84\x96\xE3\x84\xA8\xCB\x8A\x20\xE3\x84\x97\xCB\x8B\x20\xE3\x84\x90\xE3\x84\xA7\xCB\x87\x20\xE3\x84\x89\xE3\x84\x9C\xCB\x99\x20\xE3\x84\x8D\xE3\x84\xA1\xCB\x87\x20\xE3\x84\xA8\xE3\x84\x9B";
    /* ㄐㄧㄣ ㄨㄛ ㄧㄣˊ ㄨㄛ ㄅㄨˋ ㄖㄨˊ ㄗˋ ㄐㄧˇ ㄉㄜ˙ ㄍㄡˇ ㄨㄛ */
    const char phrase_out_of_limit[] =
        "\xE9\x87\x91\xE7\xAA\xA9\xE9\x8A\x80\xE7\xAA\xA9\xE4\xB8\x8D\xE5\xA6\x82\xE8\x87\xAA\xE5\xB7\xB1\xE7\x9A\x84\xE7\x8B\x97\xE7\xAA\xA9\xE5\x97\x8E";
    /* 金窩銀窩不如自己的狗窩嗎 */
    const char bopomofo_out_of_limit[] =
        "\xE3\x84\x90\xE3\x84\xA7\xE3\x84\xA3\x20\xE3\x84\xA8\xE3\x84\x9B\x20\xE3\x84\xA7\xE3\x84\xA3\xCB\x8A\x20\xE3\x84\xA8\xE3\x84\x9B\x20\xE3\x84\x85\xE3\x84\xA8\xCB\x8B\x20\xE3\x84\x96\xE3\x84\xA8\xCB\x8A\x20\xE3\x84\x97\xCB\x8B\x20\xE3\x84\x90\xE3\x84\xA7\xCB\x87\x20\xE3\x84\x89\xE3\x84\x9C\xCB\x99\x20\xE3\x84\x8D\xE3\x84\xA1\xCB\x87\x20\xE3\x84\xA8\xE3\x84\x9B \xE3\x84\x87\xE3\x84\x9A\xCB\x99";
    /* ㄐㄧㄣ ㄨㄛ ㄧㄣˊ ㄨㄛ ㄅㄨˋ ㄖㄨˊ ㄗˋ ㄐㄧˇ ㄉㄜ˙ ㄍㄡˇ ㄨㄛ ㄇㄚ˙ */

    ctx = chewing_new();

    ret = chewing_userphrase_add(ctx, phrase_in_limit, bopomofo_in_limit);
    ok(ret == 1, "chewing_userphrase_add() return value `%d' shall be `%d'", ret, 1);
    ret = chewing_userphrase_lookup(ctx, phrase_in_limit, bopomofo_in_limit);
    ok(ret == 1, "chewing_userphrase_lookup() return value `%d' shall be `%d'", ret, 1);

    fprintf((FILE*)ffp, "asd %d \n", chewing_get_phoneSeqLen(ctx));

    /*
    ret = sqlite3_exec(ctx->data->static_data.db, 
	    "SELECT phrase FROM userphrase_v1", partition, (void*) fp, NULL);
    if (ret != SQLITE_OK) {
	printf("ERRRRRRRRRRRRRRRRRRRRRRRR");
    }
	*/

    chewing_delete(ctx);
}
