cmake_minimum_required(VERSION 3.0)

project(sztest C)

set(CMAKE_C_STANDARD 99)

add_executable(sztest sztest.c)
target_link_libraries(sztest sz z m)
