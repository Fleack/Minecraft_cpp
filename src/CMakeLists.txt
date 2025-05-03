file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS *.cpp)
list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

add_library(voxel_engine STATIC ${SRC_FILES})

target_include_directories(voxel_engine
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${SDL2_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/externals/magnum/src
        ${PROJECT_SOURCE_DIR}/externals/corrade/src
)

target_link_libraries(voxel_engine
        PUBLIC
        Magnum::Magnum
        Magnum::GL
        Magnum::Shaders
        Magnum::Sdl2Application
        Magnum::SceneGraph
        Magnum::Primitives       # <- for Primitives::cubeSolid()
        Magnum::MeshTools        # <- for MeshTools::compile()
        Magnum::Trade
        Corrade::Main
        dbghelp
        fastnoise-lite::fastnoise-lite
        imgui::imgui
        spdlog::spdlog
        concurrencpp::concurrencpp
        cpptrace::cpptrace
)

target_compile_features(voxel_engine PRIVATE cxx_std_23)

add_executable(Minecraft main.cpp)
target_link_libraries(Minecraft PRIVATE voxel_engine)
