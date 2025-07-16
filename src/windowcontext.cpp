#include "windowcontext.h"

WindowContext::WindowContext(int width, int height, const char *title) : m_width(width), m_height(height), m_title(title) {}

WindowContext::~WindowContext()
{
    if (m_window)
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
}

GLFWwindow *WindowContext::get() { return m_window; }

void WindowContext::createWindow(int width, int height, const char *title)
{

    // initialize GLFW if needed
    if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW");

    // ask for OpenGL 3.3 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_window) throw std::runtime_error("Failed to create GLFW window");
    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
    glfwSwapInterval(1);
}