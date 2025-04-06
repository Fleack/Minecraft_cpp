file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS *.cpp)

add_executable(UnitTests ${SRC_FILES})

target_include_directories(UnitTests PRIVATE
        ${glm_SOURCE_DIR}
        ${stb_SOURCE_DIR}
        ${fastnoise_SOURCE_DIR}/Cpp
)

target_link_libraries(UnitTests
        PRIVATE
        Catch2::Catch2WithMain
        voxel_engine
        spdlog::spdlog
)
