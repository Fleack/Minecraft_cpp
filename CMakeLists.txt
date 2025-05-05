cmake_minimum_required(VERSION 3.16)
project(Minecraft VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

message(STATUS "Project Minecraft, build type: ${CMAKE_BUILD_TYPE}")

list(APPEND CMAKE_MODULE_PATH
        "${PROJECT_SOURCE_DIR}/extern/magnum-extras/modules"
)
include(cmake/ConanSetup.cmake)
include(cmake/MagnumSetup.cmake)
include(cmake/Dependencies.cmake)
include(cmake/InstallAssets.cmake)

add_subdirectory(src)
# add_subdirectory(test)
