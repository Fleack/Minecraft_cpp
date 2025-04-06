file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS *.cpp)

add_executable(UnitTests ${SRC_FILES})

target_include_directories(UnitTests PRIVATE
        ${glm_SOURCE_DIR}
        ${stb_SOURCE_DIR}
        ${fastnoise_SOURCE_DIR}/Cpp
)

set(GLAD_SOURCE_DIR "${LIBS_SOURCE_DIR}/glad")
target_include_directories(glad_lib PUBLIC ${GLAD_SOURCE_DIR}/include)

target_link_libraries(UnitTests
        PRIVATE
        voxel_engine
        Catch2::Catch2WithMain
        spdlog::spdlog
        glfw
        glad_lib
)
