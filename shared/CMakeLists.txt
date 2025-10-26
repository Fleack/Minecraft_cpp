cmake_minimum_required(VERSION 3.16)

file(GLOB_RECURSE SHARED_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(Shared STATIC ${SHARED_SRC})

target_compile_options(Shared PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wnull-dereference
    -Wimplicit-fallthrough
)

target_include_directories(Shared
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/extern/enet/include
    ${PROJECT_SOURCE_DIR}/extern/magnum/src
    ${PROJECT_SOURCE_DIR}/extern/corrade/src
)

target_link_libraries(Shared
    PUBLIC
    Magnum::Magnum
    spdlog::spdlog
    tsl::hopscotch_map
)

if(WIN32)
    target_link_libraries(Shared PUBLIC ws2_32 winmm)
endif()

target_compile_features(Shared PUBLIC cxx_std_23)
