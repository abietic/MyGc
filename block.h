//
// Created by iamyh on 2018/9/4.
//

#ifndef MYGC_BLOCK_H
#define MYGC_BLOCK_H

#include "pool.h"
#include "arena.h"
#include "dobject.h"



typedef struct large_dobject_header large_dobject_header;

struct large_dobject_header {
    dobject_header header;
    large_dobject_header *next, *prev;
};

#define USED_LDOBJECT_EMPTY ((large_dobject_header*)((void*)used_ldobject - offsetof(large_dobject_header, next)))

void *dobject_malloc(size_t nbytes);

#endif //MYGC_BLOCK_H
