#ifndef __CURLOPTS_H__
#define __CURLOPTS_H__

struct curlopts;

struct curlopts * curlopts_init(size_t);

void curlopts_free(struct curlopts *);

int curlopts_add(struct curlopts *, int, long);

#endif /* __CURLOPTS_H__ */
