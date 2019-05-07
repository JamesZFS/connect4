cmake_minimum_required(VERSION 3.12)
project(UCT)

set(CMAKE_CXX_STANDARD 14)

aux_source_directory(. SRCS)

add_executable(UCT ${SRCS})
