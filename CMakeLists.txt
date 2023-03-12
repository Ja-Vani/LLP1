cmake_minimum_required(VERSION 3.16)
project(LLP1 C)

set(CMAKE_C_STANDARD 11)

add_library(data data.c database.c idata.c test_database.c)

add_executable(LLP1 main.c)
target_link_libraries(LLP1 data)