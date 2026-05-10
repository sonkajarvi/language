cmake_minimum_required(VERSION 3.20)

project(play LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(play
    "main.c"
)

target_include_directories(play PRIVATE
    ".."
)

target_link_libraries(play PRIVATE
    "lang"
)

target_compile_options(play PRIVATE
    "-Wall"
    "-Wextra"
    "-Werror"
    "-Wstrict-prototypes"
)

add_subdirectory(".." "..")
