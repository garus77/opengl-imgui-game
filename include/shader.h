#pragma once // shader.h
#include <GL/glew.h>
#include <string>

class Shader
{
  public:
    static GLuint compile(GLenum type, const char *src);
    static std::string load(const char *path);
    static GLuint buildShaderProgram(GLuint vert, GLuint frag);
    static GLuint buildShaderProgram(const char *pathVert, const char *pathFrag);

  private:
};