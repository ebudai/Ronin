cmake_minimum_required(VERSION 3.28)
project(Test)
set(EXECUTABLE_OUTPUT_PATH "..")
set(CMAKE_CXX_STANDARD 26)

find_package(ICU REQUIRED)
add_executable(Test ../lexer.h ../lex.cpp ../grammar.h ../parse.cpp
    association.cpp
    block.cpp
    date_literal.cpp
    delegate.cpp
    exporter.cpp
    extension.cpp
    function.cpp
    importer.cpp
    keyword.cpp
    literal.cpp
    loop.cpp
    multiline_comment.cpp
    numeric_literal.cpp
    punctuation.cpp
    single_line_comment.cpp
    symbol.cpp
    test_lexer.h
    test_parser.h
    text_literal.cpp
    type.cpp
    whitespace.cpp)

add_compile_options(--coverage)

target_link_libraries(Test ICU)
target_link_libraries(Test gtest gtest_main)
