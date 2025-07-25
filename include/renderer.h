#pragma once // renderer.h
#include "camera.h"
#include "sceneManager.h"
#include "shader.h"
#include <GLFW/glfw3.h>

class Renderer
{
  public:
    Renderer(int width, int height);

    // init GL contexts, shaders, camera, scene
    void init(GLFWwindow *window);
    void renderFrame();
    void cleanup();
    void onResize(int width, int height);

    SceneManager &getScene();
    Camera &getCamera();

  private:
    int m_width, m_height;
    GLFWwindow *m_window;
    Shader m_shader;
    Camera m_camera;
    SceneManager m_scene;
};