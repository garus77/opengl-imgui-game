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
    // glfwSwapInterval(1);

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
    m_raceTrackTex = new Texture("resources/textures/race_track.png");

    using Vertices = std::vector<Vertex>;
    using Indicies = std::vector<unsigned>;

    const float tiling = 100000.0f;
    const float halfW = 512.0f * 5.0f;
    const float halfH = 512.0f * 5.0f;
    Vertices rectVerts = {
        {{-halfW, -halfH, 0.0f}, {0.0f, 0.0f}}, //
        {{halfW, -halfH, 0.0f}, {1.0f, 0.0f}},  //
        {{halfW, halfH, 0.0f}, {1.0f, 1.0f}},   //
        {{-halfW, halfH, 0.0f}, {0.0f, 1.0f}}   //
    };
    Indicies rectInds = {
        0, 1, 2, //
        0, 2, 3  //
    };
    Mesh *quadMesh = new Mesh(rectVerts, rectInds, *m_raceTrackTex);

    SceneObject *obj1 = new SceneObject(*quadMesh, glm::vec2(100.0f, 100.0f), glm::vec2(1.0f), 0.0f);
    m_renderer.getScene().addObject(obj1);

    m_player.init(m_renderer);
}

void Game::gameLoop()
{
    int counter = 0;
    bool show_demo_window = false;
    bool showControlPanel = true;
    bool TabDown = false;
    bool TabWasDown = false;

    float &camZoom = m_renderer.getCamera().getZoom();
    float *zoom = &m_renderer.getCamera().getZoom();
    float *pos = glm::value_ptr(m_renderer.getCamera().getPos());

    float deltaTime;
    const auto &data = m_player.m_data;

    while (!glfwWindowShouldClose(m_window))
    {
        // Poll events
        glfwPollEvents();

        deltaTime = ImGui::GetIO().DeltaTime;
        m_player.handleInput(m_window, deltaTime);
        m_player.update(deltaTime);

        TabDown = (glfwGetKey(m_window, GLFW_KEY_TAB) == GLFW_PRESS);
        if (TabDown && !TabWasDown) showControlPanel = !showControlPanel;
        TabWasDown = TabDown;

        if (glfwGetKey(m_window, GLFW_KEY_Z) == GLFW_PRESS) camZoom *= 1.0f + 1.0f * deltaTime;
        if (glfwGetKey(m_window, GLFW_KEY_X) == GLFW_PRESS) camZoom /= 1.0f + 1.0f * deltaTime;

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Control panel
        if (showControlPanel)
        {
            ImGui::Begin("Control Panel (Tab to toggle)");

            ImGui::Text("This is a GLEW + GLFW + ImGui demo");
            ImGui::Text("Button pressed %d times", counter);
            ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
            ImGui::Text("mSPF: %.5f miliseconds", ImGui::GetIO().DeltaTime * 1000.0f);
            ImGui::Text("Runtime: %.2f", ImGui::GetTime());
            ImGui::Text("Mouse: %.2f, %.2f", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
            if (ImGui::Button("Click Me"))
            {
                counter++;
            }
            ImGui::Checkbox("Enable demo window", &show_demo_window);

            ImGui::BeginChild("alldata", ImVec2(0, 0), true);

            ImGui::Text("Camera data:");
            ImGui::SliderFloat("Camera zoom", zoom, 0.1f, 5.0f);
            ImGui::SliderFloat2("Camera position", pos, -400.0f, 400.0f);
            ImGui::SliderFloat("Camera smoothing", &m_player.m_constData.cameraSmoothing, 0.0f, 30.0f);

            ImGui::Text("Car params:");
            ImGui::SliderFloat("Max Speed", &m_player.m_constData.maxSpeed, 10.0f, 1000.0f);
            ImGui::SliderFloat("Acceleration rate", &m_player.m_constData.accelerationRate, 10.0f, 1000.0f);
            ImGui::SliderFloat("Max turn rate", &m_player.m_constData.maxTurnRate, -1000.0f, 1000.0f);
            ImGui::SliderFloat("Turn rate", &m_player.m_constData.turnRate, -1000.0f, 1000.0f);
            ImGui::SliderFloat("Angular drag", &m_player.m_constData.angularDrag, 0.0f, 3.0f);
            ImGui::SliderFloat("Linear drag", &m_player.m_constData.linearDrag, 0.0f, 3.0f);

            ImGui::BeginChild("cardata", ImVec2(0, 0), true);
            ImGui::Text("Car data:");
            ImGui::Text("Steering: %.2f, Throttle: %.2f", data.m_steer, data.m_throttle);
            ImGui::Text("Angular velocity: %.2f, Rotation: %.2f", data.m_angularVelocity, data.m_rotation);
            ImGui::Text("Position: %.2f, %.2f", data.m_position.x, data.m_position.y);
            ImGui::Text("Velocity: %.2f, %.2f", data.m_velocity.x, data.m_velocity.y);
            ImGui::EndChild();

            ImGui::EndChild();

            ImGui::End();
        }

        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::Begin("##fps", nullptr,
                     ImGuiWindowFlags_NoDecoration           //
                         | ImGuiWindowFlags_AlwaysAutoResize //
                         | ImGuiWindowFlags_NoInputs         //
                         | ImGuiWindowFlags_NoBackground     //
        );
        ImGui::Text("FPS: %.0f", ImGui::GetIO().Framerate);
        ImGui::End();

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