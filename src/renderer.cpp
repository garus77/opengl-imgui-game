// renderer.cpp
#include "renderer.h"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>

Renderer::Renderer(int width, int height) : m_width(width), m_height(height), m_camera((float)width, (float)height), m_window(nullptr) {}

SceneManager &Renderer::getScene() { return m_scene; }

Camera &Renderer::getCamera() { return m_camera; }

void Renderer::init(GLFWwindow *window)
{
    m_window = window;
    // GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) throw std::runtime_error("GLEW init failed");
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build shader
    m_shader = Shader::buildShaderProgram("resources/shaders/vertex.glsl", "resources/shaders/fragment.glsl");
    m_shader.use();

    // pass projection & view once
    glm::mat4 proj = m_camera.getProjectionMatrix();
    glm::mat4 view = m_camera.getViewMatrix();
    m_shader.setMat4("uProjection", proj);
    m_shader.setMat4("uView", view);
}

void Renderer::renderFrame()
{
    glViewport(0, 0, m_width, m_height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_shader.use();
    // if camera moved/zoomed, update:
    m_shader.setMat4("uProjection", m_camera.getProjectionMatrix());
    m_shader.setMat4("uView", m_camera.getViewMatrix());

    // draw all objects
    m_scene.drawAll(m_shader.id());
}

void Renderer::cleanup()
{
    // shader clean
    glDeleteProgram(m_shader.id());
}
