cmake_minimum_required(VERSION 3.28)
project(Compiler)

set(CMAKE_CXX_STANDARD 14)

add_executable(Compiler
    src/main.cpp
    src/lexer.cpp
    src/token.cpp
    src/AST.h
    src/lexer.h
    src/token.h
)
