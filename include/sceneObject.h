#pragma once // sceneObject.h
#include "mesh.h"
#include <glm/glm.hpp>

class SceneObject
{
  public:
    SceneObject(Mesh &mesh, const glm::vec2 &worldPosisiton);
    void draw(GLuint shaderProgram) const;
    void setPosition(const glm::vec2 &worldPosition);

  private:
    Mesh m_mesh;
    glm::vec2 m_worldPos;
};