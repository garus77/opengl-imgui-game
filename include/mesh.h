#pragma once // mesh.h
#include "texture.h"
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 Position;
    glm::vec2 TexCoords;
};

class Mesh
{
  public:
  private:
    Texture &texture;
};