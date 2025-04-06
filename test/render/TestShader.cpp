#include <glad/gl.h>

#include "render/Shader.hpp"

#include <fstream>

#include <catch2/catch_all.hpp>
#include <GLFW/glfw3.h>

using namespace mc::render;

namespace
{
void writeTestShader(const std::string& path, const std::string& code)
{
    std::ofstream out(path);
    out << code;
}
}

TEST_CASE("Shader loads, compiles and links correctly", "[Render][Shader]")
{
    REQUIRE(glfwInit());

    GLFWwindow* window = glfwCreateWindow(800, 600, "ShaderTest", nullptr, nullptr);
    REQUIRE(window != nullptr);
    glfwMakeContextCurrent(window);
    REQUIRE(gladLoadGL(glfwGetProcAddress));

    std::string vertexPath = "test_shader.vert";
    std::string fragmentPath = "test_shader.frag";

    writeTestShader(vertexPath, R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos, 1.0);
        }
    )");

    writeTestShader(fragmentPath, R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0);
        }
    )");

    Shader shader(vertexPath, fragmentPath);

    shader.bind();
    GLint currentProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    REQUIRE(static_cast<GLuint>(currentProgram) == shader.getID());

    shader.unbind();
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    REQUIRE(currentProgram == 0);

    std::remove(vertexPath.c_str());
    std::remove(fragmentPath.c_str());

    glfwDestroyWindow(window);
    glfwTerminate();
}
