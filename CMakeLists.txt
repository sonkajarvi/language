cmake_minimum_required(VERSION 3.20)

project(lang LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(lang
    "main.c"

    "src/ast.c"
    "src/lexer.c"
    "src/node.c"
    "src/op.c"
)

target_compile_options(lang PRIVATE
    "-Wall"
    "-Wextra"
    "-Werror"
    "-Wstrict-prototypes"
)
