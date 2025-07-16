#pragma once // init.h

class init
{
    /*
    public:
    static void all(GLFWwindow *window, int width, int height, const char *title, GLFWframebuffersizefun callback)
    {
        GLFW();
        Window(window, width, height, title, callback);
        GLEW();
        ImGui(window);
    }
    */

  public:
    static void GLFW()
    {
        // Initialize GLFW
        if (glfwInit() != GLFW_TRUE) throw std::runtime_error("Failed to initialize GLFW\n");
        // Request OpenGL 3.3 Core context
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }

    /*
    static void Window(GLFWwindow *window, int width, int height, const char *title, GLFWframebuffersizefun callback)
    {
        window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!window) throw std::runtime_error("Failed to create window");
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, callback);
        glfwSwapInterval(1);
    }
    */

    static void GLEW()
    {
        // Initialize GLEW (load OpenGL function pointers)
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) throw std::runtime_error("Failed to initialize GLEW\n");
    }

    static void ImGui(GLFWwindow *window)
    {
        // Initialize ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) throw std::runtime_error("Failed to initialize ImGui for OpenGL\n");
        if (!ImGui_ImplOpenGL3_Init("#version 330 core")) throw std::runtime_error("Failed to initialize OpenGL version for ImGui\n");
    }
};