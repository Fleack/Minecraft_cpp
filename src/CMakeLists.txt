file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS *.cpp)
list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

add_library(voxel_engine STATIC ${SRC_FILES})

target_include_directories(voxel_engine
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(voxel_engine
    PRIVATE
        glfw
        glm::glm
        glad::glad
        stb::stb
        fastnoise-lite::fastnoise-lite
        imgui::imgui
        spdlog::spdlog
)

add_executable(Minecraft main.cpp)
target_link_libraries(Minecraft PRIVATE voxel_engine)