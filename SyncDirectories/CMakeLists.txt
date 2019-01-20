cmake_minimum_required(VERSION 2.8)

project(Sync)

add_library("Cmd" "Command.cpp")
add_library(CE CommandExecutor.cpp)
add_library(A Analyze.cpp)
add_library(H Help.cpp)
add_library(P Perform.cpp)

add_executable(main main.cpp)

target_link_libraries(main ${CMAKE_SOURCE_DIR}/libs/libcryptopp.a)

target_link_libraries(main "Cmd")
target_link_libraries(main CE)
target_link_libraries(main A)
target_link_libraries(main H)
target_link_libraries(main P)
