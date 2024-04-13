cmake_minimum_required(VERSION 3.16)
project(mish)

add_executable(mish mish.cpp
    stringParser.cpp
    commandRunner.cpp
    inputLoop.cpp
    runFile.cpp)

target_link_libraries(mish readline)