cmake_minimum_required(VERSION 3.28)
project(Ronin)

set(CMAKE_CXX_STANDARD 26)

set(ICU_INCLUDE_DIR "C:/Program Files (x86)/Windows Kits/10/Include/10.0.22621.0/um")
set(ICU_LIBRARY "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.22621.0/um/x64")
find_package(ICU REQUIRED)

add_executable(Ronin
        lib/tagged_ptr.hpp
        lib/ctre.hpp
        lib/magic_enum.hpp
        context.cpp
        context.h
        error.h
        lex.cpp
        lexer.h
        lexicon.h
        parse.cpp
        grammar.h
        ronin.cpp
        semantics.cpp
        semantics.h
        token.h)

target_link_libraries (Ronin ICU)

add_subdirectory(test)

include(FetchContent)
FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)