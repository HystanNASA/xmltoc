project(xmltoc)

set(CMAKE_CXX_STANDARD 17)
# Todo: add static analysis: clang -std=c++17 --analyze src/*

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -fsanitize=address")

# add_subdirectory(src)

set(SOURCE_FILES src/main.cpp 
                 src/utils.hpp
                 src/Lexer.hpp src/Lexer.cpp 
                 src/Parser.hpp src/Parser.cpp
                 src/CodeGenerator.hpp src/CodeGenerator.cpp
                 src/XMLToC.hpp)

add_executable(xmltoc ${SOURCE_FILES})
