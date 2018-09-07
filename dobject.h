//
// Created by iamyh on 2018/8/30.
//

#ifndef MYGC_DOBJECT_H
#define MYGC_DOBJECT_H

#include <stdint.h>
#include <stdbool.h>


//the width is 16 in my machine
typedef union {
    int64_t intrger;
    double real;
    void *ptr;
} dvalue;


//I will define this later
typedef struct {
    uint8_t type;
    uint8_t mark; //暂且使用简单的标记清除
    uint8_t next; //在block的freelist中连接单链表使用
    bool is_large_block;
} dobject_header;

#ifndef NIL
#define NIL 0
#endif


#ifndef INTEGER
#define INTEGER 1
#endif


#ifndef REAL
#define REAL 2
#endif


#ifndef BOOL
#define BOOL 3
#endif


#ifndef BYTEARRAY
#define BYTEARRAY 4
#endif

#ifndef ARRAY
#define ARRAY 5
#endif

#ifndef FUNCTION
#define FUNCTION 6
#endif


#ifndef CLASS
#define CLASS 7
#endif


#ifndef PROTOTYPE
#define PROTOTYPE 8
#endif

#ifndef UPVALUE
#define UPVALUE 9
#endif

#ifndef TUPLE
#define TUPLE 10
#endif

#ifndef MAP
#define MAP 11
#endif

typedef struct {
    dobject_header header;
    dvalue dval;
} dobject;

#endif //MYGC_DOBJECT_H
