file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS *.cpp)

add_executable(UnitTests ${SRC_FILES})

find_package(Catch2 REQUIRED)

target_link_libraries(UnitTests
    PRIVATE
        voxel_engine
        glfw
        glm::glm
        glad::glad
        stb::stb
        fastnoise-lite::fastnoise-lite
        imgui::imgui
        spdlog::spdlog
        Catch2::Catch2WithMain
)

target_include_directories(UnitTests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)
