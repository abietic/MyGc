//
// Created by iamyh on 2018/9/4.
//

#include "block.h"
#include "stdlib.h"

extern const uint16_t const idx2blksz[];

extern const uint8_t const idx2maxus[];

extern pool *used_pool[];

large_dobject_header *used_ldobject[2] = {USED_LDOBJECT_EMPTY, USED_LDOBJECT_EMPTY};


void *large_dobject_malloc(size_t nbytes);

void *dobject_malloc(size_t nbytes) {
    //如果大于最大可分配块，分配成大对象
    if (nbytes > MAX_BLOCK_SIZE) {
        return large_dobject_malloc(nbytes);
    }
    //best-fit寻找可用block
    int idx;
    for (idx = 0; idx < USED_POOL_SIZE; ++idx) {
        if (idx2blksz[idx] >= nbytes) {
            break;
        }
    }
    //在相应block-size的used_pool中寻找有空位的pool
    pool *pl = used_pool[idx * 2];
    bool is_empty = true;
    if (pl != USED_POOL_EMPTY(idx)) {
        is_empty = false;
    }
    while (pl != USED_POOL_EMPTY(idx)) {
        if (count_used(pl, idx) < idx2maxus[idx]) {
            break;
        }
        pl = pl->next;
    }
    //如果有pool没有空位
    if (pl == USED_POOL_EMPTY(idx) && is_empty == false) {


        ///////////
        //进行一次GC
        ///////////

        //再次寻找空位
        pl = used_pool[idx * 2];

        while (pl != USED_POOL_EMPTY(idx)) {
            if (count_used(pl, idx) < idx2maxus[idx]) {
                break;
            }
            pl = pl->next;
        }

    }
    //如果没有有空位的，那么申请一个新的pool
    if (pl == USED_POOL_EMPTY(idx)) {
        pool *new_pl = malloc_pool();
        if (new_pl == NULL) {
            //allocation fail
        }
        pool_init(new_pl,idx);
        pl = new_pl;
    }
    //找到有空位的pool中第一个空闲的位置
    uint16_t fst_bst = get_first_fit(pl);

    if (fst_bst == 0) {
        //get fail
    }

    //生成相应的block地址
    void *blk_addr = (void*)pl + fst_bst * idx2blksz[idx];
    //对已分配的空间置位防止被二次分配
    set_bit(pl, (intptr_t)blk_addr);
    //对used_pool重新排序
    sort_used_pool(idx);
    return blk_addr;
}


void *large_dobject_malloc(size_t nbytes) {
    void *res = NULL;
    res = calloc(nbytes, sizeof(uint8_t));
    if (res == NULL) {
        /////////////
        //执行GC
        ////////////
        res = calloc(nbytes, sizeof(uint8_t));
        if (res == NULL) {
            //allocation fail
        }
    }

    large_dobject_header *header = res;
    large_dobject_header *h = used_ldobject[0];
    header->next = h->next;
    header->prev = h;
    h->next->prev = header;
    h->next = header;
    return res;
}