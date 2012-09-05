/*
 * MD5 utilities.
 * $Rev: 258 $
 */

#include <stdio.h>     // FILE, fopen, fseek, fread, fclose
#include <stdlib.h>    // calloc
#include <assert.h>    // assert
#include <string.h>    // strlen

#include "md5_utils.h"

#define BUFF 256

char *to_hexa(md5_byte_t *digest, size_t len) {
    unsigned int i = 0;
    char *result = NULL;

/* PRE: */assert(digest != NULL);
/* PRE: */assert(len > 0);

    result = (char *)calloc(len*2 + 1, sizeof(char));
    assert(result != NULL);

    for (i = 0; result != NULL && i < len; ++i) {
        sprintf(result + i * 2, "%02x", digest[i]);
    }

/* POST: */assert(result != NULL);
/* POST: */assert(strlen(result) == len*2);

    return result;
}

char *slice_md5(const char *a_path, unsigned int offset, unsigned int len) {
    char *result = NULL;
    md5_state_t state;
    md5_byte_t digest[16], content[BUFF];
    FILE *f = NULL;
    int f_res = 0;
    int i = 0, j = 0;

/* PRE: assert(valid_path_name(a_path)); */
/* PRE: assert(path_is_file(a_path)); */
/* PRE: assert(offset <= path_size(a_path)); */
/* PRE: assert(len < path_size(a_path) - offset); */

    //printf("md5_utils.c: slice_md5: a_path -> %s\n", a_path);
    f = fopen(a_path, "r");
    assert(f != NULL);

    f_res = fseek(f, offset, SEEK_SET);
    assert(f_res == 0);

    md5_init(&state);

    i = len;
    do {
        j = MIN_(i, BUFF);
        //printf("md5_utils.c: slice_md5: i -> %i, j -> %i\n", i, j);
        assert(!feof(f));
        f_res = fread(&content, sizeof(md5_byte_t), j, f);
        assert(f_res == j);
        md5_append(&state, (const md5_byte_t *)content, j);
        i -= j;
    } while (i > 0);

    md5_finish(&state, digest);
    fclose(f);

    result = to_hexa(digest, 16);
    //printf("md5_utils.c: slice_md5: result = %s\n", result);

/* POST: */assert(result != NULL);
/* POST: */assert(strlen(result) == 32);

    return result;
}
