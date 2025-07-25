// sceneObject.cpp
#include "sceneObject.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

SceneObject::SceneObject(Mesh &mesh, const glm::vec2 &worldPosisiton) : m_mesh(mesh), m_worldPos(worldPosisiton) {}

void SceneObject::setPosition(const glm::vec2 &worldPosition) { m_worldPos = worldPosition; }

void SceneObject::draw(GLuint shaderProgram) const
{
    // build model matrix in X/Y plane -> Z=0
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(m_worldPos, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uModel"), 1, GL_FALSE, glm::value_ptr(model));

    m_mesh.Draw(shaderProgram);
}