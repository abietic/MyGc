#include <stdio.h>
#include <stdlib.h>
#include "dobject.h"
#include "arena.h"
#include "block.h"

extern arena *arena_freelist[];

int main() {
    /*arena** freelist = arena_freelist;
    printf("%p",freelist);
    if (freelist[0]->prev == freelist[1] && freelist[0]->next == freelist[1] && freelist[1])*/
    /*void *spc = malloc(ARENA_SIZE), *fst_pspc = NULL;
    //if not out of memory
    if (spc != NULL) {
        //为分配pool的地址进行地址4k对齐
        char ss[256];
        intptr_t t = spc, rest = 0;
        lltoa(t, ss, 2);
        printf("17. %s\n", ss);
        if ((rest = t % POOL_SIZE) != 0) {
            lltoa(rest, ss, 2);
            printf("20. %s\n", ss);
            rest = POOL_SIZE - rest;
            lltoa(rest, ss, 2);
            printf("23. %s\n", ss);
        }
        fst_pspc = spc + rest;
        lltoa(fst_pspc, ss, 2);
        printf("27. %s\n", ss);
        lltoa(POOL_MASK, ss, 2);
        printf("29. %s\n", ss);
    }*/

//    char buff[70];
//
//    ulltoa(UINT64_C(18446744073709551615), buff, 2);
//
//    printf("%s\n", buff);

    printf("Hello, World!%ld\n",
           sizeof(pool));
    for (int i = 0; i < 945; ++i) {
        void *ps = dobject_malloc(254);

//        printf("%p\n", ps);
    }

    void *p = dobject_malloc(246);

//    printf("%p\n", p);

    /*p = dobject_malloc(360);

    printf("%p", p);

    p = dobject_malloc(480);

    printf("%p", p);*/

    return 0;
}