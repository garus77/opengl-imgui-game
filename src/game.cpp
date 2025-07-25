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
    glfwSetWindowUserPointer(m_window, &m_renderer);
    glfwSetFramebufferSizeCallback(m_window,
                                   [](GLFWwindow *win, int w, int h)
                                   {
                                       // first the GL viewport
                                       glViewport(0, 0, w, h);
                                       // now tell our Renderer
                                       Renderer *R = static_cast<Renderer *>(glfwGetWindowUserPointer(win));
                                       if (R) R->onResize(w, h);
                                   });
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
    m_brickTex = new Texture("resources/textures/brick_x32.png");
    float halfW = 50.0f;
    float halfH = 50.0f;
    std::vector<Vertex> verts = {
        {{-halfW, -halfH, 0.0f}, {0.0f, 0.0f}}, //
        {{halfW, -halfH, 0.0f}, {1.0f, 0.0f}},  //
        {{halfW, halfH, 0.0f}, {1.0f, 1.0f}},   //
        {{-halfW, halfH, 0.0f}, {0.0f, 1.0f}},  //
    };
    std::vector<unsigned> inds = {0, 1, 2, 0, 2, 3};

    Mesh *quadMesh = new Mesh(verts, inds, *m_brickTex);

    SceneObject *obj1 = new SceneObject(*quadMesh, glm::vec2(0.0f, 0.0f));
    // SceneObject *obj2 = new SceneObject(*quadMesh, glm::vec2(50.0f, 50.0f));
    // SceneObject *obj3 = new SceneObject(*quadMesh, glm::vec2(-50.0f, 50.0f));

    m_renderer.getScene().addObject(obj1);
    // m_renderer.getScene().addObject(obj2);
    // m_renderer.getScene().addObject(obj3);

    /*
    const float radius = 50.0f;
    const int segments = 128;
    std::vector<Vertex> circleVerts;
    circleVerts.reserve(segments + 2);
    circleVerts.push_back({
        {0.0f, 0.0f, 0.0f}, {0.5f, 0.5f} // UV center of a white 1×1 texture or tile
    });
    for (int i = 0; i <= segments; ++i)
    {
        float theta = 2.0f * M_PI * float(i) / float(segments);
        float x = cosf(theta) * radius;
        float y = sinf(theta) * radius;
        // Map the circle into the [0,1]×[0,1] UV space if you have a texture
        float u = 0.5f + 0.5f * cosf(theta);
        float v = 0.5f + 0.5f * sinf(theta);
        circleVerts.push_back({{x, y, 0.0f}, {u, v}});
    }
    std::vector<unsigned> circleIdx;
    circleIdx.reserve(segments * 3);
    for (int i = 1; i <= segments; ++i)
    {
        circleIdx.push_back(0);     // center
        circleIdx.push_back(i);     // current edge vertex
        circleIdx.push_back(i + 1); // next edge vertex
    }
    Mesh *circleMesh = new Mesh(circleVerts, circleIdx, *m_brickTex);

    // 4) Instance it as a SceneObject
    SceneObject *circleObj = new SceneObject(*circleMesh, glm::vec2(100.0f, 100.0f));
    m_renderer.getScene().addObject(circleObj);
    */
}

void Game::gameLoop()
{
    int counter = 0;
    bool show_demo_window = false;
    bool showControlPanel = true;
    bool TabDown = false;
    bool TabWasDown = false;

    float *zoom = &m_renderer.getCamera().getZoom();
    float *pos = glm::value_ptr(m_renderer.getCamera().getPos());

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
            ImGui::SliderFloat("Camera zoom", zoom, 0.1f, 5.0f);
            ImGui::SliderFloat2("Camera position", pos, -400.0f, 400.0f);
            ImGui::EndChild();

            ImGui::End();
        }

        // m_renderer.getCamera().setZoom(zoom);

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

    // GLFW cleanup
    glfwDestroyWindow(m_window);
    glfwTerminate();
    m_window = nullptr;
}