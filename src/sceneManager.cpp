// sceneManager.cpp
#include "sceneManager.h"

void SceneManager::addObject(SceneObject *object) { m_objects.push_back(object); }

void SceneManager::drawAll(GLuint shaderProgram) const
{
    for (auto *obj : m_objects)
        obj->draw(shaderProgram);
}