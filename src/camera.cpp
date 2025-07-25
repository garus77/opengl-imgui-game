// camera.cpp
#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float width, float height) : m_width(width), m_height(height), m_position(0.0f, 0.0f), m_zoom(1.0f) {}

void Camera::setPosition(glm::vec2 newPosition) { m_position = newPosition; }

void Camera::setZoom(float newZoom) { m_zoom = newZoom; }

glm::mat4 Camera::getViewMatrix() const
{
    // translate world by -camera position
    return glm::translate(glm::mat4(1.0f), glm::vec3(-m_position, 0.0f));
}

glm::mat4 Camera::getProjectionMatrix() const
{
    // orthographic: world units to NDC
    float hw = (m_width * 0.5f) / m_zoom;
    float hh = (m_height * 0.5f) / m_zoom;
    return glm::ortho(-hw, hw, -hh, hh, -1.0f, 1.0f);
}