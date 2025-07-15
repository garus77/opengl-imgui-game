#include "game.h"

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

    // 3. Initialize GLEW (load OpenGL function pointers)
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) throw std::runtime_error("Failed to initialize GLEW\n");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    m_log.log("Initialization complete");
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
        ImGui::EndChild();

        ImGui::End();

        if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(m_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(m_window);
    }
}

void Game::shutDown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(m_window);
    glfwTerminate();
    m_window = nullptr;
    m_log.log("Shutdown complete");
}