#include <stdlib.h>
#include <string.h>
#include "accum.h"
#include "logging.h"
#include "macros.h"
#include "curl/curl.h"

struct accum * accum_init(size_t size, size_t factor)
{
    /* vars */
    char *buf = NULL;
    struct accum *acc = NULL;

    /* begin */
    buf = malloc(size);
    if (!buf) {
        log_error("failed to malloc(size)");
        goto err;
    }

    acc = malloc(sizeof(struct accum));
    if (!acc) {
        log_error("failed to malloc struct accum");
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
    log_debug("adding data, len: %lu, rem: %lu", len, rem);
    if (len > rem) {
        size_t size_new = _MAX(acc->size, len) * acc->factor;
        log_debug("resizing, size new: %lu", size_new);
        void *tmp = realloc(acc->buf, size_new);
        if (!tmp) {
            log_error("failed to resize, size new: %lu", size_new);
            return 0;
        }
        acc->buf = tmp;
        acc->size = size_new;
        log_debug("resising ok");
    }
    memcpy(acc->buf + acc->len, data, len);
    acc->len += len;
    log_debug("acc size: %lu, len: %lu", acc->size, acc->len);
    return 1;
}

size_t accum_write_callback(char *data, size_t size, size_t nmemb, void *userdata)
{
    log_debug("accumulator callback gets called");
    size_t len = size * nmemb;
    struct accum *acc = (struct accum *) userdata;
    if (!accum_add(acc, data, len)) return CURL_WRITEFUNC_ERROR;
    return len;
}
