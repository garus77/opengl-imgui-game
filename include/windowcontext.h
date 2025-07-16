#pragma once // windowcontext.h
#include <GLFW/glfw3.h>
#include <iostream>

class WindowContext
{
  public:
    WindowContext() = default;
    WindowContext(int width, int height, const char *title);
    ~WindowContext();
    void createWindow(int, int, const char *); // void createWindow(int width, int height, const char *title); // {}
    GLFWwindow *get();

  private:
    static void framebuffer_size_callback(GLFWwindow *window, int w, int h) { glViewport(0, 0, w, h); }

  private:
    GLFWwindow *m_window = nullptr;
    int m_width;
    int m_height;
    const char *m_title;
};