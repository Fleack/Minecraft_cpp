#include <glad/gl.h>

#include <catch2/catch_all.hpp>
#include <GLFW/glfw3.h>

#include "render/ChunkMesh.hpp"

using namespace mc::render;

TEST_CASE("ChunkMesh uploads and draws vertex data", "[Render][ChunkMesh]")
{
    REQUIRE(glfwInit());
    GLFWwindow* window = glfwCreateWindow(800, 600, "MeshTest", nullptr, nullptr);
    REQUIRE(window);
    glfwMakeContextCurrent(window);
    REQUIRE(gladLoadGL(glfwGetProcAddress));

    ChunkMesh mesh;
    mesh.setChunkPosition({2, 0, 3});
    REQUIRE(mesh.getChunkPosition() == glm::ivec3(2, 0, 3));

    std::vector<Vertex> vertices {
        {{0, 0, 0}, {0, 1, 0}, {0, 0}},
        {{1, 0, 0}, {0, 1, 0}, {1, 0}},
        {{0, 1, 0}, {0, 1, 0}, {0, 1}}
    };

    mesh.upload(vertices);
    REQUIRE_NOTHROW(mesh.draw());

    glfwDestroyWindow(window);
    glfwTerminate();
}
