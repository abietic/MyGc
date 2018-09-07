cmake_minimum_required(VERSION 3.9)
project(MyGc C)

set(CMAKE_C_STANDARD 99)

add_executable(MyGc main.c dobject.h arena.h arena.c pool.h pool.c block.h block.c)