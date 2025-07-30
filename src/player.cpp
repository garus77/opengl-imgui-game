// player.cpp
#include "player.h"
#include <algorithm>
#include <vector>

Player::Player() : m_carTexture(nullptr), m_carSprite(nullptr), m_camera(nullptr) {}

void Player::init(Renderer &renderer)
{
    const float halfLen = 40.0f;
    const float halfWidth = 30.0f;
    const float halfTipWidth = 20.0f;
    const float tipLen = 20.0f;

    std::vector<Vertex> carVertices = {
        {{-halfLen, -halfWidth, 0.0f}, {0.25f, 0.0f}},            // 0
        {{halfLen, -halfWidth, 0.0f}, {0.75f, 0.0f}},             // 1
        {{halfLen, halfWidth, 0.0f}, {0.75f, 1.0f}},              // 2
        {{-halfLen, halfWidth, 0.0f}, {0.25f, 1.0f}},             // 3
        {{-halfLen - tipLen, halfTipWidth, 0.0f}, {0.0f, 1.0f}},  // 4
        {{-halfLen - tipLen, -halfTipWidth, 0.0f}, {0.0f, 0.0f}}, // 5
        {{halfLen + tipLen, -halfTipWidth, 0.0f}, {1.0f, 0.0f}},  // 6
        {{halfLen + tipLen, halfTipWidth, 0.0f}, {1.0f, 1.0f}},   // 7
    };
    std::vector<unsigned> carIndicies = {
        0, 1, 2, //
        0, 2, 3, //
        5, 0, 3, //
        5, 3, 4, //
        1, 6, 7, //
        1, 7, 2, //
    };
    m_carTexture = new Texture("resources/textures/car_tex.png");
    Mesh *carMesh = new Mesh(carVertices, carIndicies, *m_carTexture);
    m_carSprite = new SceneObject(*carMesh, m_data.m_position, glm::vec2(1.0f), m_data.m_rotation);
    renderer.getScene().addObject(m_carSprite);

    m_camera = &renderer.getCamera();

    m_constData.accelerationRate = 1000.0f;
    m_constData.angularDrag = 2.0f;
    m_constData.linearDrag = 1.5f;
    m_constData.maxSpeed = 1000.0f;
    m_constData.maxTurnRate = 100.0f;
    m_constData.turnRate = 250.0f;
    m_constData.cameraSmoothing = 10.0f;
}

void Player::handleInput(GLFWwindow *window, float deltaTime)
{
    int wKey = glfwGetKey(window, GLFW_KEY_W);
    int sKey = glfwGetKey(window, GLFW_KEY_S);
    if (!(wKey == GLFW_PRESS) && !(sKey == GLFW_PRESS)) m_data.m_throttle = 0.0f;
    if (wKey == GLFW_PRESS) m_data.m_throttle += 1.0f * deltaTime;
    if (sKey == GLFW_PRESS) m_data.m_throttle -= 0.5f * deltaTime;
    m_data.m_throttle = std::clamp(m_data.m_throttle, -1.0f, 1.0f);

    int aKey = glfwGetKey(window, GLFW_KEY_A);
    int dKey = glfwGetKey(window, GLFW_KEY_D);
    if (!(aKey == GLFW_PRESS) && !(dKey == GLFW_PRESS)) m_data.m_steer = 0.0f;
    if (aKey == GLFW_PRESS) m_data.m_steer += 3.0f * deltaTime;
    if (dKey == GLFW_PRESS) m_data.m_steer -= 3.0f * deltaTime;
    m_data.m_steer = std::clamp(m_data.m_steer, -1.0f, 1.0f);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {

        m_constData.accelerationRate = 500.0f;
        m_constData.linearDrag = 0.5f;
    }
}

void Player::update(float deltaTime)
{
    m_data.m_angularVelocity += m_data.m_steer * m_constData.turnRate * deltaTime;
    m_data.m_angularVelocity = std::clamp(m_data.m_angularVelocity, -m_constData.maxTurnRate, m_constData.maxTurnRate);
    if (abs(m_data.m_angularVelocity) > 10.0f)
    {
        m_data.m_angularVelocity /= (1.0f + m_constData.angularDrag * deltaTime);
    }
    else if (m_data.m_steer == 0.0f)
    {
        m_data.m_angularVelocity = 0.0f;
    }

    m_data.m_rotation += m_data.m_angularVelocity * deltaTime;
    if (m_data.m_rotation > 360.0f) m_data.m_rotation -= 360.0f;
    if (m_data.m_rotation < -360.0f) m_data.m_rotation += 360.0f;

    float rot = glm::radians(m_data.m_rotation);
    glm::vec2 forward{std::cos(rot), std::sin(rot)};
    m_data.m_velocity += forward * (m_data.m_throttle * m_constData.accelerationRate * deltaTime);
    float speed = glm::length(m_data.m_velocity);
    if (speed > m_constData.maxSpeed) m_data.m_velocity -= forward * (m_data.m_throttle * m_constData.accelerationRate * deltaTime);

    if (speed > 10.0f)
    {
        m_data.m_velocity /= (1.0f + m_constData.linearDrag * deltaTime);
    }
    else if (m_data.m_throttle == 0.0f)
    {
        m_data.m_velocity = glm::vec2(0.0f);
    }
    m_data.m_position += m_data.m_velocity * deltaTime;

    if (m_carSprite)
    {
        m_carSprite->setPosition(m_data.m_position);
        m_carSprite->setRotation(m_data.m_rotation);
    }
    if (m_camera) updateCamera(deltaTime);
}

void Player::updateCamera(float deltaTime)
{
    glm::vec2 camPos = m_camera->getPos();
    float alpha = std::clamp(m_constData.cameraSmoothing * deltaTime, 0.0f, 1.0f);
    camPos += (m_data.m_position - camPos) * alpha;

    m_camera->setPosition(camPos);
}
