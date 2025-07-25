#pragma once // camera.h
#include <glm/glm.hpp>

class Camera
{
  public:
    Camera(float width, float height);
    void setPosition(glm::vec2 newPosition);
    void setSize(float newWidth, float newHeight);
    void setZoom(float newZoom);
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;

    float &getZoom();
    glm::vec2 &getPos();

  private:
    glm::vec2 m_position;
    float m_zoom;
    float m_width, m_height;
};