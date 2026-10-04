#ifndef __ACCUM_H
#define __ACCUM_H

struct accum;

struct accum * accum_init(size_t, size_t);

void accum_free(struct accum *);

int accum_add(struct accum *, char *, size_t);

size_t accum_write_callback(char *, size_t, size_t, void *);

#endif /* __ACCUM_H */
