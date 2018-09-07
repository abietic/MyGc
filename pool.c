//
// Created by iamyh on 2018/9/3.
//

#include "pool.h"
#include <string.h>


extern arena *used_arena[];
extern arena *arena_array;

// block_sz 16 32 64 128 256
pool *used_pool[USED_POOL_SIZE * 2] = {PT(0), PT(1), PT(2), PT(3), PT(4)};

const uint16_t const idx2blksz[] = {16, 32, 64, 128, 256};

const uint8_t const idx2bitsz[] = {4, 2, 1, 1, 1};

const uint16_t const idx2maxus[] = {256, 128, 64, 32, 16};

pool *malloc_pool() {
    pool *pl = NULL;
    arena *ar = NULL;
    uint64_t b;

    //如果有已经使用的arena
    if (used_arena[0] != USED_ARENA_EMPTY) {
        //看是否有空着的pool
        for (ar = used_arena[0]; ar != USED_ARENA_EMPTY; ar = ar->next) {
            b = ar->bits;
            if ((~b) != 0)
                break;
        }
        //如果全满了
        if (ar == NULL || ar == USED_ARENA_EMPTY) {


            ///////////
            //进行一次GC
            ///////////
            //TODO

            //再进行一次搜索
            if (used_arena[0] != USED_ARENA_EMPTY) {
                for (ar = used_arena[0]; ar != USED_ARENA_EMPTY; ar = ar->next) {
                    b = ar->bits;
                    if ((~b) != 0)
                        break;
                }
            }
        }
    }

    //如果没有已使用的arena或进行一次GC后仍都满
    if (ar == NULL || ar == USED_ARENA_EMPTY) {

        //创建一个新arena
        if ((ar = malloc_arena()) == NULL) {
            //allocation fail
        }
        //对新arena进行初始化
        arena_init(ar);
    }

    //在得到的arena中寻找到一个空闲的pool
    int i;
    for (i = 0, b = ar->bits; i < BITS_SIZE; ++i, b >>= 1) {
        if ((b & 1) == 0)
            break;
    }

    //对要分配的pool区位置位
    ar->bits |= (1 << i);

    //取得空闲pool的地址
    pl = (void*)(ar->fst_pool) + i * POOL_SIZE;

    //由于pool对象必须赋值arena_index只能在此处初始化
    intptr_t offset = (intptr_t)((void*)ar - (void*)arena_array);

    pl->arena_index = (uint16_t)offset;

    //重新排序used_arena
    sort_used_arena();


    return pl;
}

uint8_t pool_init(pool *pl, uint8_t idx) {
    uint8_t success = 0;
    if (pl == NULL) {
        //没分配上空间
    }
    if (idx > (USED_POOL_SIZE - 1)) {
        //超出索引范围
    }

    //赋值位于哪个used_pool
    pl->used_pool_index = idx;

    //为位图置零
    memset(pl->bits, 0, sizeof(uint64_t) * 4);
    //将pool头部占用的位置置位
    for (int i = idx2bitsz[idx], j = 0; i > 0 ; --i, ++j) {
        pl->bits[0] |= (uint64_t)(1 << j);
    }

    //插入used_pool尾
    INSERT_BACK_USED_POOL(pl,idx);

    return success;
}


void free_pool(pool *pl) {
    if (pl == NULL) {
        return;
    }


    //从used_pool中摘除
    pl->prev->next = pl->next;
    pl->next->prev = pl->prev;

    //获得pool所在的arena
    arena* ar = (void*)arena_array + (pl->arena_index) * ARENA_SIZE;

    //获得pool在arena中的位置
    intptr_t lc = ((intptr_t)pl - (intptr_t)ar->fst_pool) / POOL_SIZE;

    //对相应arena位图进行置位
    ar->bits ^= (1 << lc);

    //如果arena为空，释放arena
    if (ar->bits == 0 || (ar->bits == MAYBE_EMPTY_ARENA && ar->pools_contain == (ARENA_SIZE/POOL_SIZE - 1))) {
        ar->bits ^= ar->bits;
        free_arena(ar);
    }
    //重新排序used_pool
    sort_used_pool(pl->used_pool_index);
}

uint16_t count_used(pool *pl, uint8_t idx) {
    if (pl == NULL || idx > (USED_POOL_SIZE - 1)) {
        return 0;
    }
    uint16_t sum;
    for (int i = 0; i < idx2bitsz[idx]; ++i) {
        uint16_t s;
        HAMMING_WEIGHT(pl->bits[i],s);
        sum += s;
    }
    return sum;
}

void sort_used_pool(uint8_t idx) {
    if (idx > (USED_POOL_SIZE - 1)) {
        return;
    }
    if (used_pool[idx * 2] == USED_POOL_EMPTY(idx)) {
        return;
    }
    bool changed = true;
    while (changed) {
        changed = false;
        for (pool *i = used_pool[idx * 2]; i->next != USED_POOL_EMPTY(idx); i = i->next) {
            pool *j = i->next;
            uint16_t cmp_i = count_used(i, idx), cmp_j = count_used(j,idx);
            if (cmp_i < cmp_j) {
                i->next = j->next;
                j->prev = i->prev;
                i->prev->next = j;
                j->next->prev = i;
                i->prev = j;
                j->next = i;
                i = j;
                changed = true;
            }
        }
    }
}

uint16_t get_first_fit(pool *pl) {
    if (pl == NULL) {
        return 0;
    }
    uint8_t idx = pl->used_pool_index;
    uint16_t max = idx2maxus[idx];
    uint16_t count = 0;
    for (int i = 0; i < idx2bitsz[idx] && count < max; ++i) {
        uint64_t b = pl->bits[i];
        for (int j = 0; j < 64 && count < max; ++j, ++count) {
            if ((b & (1 << j)) == 0) {
                return count;
            }
        }
    }
    return 0;
}

uint8_t set_bit(pool *pl, intptr_t addr) {
    uint8_t success = 0;
    if (pl == NULL || (intptr_t)pl & (POOL_SIZE - 1) != 0) {
        //invalid pool address
        success = 1;
    }
    if (addr >= ((intptr_t)pl + POOL_SIZE)) {
        //invalid block address
    }
    uint16_t size = idx2blksz[pl->used_pool_index];
    int bit_loc = (addr - (intptr_t)pl) / size;
    int b_idx = bit_loc / 64, b_offset = bit_loc % 64;
    pl->bits[b_idx] |= (1 << b_offset);
    return success;
}