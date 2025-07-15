#pragma once // game.h
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "logger.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <iostream>

class Game
{
  public:
    Game();
    ~Game();
    int run();

  private:
    void init();
    void gameLoop();
    void shutDown();

    GLFWwindow *m_window;
    struct WindowSettings
    {
        int m_width;
        int m_height;
        const char *m_title;
        static void framebuffer_size_callback(GLFWwindow *window, int w, int h) { glViewport(0, 0, w, h); }
    } m_windowSettings;
    Logger &m_log = Logger::instance();

    // openGL
    struct OpenGLResources
    {
        GLuint m_shaderProgram = 0;
        GLuint m_VAO = 0;
        GLuint m_VBO = 0;
    } m_openglResources;
    std::string loadShaderSrc(const char *path);
    GLuint compileShader(GLenum type, const char *src);
    void setupScene(); // compiles shaders + buffers
};