#include <stdlib.h>
#include <string.h>

#include "accum.h"
#include "debug.h"

#include "curl/curl.h"

struct accum * accum_init(size_t size, size_t factor)
{
    /* vars */
    char *buf = NULL;
    struct accum *acc = NULL;

    /* begin */
    buf = malloc(size);
    if (!buf) {
        debug("failed to malloc(size)");
        goto err;
    }

    acc = malloc(sizeof(struct accum));
    if (!acc) {
        debug("failed to malloc struct accum");
        goto err;
    }

    acc->size = size;
    acc->len = 0;
    acc->buf = buf;
    acc->factor = factor;
    return acc;

err:
    if (buf) free(buf);
    if (acc) free(acc);
    return NULL;
}

void accum_free(struct accum *acc)
{
    if (!acc) return;
    free(acc->buf);
    free(acc);
}

int accum_add(struct accum *acc, char *data, size_t len)
{
    if (!acc) return 0;
    size_t rem = acc->size - acc->len;
    debug("adding data, len: %lu, rem: %lu", len, rem);
    if (len > rem) {
        size_t size_new = ((acc->size > len) ? acc->size : len) * acc->factor;
        debug("resizing, size new: %lu", size_new);
        void *tmp = realloc(acc->buf, size_new);
        if (!tmp) {
            debug("failed to resize");
            return 0;
        }
        acc->buf = tmp;
        acc->size = size_new;
        debug("resising ok");
    }
    memcpy(acc->buf + acc->len, data, len);
    acc->len += len;
    debug("acc size: %lu, len: %lu", acc->size, acc->len);
    return 1;
}

size_t accum_write_callback(char *data, size_t size, size_t nmemb, void *userdata)
{
    debug("accumulator callback gets called");
    size_t len = size * nmemb;
    struct accum *acc = (struct accum *) userdata;
    if (!accum_add(acc, data, len)) return CURL_WRITEFUNC_ERROR;
    return len;
}
