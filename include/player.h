#pragma once // player.h
#include "camera.h"
#include "renderer.h"
#include "sceneObject.h"
#include "texture.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct PlayerConstData
{
    float maxSpeed;
    float accelerationRate;
    float linearDrag;
    float maxTurnRate;
    float turnRate;
    float angularDrag;
    float cameraSmoothing;
};

struct PlayerData
{
    PlayerData() : m_position(0.0f), m_velocity(0.0f), m_angularVelocity(0.0f), m_rotation(0.0f), m_steer(0.0f), m_throttle(0.0f) {}
    glm::vec2 m_position;
    glm::vec2 m_velocity;
    float m_angularVelocity;
    float m_rotation;
    float m_steer;
    float m_throttle;
};

class Player
{
  public:
    Player();
    void update(float deltaTime);
    void handleInput(GLFWwindow *window, float deltaTime);
    void init(Renderer &renderer);

    PlayerData m_data;
    PlayerConstData m_constData;

  private:
    void updateCamera(float deltaTime);

  private:
    SceneObject *m_carSprite;
    Texture *m_carTexture;
    Camera *m_camera;
};