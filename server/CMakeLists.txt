cmake_minimum_required(VERSION 3.16)

file(GLOB_RECURSE SERVER_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(Server ${SERVER_SRC})

target_compile_options(Server PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wnull-dereference
    -Wimplicit-fallthrough
)

target_include_directories(Server
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/extern/magnum/src
    ${PROJECT_SOURCE_DIR}/extern/corrade/src
)

target_link_libraries(Server
    PRIVATE
    Shared
    Magnum::Magnum  # Base module for Math (Vector3)
    concurrencpp::concurrencpp
    fastnoise-lite::fastnoise-lite
    cpptrace::cpptrace
)

target_compile_features(Server PRIVATE cxx_std_23)

