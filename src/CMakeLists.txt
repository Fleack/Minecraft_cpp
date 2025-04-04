file(GLOB_RECURSE SRC_FILES *.cpp)

add_executable(Minecraft ${SRC_FILES})

# OpenGL
target_link_libraries(Minecraft PRIVATE opengl32)

# GLAD
set(GLAD_SOURCE_DIR "${LIBS_SOURCE_DIR}/glad")
add_library(glad_lib STATIC ${GLAD_SOURCE_DIR}/src/gl.c)
target_include_directories(glad_lib PUBLIC ${GLAD_SOURCE_DIR}/include)
target_link_libraries(Minecraft PRIVATE glad_lib)

# GLFW
target_link_libraries(Minecraft PRIVATE glfw)

# GLM, STB, FastNoiseLite (header-only)
message("noise = ${fastnoise_SOURCE_DIR}")
target_include_directories(Minecraft PRIVATE
        ${glm_SOURCE_DIR}
        ${stb_SOURCE_DIR}
        ${fastnoise_SOURCE_DIR}/Cpp
)

# ImGui
file(GLOB IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/*.cpp
)
add_library(imgui_lib STATIC ${IMGUI_SOURCES})
target_include_directories(imgui_lib PUBLIC ${imgui_SOURCE_DIR})
target_link_libraries(Minecraft PRIVATE imgui_lib)

# spdlog
target_link_libraries(Minecraft PRIVATE spdlog::spdlog)
