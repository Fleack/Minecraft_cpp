cmake_minimum_required(VERSION 3.16)
project(Minecraft VERSION 0.1 LANGUAGES C CXX)

set(CONAN_TOOLCHAIN_FILE
        "${CMAKE_BINARY_DIR}/conan/build/${CMAKE_BUILD_TYPE}/generators/conan_toolchain.cmake"
)

if (EXISTS "${CONAN_TOOLCHAIN_FILE}")
    message(STATUS "Including Conan toolchain: ${CONAN_TOOLCHAIN_FILE}")
    include("${CONAN_TOOLCHAIN_FILE}")
else ()
    message(WARNING "Conan toolchain not found at ${CONAN_TOOLCHAIN_FILE}")
endif ()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# external libs # TODO Rework for external installation
set(CORRADE_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(MAGNUM_BUILD_STATIC ON CACHE BOOL "" FORCE)
add_subdirectory(extern/corrade)
add_subdirectory(extern/magnum)
add_subdirectory(extern/magnum-extras)

# MUST be after add_subdirectory of extern
# to avoid compilation errors of external libs
add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Woverloaded-virtual
        -Wnull-dereference
        -Wimplicit-fallthrough)

# libs
find_package(SDL2 REQUIRED)
find_package(fastnoise-lite REQUIRED)
find_package(imgui REQUIRED)
find_package(spdlog REQUIRED)
find_package(concurrencpp REQUIRED)
find_package(cpptrace REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)
add_subdirectory(test)

foreach (dir shaders assets)
    file(COPY ${CMAKE_SOURCE_DIR}/${dir} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endforeach ()
