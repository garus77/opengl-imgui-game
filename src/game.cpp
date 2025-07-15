#include "game.h"
#include <fstream>
#include <sstream>

Game::Game() : m_window(nullptr)
{
    m_windowSettings.m_width = 800;
    m_windowSettings.m_height = 600;
    m_windowSettings.m_title = "A little game";
}

Game::~Game()
{
    if (m_window) shutDown();
}

int Game::run()
{
    init();
    gameLoop();
    shutDown();
    return 0;
}

void Game::init()
{
    if (glfwInit() != GLFW_TRUE) throw std::runtime_error("Failed to initialize GLFW\n");
    // Request OpenGL 3.3 Core context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_windowSettings.m_width, m_windowSettings.m_height, m_windowSettings.m_title, nullptr, nullptr);
    if (!m_window) throw std::runtime_error("Failed to create window");
    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, m_windowSettings.framebuffer_size_callback);
    glfwSwapInterval(1);

    // 3. Initialize GLEW (load OpenGL function pointers)
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) throw std::runtime_error("Failed to initialize GLEW\n");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // opengl
    setupScene();

    m_log.log("Initialization complete");
}

std::string Game::loadShaderSrc(const char *path)
{
    std::ifstream file(path);
    std::stringstream buf;
    buf << file.rdbuf();
    return buf.str();
}
GLuint Game::compileShader(GLenum type, const char *src)
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        char log[512];
        glGetShaderInfoLog(s, 512, nullptr, log);
        throw std::runtime_error(std::string("Shader compile error:\n") + log);
    }
    return s;
}

void Game::setupScene()
{
    // 1) Load sources
    auto vertexSrc = loadShaderSrc("resources/shaders/vertex.glsl");
    auto fragmentSrc = loadShaderSrc("resources/shaders/fragment.glsl");

    // 2) Compile
    GLuint vert = compileShader(GL_VERTEX_SHADER, vertexSrc.c_str());
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragmentSrc.c_str());

    // 3) Link program
    m_openglResources.m_shaderProgram = glCreateProgram();
    glAttachShader(m_openglResources.m_shaderProgram, vert);
    glAttachShader(m_openglResources.m_shaderProgram, frag);
    glLinkProgram(m_openglResources.m_shaderProgram);

    // check link status
    GLint ok;
    glGetProgramiv(m_openglResources.m_shaderProgram, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        char log[512];
        glGetProgramInfoLog(m_openglResources.m_shaderProgram, 512, nullptr, log);
        throw std::runtime_error(std::string("Program link error:\n") + log);
    }
    glDeleteShader(vert);
    glDeleteShader(frag);

    m_locColor = glGetUniformLocation(m_openglResources.m_shaderProgram, "uColor");
    m_locOffset = glGetUniformLocation(m_openglResources.m_shaderProgram, "uOffset");
    m_locScale = glGetUniformLocation(m_openglResources.m_shaderProgram, "uScale");
    m_locRotation = glGetUniformLocation(m_openglResources.m_shaderProgram, "uRotation");

    // 4) Set up a single-triangle VBO + VAO
    float vertices[] = {
        0.0f,  0.5f,  0.0f, // top
        -0.5f, -0.5f, 0.0f, // bottom left
        0.5f,  -0.5f, 0.0f  // bottom right
    };

    glGenVertexArrays(1, &m_openglResources.m_VAO);
    glGenBuffers(1, &m_openglResources.m_VBO);

    glBindVertexArray(m_openglResources.m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_openglResources.m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void Game::gameLoop()
{
    int counter = 0;
    bool flag = false;
    bool show_demo_window = false;

    char logMessage[100] = {0};

    while (!glfwWindowShouldClose(m_window))
    {
        // Poll events
        glfwPollEvents();

        if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(m_window, true);

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Example ImGui window
        ImGui::Begin("Control Panel");

        ImGui::Text("This is a GLEW + GLFW + ImGui demo");
        ImGui::Text("Button pressed %d times", counter);
        ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
        ImGui::Text("Test: %.5f miliseconds", ImGui::GetIO().DeltaTime * 1000.0f);
        ImGui::Text("Test: %s", ImGui::GetIO().IniFilename);
        ImGui::Text("Mouse: %.2f, %.2f", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
        if (ImGui::Button("Click Me"))
        {
            // std::cout << "Flag is: " << flag;
            counter++;
        }
        ImGui::Checkbox("Enable Feature", &flag);
        ImGui::Checkbox("Enable demo window", &show_demo_window);

        ImGui::BeginChild("CHILD", ImVec2(0, 0), true);
        ImGui::Text("Im a child!");
        ImGui::InputText("A message to log", logMessage, 100);
        if (ImGui::Button("Log the message"))
        {
            Logger::instance().log(logMessage);
        }
        ImGui::Text("Triangle Controls:");
        ImGui::ColorEdit3("Color", glm::value_ptr(m_triangleColor));
        ImGui::SliderFloat2("Offset", glm::value_ptr(m_triangleOffset), -1.0f, 1.0f);
        ImGui::SliderFloat("Scale", &m_triangleScale, 0.1f, 2.0f);
        ImGui::SliderAngle("Rotation", &m_triangleRotate); // degrees → ImGui handles conversion
        ImGui::EndChild();

        ImGui::End();

        if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

        // Rendering
        // 1) Clear the backbuffer
        int display_w, display_h;
        glfwGetFramebufferSize(m_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 2) Draw your OpenGL scene (triangle)
        glUseProgram(m_openglResources.m_shaderProgram);

        glUniform3fv(m_locColor, 1, glm::value_ptr(m_triangleColor));
        glUniform2fv(m_locOffset, 1, glm::value_ptr(m_triangleOffset));
        glUniform1f(m_locScale, m_triangleScale);
        glUniform1f(m_locRotation, m_triangleRotate);

        glBindVertexArray(m_openglResources.m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(m_window);
    }
}

void Game::shutDown()
{
    // ImGui shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // GL cleanup
    if (m_openglResources.m_shaderProgram) glDeleteProgram(m_openglResources.m_shaderProgram);
    if (m_openglResources.m_VBO) glDeleteBuffers(1, &m_openglResources.m_VBO);
    if (m_openglResources.m_VAO) glDeleteVertexArrays(1, &m_openglResources.m_VAO);

    // GLFW cleanup
    glfwDestroyWindow(m_window);
    glfwTerminate();
    m_window = nullptr;

    // Log
    m_log.log("Shutdown complete");
}