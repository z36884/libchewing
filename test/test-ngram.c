/**
 * test-ngram.c
 *
 * Copyright (c) 2004, 2005, 2008, 2011
 *      libchewing Core Team. See ChangeLog for details.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chewing.h"
#include "testhelper.h"

FILE *fd;

void test_ngram()
{
    int i, j, k, m, length, correct, total, pos; 
    ChewingContext *ctx;
    char word_input[50], phone_input[50];

    char test_input[5][10][50] = {
	{"3", "ji up6ji <E>","窩銀窩", 
	    "\xE3\x84\xA8\xE3\x84\x9B", "\xE3\x84\xA7\xE3\x84\xA3\xCB\x8A", "\xE3\x84\xA8\xE3\x84\x9B"}, /* 窩銀窩 */
	{"3", "ru8 aj4xj4<E>", "家目錄", 
	    "\xE3\x84\x90\xE3\x84\xA7\xE3\x84\x9A", "\xE3\x84\x87\xE3\x84\xA8\xCB\x8B", "\xE3\x84\x8C\xE3\x84\xA8\xCB\x8B"},                        /*"家目錄"*/
	{"4", "j;3ru4j;3xj4<E>", "網際網路", 
	    "\xE3\x84\xA8\xE3\x84\xA4\xCB\x87", "\xE3\x84\x90\xE3\x84\xA7\xCB\x8B", "\xE3\x84\xA8\xE3\x84\xA4\xCB\x87", "\xE3\x84\x8C\xE3\x84\xA8\xCb\x8B"},                /*"網際網路"*/
	{"4", "yjo4rup4d9 fu3<E>", "最近開啟", 
	    "\xE3\x84\x97\xE3\x84\xA8\xE3\x84\x9F\xCb\x8B", "\xE3\x84\x90\xE3\x84\xA7\xE3\x84\xA3\xCb\x8B", "\xE3\x84\x8E\xE3\x84\x9E", "\xE3\x84\x91\xE3\x84\xA7\xCb\x87"}             /*"最近開啟"*/
    };

    start_testcase(ctx,fd);

    ctx = chewing_new();

    for (i = 0; i < 4; i++) {
	chewing_Reset(ctx);
	chewing_set_maxChiSymbolLen(ctx, 16);
	type_keystroke_by_string(ctx, test_input[i][1]);

	pos = correct = total = 0;
	length = atoi(test_input[i][0]);

	printf("Test Case : %d. This case is %s\n", i+1, test_input[i][2]);

	for (j = 0; j < length; j++) {
	    for (k = j; k < length; k++) {
		total++;
		memset(word_input, 0, sizeof(word_input));
		memset(phone_input, 0, sizeof(phone_input));

		for (m = j*3; m < j*3+(k-j+1)*3; m++) {
		    word_input[m-j*3] = test_input[i][2][m];
		}
		word_input[m] = '\0';

		for (m = j; m <= k; m++) {
		    strcat(phone_input, test_input[i][m+3]);
		    if(m != k)
			strcat(phone_input, " ");
		}

		ok(chewing_userphrase_lookup(ctx, word_input, phone_input), "find %s in the userphrase database", word_input);

		if (chewing_userphrase_lookup(ctx, word_input, phone_input)) {
		    correct++;
		}
	    }
	}
	printf("correct / times => %d / %d , ratio = %.2lf%\n\n", correct, total, (double)(correct/total)*100);
    }

    chewing_delete(ctx); 
}

int main(int argc, char *argv[])
{
    char *logname;
    int ret;

    putenv("CHEWING_PATH=" CHEWING_DATA_PREFIX);
    putenv("CHEWING_USER_PATH=" TEST_HASH_DIR);

    ret = asprintf(&logname, "%s.log", argv[0]);
    if (ret == -1)
        return -1;
    fd = fopen(logname, "w");
    assert(fd);
    free(logname);

    test_ngram();

    fclose(fd);

    return 0;
}
