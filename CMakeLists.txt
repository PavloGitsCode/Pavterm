cmake_minimum_required(VERSION 4.4.2)
project(Pavterm LANGUAGES C)

# SDL3
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

add_executable(pavterm src/main.c)

target_link_libraries(pavterm PRIVATE SDL3::SDL3)
