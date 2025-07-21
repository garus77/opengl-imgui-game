// shader.cpp
#include "shader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::string Shader::load(const char *path)
{
    std::ifstream file(path);
    std::stringstream buf;
    buf << file.rdbuf();
    return buf.str();
}

GLuint Shader::compile(GLenum type, const char *src)
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        char log[512];
        glGetShaderInfoLog(s, 512, nullptr, log);
        throw std::runtime_error(std::string("Shader compile error:\n") + log);
    }
    return s;
}

GLuint Shader::buildShaderProgram(GLuint vert, GLuint frag)
{
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vert);
    glAttachShader(shaderProgram, frag);

    glLinkProgram(shaderProgram);
    GLint ok;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        char log[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, log);
        throw std::runtime_error(std::string("Program link error:\n") + log);
    }
    return shaderProgram;
}

GLuint Shader::buildShaderProgram(const char *pathVert, const char *pathFrag)
{
    std::string vertexSrc = load(pathVert);
    std::string fragmentSrc = load(pathFrag);
    GLuint vert = compile(GL_VERTEX_SHADER, vertexSrc.c_str());
    GLuint frag = compile(GL_FRAGMENT_SHADER, fragmentSrc.c_str());
    GLuint shaderProgram = buildShaderProgram(vert, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);
    return shaderProgram;
}