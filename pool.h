//
// Created by iamyh on 2018/9/3.
//

#ifndef MYGC_POOL_H
#define MYGC_POOL_H

#include <stdint.h>

#include "arena.h"

#define POOL_SIZE (4<<10)

#define MIN_BLOCK_SIZE 16
#define MAX_BLOCK_SIZE 256

#define POOL_MASK (POOL_SIZE - 1)

#define BITS_SIZE (ARENA_SIZE / POOL_SIZE)

typedef struct pool pool;

struct pool {
    pool *next, *prev;
    uint16_t arena_index;
    uint8_t used_pool_index;
    uint8_t blk_freelist;
    uint8_t nxt_blk;
    uint64_t bits[4];
};



#define MAX_64 UINT64_MAX

#define MAX_128 ((uint64_t)UINT32_MAX)

#define MAX_256 ((uint64_t)UINT16_MAX)


#define INSERT_BACK_USED_POOL(ie_pl,IDX) do{pool *i_pl = used_pool[((IDX)*2)]->prev;(ie_pl)->prev=i_pl->prev;i_pl->prev->next=(ie_pl);(ie_pl)->next=i_pl;i_pl->prev=(ie_pl);}while(false)

#define INSERT_FRONT_USED_POOL(ie_pl,IDX) do{pool *i_pl = used_pool[((IDX)*2)];(ie_pl)->prev=i_pl->prev;i_pl->prev->next=(ie_pl);(ie_pl)->next=i_pl;i_pl->prev=(ie_pl);}while(false)

/*#define ARENA_INDEX_MASK (((1<<10)-1)<<6)

#define GET_POOLS_ARENA_INDEX(PL) (((PL)->ar_pl_index)>>6)

#define SET_POOLS_ARENA_INDEX(PL,IDX) do{(PL)->ar_pl_index &= (uint16_t)(~ARENA_INDEX_MASK);(PL)->ar_pl_index |= ((IDX)<<6);}while(false)

#define GET_USED_POOL_INDEX(PL) (((PL)->ar_pl)&(~ARENA_INDEX_MASK))

#define SET_USED_POOL_INDEX(PL,IDX) do{(PL)->ar_pl_index &= (uint16_t)ARENA_INDEX_MASK;(PL)->ar_pl_index |= (IDX);}while(false)*/



#define USED_POOL_SIZE 5

#define PT(X) PTA((X)*2), PTA((X)*2)

#define PTA(X) ((pool*)((void*)(&used_pool[X]) - offsetof(pool, next)))

#define USED_POOL_EMPTY(X) PTA((X)*2)

pool *malloc_pool();

uint8_t pool_init(pool *pl, uint8_t idx);

void free_pool(pool *pl);

uint16_t count_used(pool *pl, uint8_t idx);

void sort_used_pool(uint8_t idx);

uint16_t get_first_fit(pool *pl);

uint8_t set_bit(pool *pl, intptr_t addr);

#endif //MYGC_POOL_H
