#pragma once // sceneObject.h
#include "mesh.h"
#include <glm/glm.hpp>

class SceneObject
{
  public:
    SceneObject(Mesh &mesh, const glm::vec2 &worldPosisiton, const glm::vec2 &scale, float rotation);
    void draw(GLuint shaderProgram) const;
    void setPosition(const glm::vec2 &worldPosition);
    void setScale(const glm::vec2 &scale);
    void setRotation(float rotation);

  private:
    Mesh m_mesh;
    glm::vec2 m_worldPos;
    glm::vec2 m_scale;
    float m_rotation;
};