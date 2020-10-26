//
// Created by Dima on 10/15/2020.
//

#include <string>
#include <vector>
#include "Renderer.h"
GLFWwindow* Renderer::window = nullptr;
GLuint Renderer::vertexArrayObject = 0;
GLuint Renderer::vertexBufferObject = 0;
GLuint Renderer::vertexShader = 0;
GLuint Renderer::fragmentShader = 0;
GLuint Renderer::shaderProgram = 0;
int Renderer::verticesCount = 0;

unsigned int Renderer::vertBuffer[1024*128] = {};

void Renderer::init() {
    glfwInit();

    window = glfwCreateWindow(1024, 512, "emulator", nullptr, nullptr);

    glfwMakeContextCurrent(window);

    glewInit();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);

    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    const char* vertShader = "#version 420 core\n"
                             "\n"
                             "layout (location = 0) in uvec2 vertex_position;\n"
                             "layout (location = 1) in uvec3 vertex_color;\n"
                             "\n"
                             "out vec3 color;\n"
                             "\n"
                             "void main()\n"
                             "{\n"
                             "    float xpos = (float(vertex_position.x) / 512) - 1.0;\n"
                             "    float ypos = 1.0 - (float(vertex_position.y) / 256);\n"
                             "\n"
                             "    gl_Position = vec4(xpos, ypos, 0.0, 1.0);\n"
                             "\n"
                             "    color = vec3(\n"
                             "        float(vertex_color.x) / 255,\n"
                             "        float(vertex_color.y) / 255,\n"
                             "        float(vertex_color.z) / 255\n"
                             "    );\n"
                             "}";

    glShaderSource(vertexShader, 1, &vertShader, nullptr);
    glCompileShader(vertexShader);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char* fragShader = "#version 420 core\n"
                  "\n"
                  "in vec3 color;\n"
                  "out vec4 FragColor;\n"
                  "\n"
                  "void main()\n"
                  "{\n"
                  "    FragColor = vec4(color, 1.0);\n"
                  "}";

    glShaderSource(fragmentShader, 1, &fragShader, nullptr);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glUseProgram(shaderProgram);

    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

    glVertexAttribIPointer(0, 2, GL_UNSIGNED_INT, 5 * sizeof(unsigned int), reinterpret_cast<const void *>(0));
    glEnableVertexAttribArray(0);

    glVertexAttribIPointer(1, 3, GL_UNSIGNED_INT, 5 * sizeof(unsigned int), reinterpret_cast<const void *>(2 * sizeof(unsigned int)));
    glEnableVertexAttribArray(1);
}

void Renderer::draw() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertBuffer), vertBuffer, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, verticesCount);

    glfwSwapBuffers(window);
}
