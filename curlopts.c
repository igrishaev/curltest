#include <stdlib.h>
#include <string.h>

#include "curlopts.h"
#include "debug.h"

struct curlopts {
    size_t i;
    size_t count;
    size_t factor;
    int    *opts;
    long   *vals;
};

struct curlopts * curlopts_init(size_t count)
{
    int *opts = NULL;
    long *vals = NULL;
    struct curlopts * co = NULL;

    opts = malloc(sizeof(int) * count);
    if (!opts) goto err;

    vals = malloc(sizeof(long) * count);
    if (!vals) goto err;

    co = malloc(sizeof(struct curlopts));
    if (!co) goto err;

    co->i      = 0;
    co->count  = count;
    co->factor = 2;
    co->opts   = opts;
    co->vals   = vals;

    return co;

err:
    if (opts) free(opts);
    if (vals) free(vals);
    return NULL;
}

void curlopts_free(struct curlopts * co)
{
    if (!co) return;
    free(co->opts);
    free(co->vals);
    free(co);
}

int curlopts_add(struct curlopts * co, int opt, long val)
{
    if (!co) return 0;

    co->i++;
    if (co->i == co->count) {
        size_t count_new = co->count * co->factor;
        void *tmp;

        tmp = realloc(co->opts, sizeof(int) * count_new);
        if (!tmp) return 0;
        co->opts = tmp;

        tmp = realloc(co->vals, sizeof(long) * count_new);
        if (!tmp) return 0;
        co->vals = tmp;

        co->count = count_new;
    }
    co->opts[co->i] = opt;
    co->vals[co->i] = val;
    return 1;
}
