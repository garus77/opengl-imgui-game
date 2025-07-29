// sceneObject.cpp
#include "sceneObject.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

SceneObject::SceneObject(Mesh &mesh, const glm::vec2 &worldPosisiton, const glm::vec2 &scale, float rotation) : m_mesh(mesh), m_worldPos(worldPosisiton), m_scale(scale), m_rotation(rotation) {}

void SceneObject::setPosition(const glm::vec2 &worldPosition) { m_worldPos = worldPosition; }
void SceneObject::setScale(const glm::vec2 &scale) { m_scale = scale; }
void SceneObject::setRotation(float rotation) { m_rotation = rotation; }

void SceneObject::draw(GLuint shaderProgram) const
{
    // build model matrix in X/Y plane -> Z=0
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(m_worldPos, 0.0f));
    if (m_rotation) model = glm::rotate(model, glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    if (m_scale.x || m_scale.y) model = glm::scale(model, glm::vec3(m_scale, 1.0f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uModel"), 1, GL_FALSE, glm::value_ptr(model));

    m_mesh.Draw(shaderProgram);
}