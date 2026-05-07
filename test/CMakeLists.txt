cmake_minimum_required(VERSION 3.20)

project(test LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(test
    "main.c"
    "token_test.c"
)

target_include_directories(test PRIVATE
    ".."
)

target_link_libraries(test PRIVATE
    "lang"
)

target_compile_options(test PRIVATE
    "-Wall"
    "-Wextra"
    "-Werror"
    "-Wstrict-prototypes"
)

add_subdirectory(".." "..")
