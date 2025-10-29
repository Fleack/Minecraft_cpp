cmake_minimum_required(VERSION 3.16)
project(MinecraftCpp VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

message(STATUS "MinecraftCpp, build type: ${CMAKE_BUILD_TYPE}")

list(APPEND CMAKE_MODULE_PATH
        "${PROJECT_SOURCE_DIR}/extern/magnum-extras/modules"
)
include(cmake/ConanSetup.cmake)
include(cmake/MagnumSetup.cmake)
include(cmake/Dependencies.cmake)

add_subdirectory(shared)
add_subdirectory(server)
add_subdirectory(client)
