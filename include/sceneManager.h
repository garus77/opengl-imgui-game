#pragma once // sceneManager.h
#include "sceneObject.h"
#include <vector>

class SceneManager
{
  public:
    void addObject(SceneObject *object);
    void drawAll(GLuint shaderProgram) const;

  private:
    std::vector<SceneObject *> m_objects;
};