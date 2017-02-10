cmake_minimum_required(VERSION 3.0)

project(sztest)

add_executable(sztest sztest.c)
target_link_libraries(sztest sz z)
