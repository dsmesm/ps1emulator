//
// Created by Dima on 10/15/2020.
//

#ifndef PS1EMULATOR_RENDERER_H
#define PS1EMULATOR_RENDERER_H


#define GLEW_STATIC
#include <GL/glew.h>

#undef GLFW_DLL
#include <GLFW/glfw3.h>

struct Renderer {
    static GLFWwindow* window;

    static GLuint vertexArrayObject;
    static GLuint vertexBufferObject;

    static GLuint vertexShader;
    static GLuint fragmentShader;
    static GLuint shaderProgram;

    static int verticesCount;

    static unsigned int vertBuffer[];

    static void init();
    static void draw();
};


#endif //PS1EMULATOR_RENDERER_H
