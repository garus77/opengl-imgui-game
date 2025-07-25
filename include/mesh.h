#pragma once // mesh.h
#include "texture.h"
#include <glm/glm.hpp>
#include <vector>

struct Vertex
{
    glm::vec3 m_relPosition;
    glm::vec2 m_texCoords; // uv
};

class Mesh
{
  public:
    Mesh(const std::vector<Vertex> &verts, const std::vector<unsigned> &idx, Texture &texture);
    ~Mesh();
    void Draw(GLuint shaderProgram) const;

  private:
    GLuint m_vao, m_vbo, m_ebo;
    GLsizei m_indexCount;
    Texture &m_texture;
};