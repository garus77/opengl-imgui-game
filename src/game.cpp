// game.cpp
#include "game.h"
#include "shader.h"
#include "texture.h"
#include <fstream>
#include <sstream>

Game::Game() : m_window(nullptr)
{
    m_windowSettings.m_width = 800;
    m_windowSettings.m_height = 800;
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

    // Initialize GLFW
    if (glfwInit() != GLFW_TRUE) throw std::runtime_error("Failed to initialize GLFW\n");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Initialize window
    m_window = glfwCreateWindow(m_windowSettings.m_width, m_windowSettings.m_height, m_windowSettings.m_title, nullptr, nullptr);
    if (!m_window) throw std::runtime_error("Failed to create window");
    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, m_windowSettings.framebuffer_size_callback);
    glfwSwapInterval(1);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) throw std::runtime_error("Failed to initialize GLEW\n");

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    if (!ImGui_ImplGlfw_InitForOpenGL(m_window, true)) throw std::runtime_error("Failed to initialize ImGui for OpenGL\n");
    if (!ImGui_ImplOpenGL3_Init("#version 330 core")) throw std::runtime_error("Failed to initialize OpenGL version for ImGui\n");

    setupScene();
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
    m_openglResources.m_shaderProgram = Shader::buildShaderProgram("resources/shaders/vertex.glsl", "resources/shaders/fragment.glsl");

    // m_locColor = glGetUniformLocation(m_openglResources.m_shaderProgram, "uColor");
    m_locOffset = glGetUniformLocation(m_openglResources.m_shaderProgram, "uOffset");
    m_locScale = glGetUniformLocation(m_openglResources.m_shaderProgram, "uScale");
    m_locRotation = glGetUniformLocation(m_openglResources.m_shaderProgram, "uRotation");

    // 4) Set up a single-triangle VBO + VAO
    float tiling = 15.0f;
    float vertices[] = {
        //  x      y     z    u     v
        -0.5f, -0.5f, 0.0f, 0.0f,          0.0f,          //
        0.5f,  -0.5f, 0.0f, 1.0f * tiling, 0.0f,          //
        -0.5f, 0.5f,  0.0f, 0.0f,          1.0f * tiling, //
        0.5f,  0.5f,  0.0f, 1.0f * tiling, 1.0f * tiling, //
    };

    glGenVertexArrays(1, &m_openglResources.m_VAO);
    glGenBuffers(1, &m_openglResources.m_VBO);

    glBindVertexArray(m_openglResources.m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_openglResources.m_VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Game::gameLoop()
{
    int counter = 0;
    bool show_demo_window = false;
    bool showControlPanel = true;
    bool TabDown = false;
    bool TabWasDown = false;
    bool followMouse = false;

    Texture testTexture("resources/textures/brick_x32.png");

    while (!glfwWindowShouldClose(m_window))
    {
        // Poll events
        glfwPollEvents();

        TabDown = (glfwGetKey(m_window, GLFW_KEY_TAB) == GLFW_PRESS);
        if (TabDown && !TabWasDown) showControlPanel = !showControlPanel;
        TabWasDown = TabDown;

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Example ImGui window
        if (showControlPanel)
        {
            ImGui::Begin("Control Panel (Tab to toggle)");

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
            ImGui::Checkbox("Enable mouse following on click", &followMouse);
            ImGui::Checkbox("Enable demo window", &show_demo_window);

            ImGui::BeginChild("CHILD", ImVec2(0, 0), true);
            ImGui::Text("Im a child!");
            ImGui::Text("Triangle Controls:");
            // ImGui::ColorEdit3("Color", glm::value_ptr(m_triangleColor));
            ImGui::SliderFloat2("Offset", glm::value_ptr(m_triangleOffset), -1.0f, 1.0f, "%.3f");
            ImGui::SliderFloat("Scale", &m_triangleScale, 0.1f, 2.0f);
            ImGui::SliderAngle("Rotation (degrees)", &m_triangleRotate, -180.0f, 180.0f, "%.0f deg"); // degrees → ImGui handles conversion
            ImGui::DragFloat("Rotation (radians)", &m_triangleRotate, 0.01f, -M_PI, M_PI, "%.3f", ImGuiSliderFlags_WrapAround);
            ImGui::EndChild();

            ImGui::End();
        }

        if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

        // Rendering
        // 1) Clear the backbuffer
        int display_w, display_h;
        glfwGetFramebufferSize(m_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (followMouse)
        {
            if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            {
                double mx, my;
                glfwGetCursorPos(m_window, &mx, &my);
                m_triangleOffset.x = float((mx / display_w) * 2.0 - 1.0);
                m_triangleOffset.y = float(1.0 - (my / display_h) * 2.0);
            }
        }

        // 2) Draw your OpenGL scene (triangle)
        glUseProgram(m_openglResources.m_shaderProgram);
        testTexture.Bind(0);
        glUniform1i(glGetUniformLocation(m_openglResources.m_shaderProgram, "uTexture"), 0);

        // glUniform3fv(m_locColor, 1, glm::value_ptr(m_triangleColor));
        glUniform2fv(m_locOffset, 1, glm::value_ptr(m_triangleOffset));
        glUniform1f(m_locScale, m_triangleScale);
        glUniform1f(m_locRotation, m_triangleRotate);

        // Bind your texture to unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, testTexture.GetID());

        glBindVertexArray(m_openglResources.m_VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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
}