cmake_minimum_required(VERSION 3.16)

include(cmake/InstallAssets.cmake)

file(GLOB_RECURSE CLIENT_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(Client ${CLIENT_SRC})

target_compile_options(Client PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Wnull-dereference
        -Wimplicit-fallthrough
)

target_include_directories(Client
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/extern/magnum/src
        ${PROJECT_SOURCE_DIR}/extern/corrade/src
        ${PROJECT_SOURCE_DIR}/extern/magnum-plugins/src
        ${PROJECT_SOURCE_DIR}/extern/magnum-extras/src
)

target_link_libraries(Client
        PRIVATE
        Shared
        # Magnum
        Corrade::Main
        Magnum::Magnum
        Magnum::GL
        Magnum::Shaders
        Magnum::Sdl2Application
        Magnum::SceneGraph
        Magnum::MeshTools
        Magnum::Trade
        Magnum::AnyImageImporter
        MagnumExtras::Ui
        MagnumPlugins::StbTrueTypeFont
        MagnumPlugins::StbImageImporter
        # Other libs
        dbghelp
        concurrencpp::concurrencpp
        cpptrace::cpptrace
        fastnoise-lite::fastnoise-lite
)

target_compile_features(Client PRIVATE cxx_std_23)

