cmake_minimum_required(VERSION 4.4.2)
project(Pavterm LANGUAGES C)

# SDL3
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
find_package(SDL3_ttf CONFIG REQUIRED)

add_executable(pavterm
    src/main.c
    src/pty.c
    src/terminal.c
    src/renderer.c)

target_link_libraries(pavterm PRIVATE
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf)
