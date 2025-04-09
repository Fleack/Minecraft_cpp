cmake_minimum_required(VERSION 3.16)
project(Minecraft VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Wshadow
        -Wnon-virtual-dtor
        -Woverloaded-virtual
        -Wnull-dereference
        -Wimplicit-fallthrough)

# libs
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(glad REQUIRED)
find_package(stb REQUIRED)
find_package(fastnoise-lite REQUIRED)
find_package(imgui REQUIRED)
find_package(spdlog REQUIRED)
find_package(concurrencpp REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)
add_subdirectory(test)

foreach (dir shaders assets)
    file(COPY ${CMAKE_SOURCE_DIR}/${dir} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endforeach ()
