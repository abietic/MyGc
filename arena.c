//
// Created by iamyh on 2018/9/3.
//

#include "arena.h"
#include <stdlib.h>
#include <string.h>


//全局变量，初始的大块信息数组头，其没有分配空间
arena *arena_array = NULL;

//arena数组的当前大小
uint32_t arena_array_size = 0;

//当前已经分配了的arena数
uint32_t arena_num = 0;

//下一个未分配实际空间的arena结构体的位置
arena *arena_current = NULL;

//GC得到的空arena使用next成员连接
arena *arena_freelist = NULL;

//连接着所有被分配了空间的arena（用了类似Python的used_pool的方法）
arena *used_arena[2] = {USED_ARENA_EMPTY, USED_ARENA_EMPTY};



//申请一个arena对象，但是没有分配空间
arena* malloc_arena() {
    arena *res = NULL;

    //因为malloc_pool在需要申请新arena时已经做过GC了应该不用再GC了
    /*//如果空闲链表为空
    if (arena_freelist == NULL && used_arena != 0) {
        //TODO
        //进行一次GC
    }*/
    //如果freelist不为空从freelist中取出一个
    if (arena_freelist != NULL) {
        //从freelist单向链表中取出栈顶空闲arena
        res = arena_freelist;
        //已分配arena数加一
        ++arena_num;
        //弹栈
        arena_freelist = arena_freelist->next;
        //将取出元素的所有数据置零
        res->next = NULL;
        return res;
    }
    //如果未初始化arena数组则先分配初始空间
    if (arena_array == NULL && arena_array_size == 0) {
        arena_array_size = 4;
        arena_array = calloc(arena_array_size, sizeof(arena));
        arena_current = arena_array;
        arena_num = 0;
    }
    //如果容量到达上限且未到增长上限，则倍长数组
    if (arena_array_size == arena_num && arena_array_size < MAX_ARENA_ARRAY_SIZE) {
        //重分配内存
        arena *new_array = realloc(arena_array, (arena_array_size << 1));
        //若内存分配失败
        if (new_array == NULL) {
            //out of memory
        }
        //内存重分配成功对数组头重新赋值新地址
        arena_array = new_array;
        //将新分配出的部分空间置零
        memset((arena_array + arena_array_size), 0, (arena_array_size * sizeof(arena)));
        //新的下一个
        arena_current = arena_array + arena_array_size;
        arena_array_size <<= 1;
    }
    //取下一个空arena
    res = arena_current++;
    arena_num++;
    return res;
}

//对arena初始化，为其分配空间，成功返回0失败返回错误代码
uint8_t arena_init(arena *ar) {
    uint8_t success = 0;
    if (ar->space == NULL) {
        void *spc = malloc(ARENA_SIZE), *fst_pspc = NULL;

        if (spc == NULL) {
            //out of memory
        } else {
            //为分配pool的地址进行地址4k对齐
            intptr_t t = (intptr_t)spc, rest = 0;
            if ((rest = t % POOL_SIZE) != 0) {
                rest = POOL_SIZE - rest;
            }

            fst_pspc = spc + rest;

            //在初次使用和回收后都会进行置0所以此处不用对未初始化部分初始化
            ar->space = spc;
            ar->fst_pool = fst_pspc;
            ar->pools_contain = ARENA_SIZE / POOL_SIZE;
            if (rest != 0) {
                ar->pools_contain--;
                ar->bits |= MAYBE_EMPTY_ARENA;
            }

            //链入used双向链表尾
            INSERT_BACK_USED_ARENA(ar);
        }

    }
    return success;
}

//对已经没有pool占用的arena进行空间的释放，并将其链入freelist
void free_arena(arena *ar) {
    if (ar == NULL || ar->bits != 0) {
        return;
    }
    if (ar->space != NULL && ar->bits == 0) {
        //释放空间
        free(ar->space);

        //从used链表摘除
        ar->prev->next = ar->next;
        ar->next->prev = ar->prev;

        //清空结构体数据
        memset(ar, 0, sizeof(arena));

        //链入freelist
        ar->next = arena_freelist;
        arena_freelist = ar;
    }

    //对used_arena重新排序
    sort_used_arena();
}



void sort_used_arena() {
    if (used_arena[0] == USED_ARENA_EMPTY) {
        return;
    }
    bool changed = true;
    while (changed) {
        changed = false;
        for (arena *i = used_arena[0]; i->next != USED_ARENA_EMPTY; i = i->next) {
            arena *j = i->next;
            int cmp_res;
            COMPARE_HAMMING_WEIGHT((i->bits),(j->bits),cmp_res);
            if (cmp_res == LT) {
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