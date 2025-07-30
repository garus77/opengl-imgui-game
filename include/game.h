#pragma once // game.h
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "player.h"
#include "renderer.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <iostream>

class Game
{
  public:
    Game();
    ~Game();
    int run();

  private:
    void init();
    void gameLoop();
    void shutDown();
    void setupScene();
    GLFWwindow *m_window;
    struct WindowSettings
    {
        int m_width;
        int m_height;
        const char *m_title;
    } m_windowSettings;

    Renderer m_renderer;
    Texture *m_brickTex;
    Texture *m_raceTrackTex;
    Player m_player;
};