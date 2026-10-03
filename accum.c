#include <stdlib.h>
#include <string.h>

#include "accum.h"
#include "debug.h"

struct accum {
    size_t size;
    size_t len;
    char   *buf;
    size_t factor;
};

struct accum * accum_init(size_t size, size_t factor)
{
    /* vars */
    char *buf = NULL;
    struct accum *acc = NULL;

    /* begin */
    buf = malloc(size);
    if (!buf) goto err;

    acc = malloc(sizeof(struct accum));
    if (!acc) goto err;

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
