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

    using Vertices = std::vector<Vertex>;
    using Indicies = std::vector<unsigned>;

    const float tiling = 100000.0f;
    const float halfW = 50.0f * tiling;
    const float halfH = 50.0f * tiling;
    Vertices rectVerts = {
        {{-halfW, -halfH, 0.0f}, {0.0f, 0.0f}},                 //
        {{halfW, -halfH, 0.0f}, {1.0f * tiling, 0.0f}},         //
        {{halfW, halfH, 0.0f}, {1.0f * tiling, 1.0f * tiling}}, //
        {{-halfW, halfH, 0.0f}, {0.0f, 1.0f * tiling}}          //
    };
    Indicies rectInds = {
        0, 1, 2, //
        0, 2, 3  //
    };
    Mesh *quadMesh = new Mesh(rectVerts, rectInds, *m_brickTex);

    const float width = 50.0f;
    const float height = 50.0f;
    Vertices testVerts = {
        {{-width, -height, 0.0f}, {0.0f, 0.0f}}, //
        {{width, -height, 0.0f}, {1.0f, 0.0f}},  //
        {{0.0f, 0.0f, 0.0f}, {0.5f, 0.5f}},      //
        {{width, height, 0.0f}, {1.0f, 1.0f}},   //
        {{-width, height, 0.0f}, {0.0f, 1.0f}},  //
    };
    Indicies testInds = {
        0, 1, 2, //
        2, 3, 4  //
    };
    Mesh *testMesh = new Mesh(testVerts, testInds, *m_brickTex);

    /*const float halfLen = 40.0f;
    const float halfWidth = 30.0f;
    const float halfTipWidth = 20.0f;
    const float tipLen = 20.0f;
    Vertices carVerts = {
        {{-halfLen, -halfWidth, 0.0f}, {0.25f, 0.0f}},            // 0
        {{halfLen, -halfWidth, 0.0f}, {0.75f, 0.0f}},             // 1
        {{halfLen, halfWidth, 0.0f}, {0.75f, 1.0f}},              // 2
        {{-halfLen, halfWidth, 0.0f}, {0.25f, 1.0f}},             // 3
        {{-halfLen - tipLen, halfTipWidth, 0.0f}, {0.0f, 1.0f}},  // 4
        {{-halfLen - tipLen, -halfTipWidth, 0.0f}, {0.0f, 0.0f}}, // 5
        {{halfLen + tipLen, -halfTipWidth, 0.0f}, {1.0f, 0.0f}},  // 6
        {{halfLen + tipLen, halfTipWidth, 0.0f}, {1.0f, 1.0f}},   // 7
    };
    Indicies carInds = {
        0, 1, 2, //
        0, 2, 3, //
        5, 0, 3, //
        5, 3, 4, //
        1, 6, 7, //
        1, 7, 2, //
    };
    m_carTex = new Texture("resources/textures/car_tex.png");
    Mesh *carMesh = new Mesh(carVerts, carInds, *m_carTex);*/

    SceneObject *obj1 = new SceneObject(*quadMesh, glm::vec2(100.0f, 100.0f), glm::vec2(1.0f), 0.0f); // brick tile
    SceneObject *obj2 = new SceneObject(*testMesh, glm::vec2(100.0f, 0.0f), glm::vec2(1.0f), 0.0f);   // hourglass
    // SceneObject *obj3 = new SceneObject(*carMesh, glm::vec2(0.0f, 0.0f), glm::vec2(1.0f), 0.0f);      // car

    // m_testObject = obj3;
    // tmprotation = m_testObject->getRotation();

    // SceneObject *obj3 = new SceneObject(*quadMesh, glm::vec2(-50.0f, 50.0f));

    /*
    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < 100; j++)
        {
            SceneObject *object = new SceneObject(*quadMesh, glm::vec2(i * 100.0f, j * 100.0f), glm::vec2(1.0f), 0.0f);
            m_renderer.getScene().addObject(object);
        }
    }
    */

    m_renderer.getScene().addObject(obj2);
    m_renderer.getScene().addObject(obj1);
    //    m_renderer.getScene().addObject(m_testObject);
    // m_renderer.getScene().addObject(obj3);

    m_player.init(m_renderer);

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

    float &camZoom = m_renderer.getCamera().getZoom();
    float *zoom = &m_renderer.getCamera().getZoom();
    glm::vec2 &camPos = m_renderer.getCamera().getPos();
    float *pos = glm::value_ptr(m_renderer.getCamera().getPos());

    glm::vec2 speed{0.0f, 0.0f};

    float deltaTime;
    const auto &data = m_player.m_data;

    while (!glfwWindowShouldClose(m_window))
    {
        // Poll events
        glfwPollEvents();

        deltaTime = ImGui::GetIO().DeltaTime;
        m_player.handleInput(m_window);
        m_player.update(deltaTime);

        TabDown = (glfwGetKey(m_window, GLFW_KEY_TAB) == GLFW_PRESS);
        if (TabDown && !TabWasDown) showControlPanel = !showControlPanel;
        TabWasDown = TabDown;

        /*speed = {0.0f, 0.0f};
        if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS) speed.x += 100.0f;
        if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS) speed.x += -100.0f;
        if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS) speed.y += 100.0f;
        if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS) speed.y += -100.0f;

        camPos += speed * deltaTime;*/

        // m_testObject->setPosition(camPos);

        if (glfwGetKey(m_window, GLFW_KEY_Z) == GLFW_PRESS) camZoom *= 1.0f + 1.0f * deltaTime;
        if (glfwGetKey(m_window, GLFW_KEY_X) == GLFW_PRESS) camZoom /= 1.0f + 1.0f * deltaTime;

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
            ImGui::Text("mSPF: %.5f miliseconds", ImGui::GetIO().DeltaTime * 1000.0f);
            ImGui::Text("Runtime: %.2f", ImGui::GetTime());
            ImGui::Text("Mouse: %.2f, %.2f", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
            if (ImGui::Button("Click Me"))
            {
                // std::cout << "Flag is: " << flag;
                counter++;
            }
            ImGui::Checkbox("Enable demo window", &show_demo_window);

            ImGui::BeginChild("alldata", ImVec2(0, 0), true);

            ImGui::Text("Camera data:");
            ImGui::SliderFloat("Camera zoom", zoom, 0.1f, 5.0f);
            ImGui::SliderFloat2("Camera position", pos, -400.0f, 400.0f);

            ImGui::Text("Car params:");
            ImGui::SliderFloat("Max Speed", &m_player.m_constData.maxSpeed, -1000.0f, 1000.0f);
            ImGui::SliderFloat("Acceleration rate", &m_player.m_constData.accelerationRate, -1000.0f, 1000.0f);
            ImGui::SliderFloat("Max turn rate", &m_player.m_constData.maxTurnRate, -1000.0f, 1000.0f);
            ImGui::SliderFloat("Turn rate", &m_player.m_constData.turnRate, -1000.0f, 1000.0f);
            ImGui::SliderFloat("Angular drag", &m_player.m_constData.angularDrag, 0.0f, 3.0f);
            ImGui::SliderFloat("Linear drag", &m_player.m_constData.linearDrag, 0.0f, 3.0f);

            ImGui::BeginChild("cardata", ImVec2(0, 0), true);
            ImGui::Text("Car data:");
            ImGui::Text("Steering: %.1f, Throttle: %.1f", data.m_steer, data.m_throttle);
            ImGui::Text("Rotation: %.2f, Angular velocity: %.2f", data.m_rotation, data.m_angularVelocity);
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