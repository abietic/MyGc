//
// Created by iamyh on 2018/9/3.
//

#ifndef MYGC_ARENA_H
#define MYGC_ARENA_H

#include <stdint.h>
#include "pool.h"
#include <stdbool.h>


#define ARENA_SIZE (256<<10)

typedef struct arena arena;

typedef struct pool pool;

struct arena{
    void *space;
    pool *fst_pool;
    uint8_t pools_contain;
    arena *next, *prev;
    uint64_t bits;
} ;


//已经使用了
#define USED_ARENA_EMPTY ((arena*)((void*)used_arena - offsetof(arena, next)))

#define MAYBE_EMPTY_ARENA (UINT64_C(1) << 63)

#define INSERT_BACK_USED_ARENA(ie_ar) do{arena* i_ar=used_arena[0]->prev;(ie_ar)->prev=i_ar->prev;(ie_ar)->next=i_ar;i_ar->prev->next=(ie_ar);i_ar->prev=(ie_ar);}while(false)

#define INSERT_FRONT_USED_ARENA(ie_ar) do{arena* i_ar=used_arena[0];(ie_ar)->prev=i_ar->prev;(ie_ar)->next=i_ar;i_ar->prev->next=(ie_ar);i_ar->prev=(ie_ar);}while(false)

#define MAX_ARENA_ARRAY_SIZE (1 << 10)


#define HAMMING_WEIGHT(IN_PUT,RES_STORE) do{int one_cnt=0;uint64_t b=(IN_PUT);while(b!=0){b&=(b-1);one_cnt++;}(RES_STORE)=one_cnt;}while(false)

#define COMPARE_HAMMING_WEIGHT(IN_PUT_I,IN_PUT_J,CMP_RES) do{int cmp_i,cmp_j;HAMMING_WEIGHT((IN_PUT_I),cmp_i);HAMMING_WEIGHT((IN_PUT_J),cmp_j);(CMP_RES)=(cmp_i>cmp_j)?1:((cmp_i==cmp_j)?0:(-1));}while(false)

#define GT 1

#define LT -1

#define EQ 0


arena* malloc_arena();

uint8_t arena_init(arena *ar);

void free_arena(arena *ar);

void sort_used_arena();


#endif //MYGC_ARENA_H
