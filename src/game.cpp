// game.cpp
#include "game.h"
#include "shader.h"
#include "texture.h"
#include <fstream>
#include <sstream>

Game::Game() : m_window(nullptr), m_renderer(800, 800)
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

    m_renderer.init(m_window);

    setupScene();
}

void Game::setupScene()
{
    Texture brickTex("resources/textures/brick_x32.png");
    std::vector<Vertex> verts = {
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}}, //
        {{0.5f, -0.5f, 0.0f}, {15.0f, 0.0f}}, //
        {{0.5f, 0.5f, 0.0f}, {15.0f, 15.0f}}, //
        {{-0.5f, 0.5f, 0.0f}, {0.0f, 15.0f}}, //
    };
    std::vector<unsigned> idx = {0, 1, 2, 0, 2, 3};

    Mesh *quadMesh = new Mesh(verts, idx, brickTex);

    SceneObject *obj1 = new SceneObject(*quadMesh, glm::vec2(-0.5f, -0.5f));
    SceneObject *obj2 = new SceneObject(*quadMesh, glm::vec2(0.5f, 0.5f));

    m_renderer.getScene().addObject(obj1);
    m_renderer.getScene().addObject(obj2);
}

void Game::gameLoop()
{
    int counter = 0;
    bool show_demo_window = false;
    bool showControlPanel = true;
    bool TabDown = false;
    bool TabWasDown = false;

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
            ImGui::Checkbox("Enable demo window", &show_demo_window);

            ImGui::BeginChild("CHILD", ImVec2(0, 0), true);
            ImGui::Text("Im a child!");
            ImGui::EndChild();

            ImGui::End();
        }

        if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

        m_renderer.renderFrame();
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

    m_renderer.cleanup();

    /*
    // GL cleanup
    if (m_openglResources.m_shaderProgram) glDeleteProgram(m_openglResources.m_shaderProgram);
    if (m_openglResources.m_VBO) glDeleteBuffers(1, &m_openglResources.m_VBO);
    if (m_openglResources.m_VAO) glDeleteVertexArrays(1, &m_openglResources.m_VAO);
    */

    // GLFW cleanup
    glfwDestroyWindow(m_window);
    glfwTerminate();
    m_window = nullptr;
}