cmake_minimum_required(VERSION 3.28)
project(Compiler)

set(CMAKE_CXX_STANDARD 20)

enable_testing()
add_subdirectory(test)

add_executable(Compiler
    src/main.cpp
    src/lexer.cpp
    src/token.cpp
        src/ast.h
    src/lexer.h
    src/token.h
        src/parser.cpp
        src/parser.h
        src/ast.cpp
        src/type.cpp
        src/type.h
)
