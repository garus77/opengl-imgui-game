// shader.cpp
#include "shader.h"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>
#include <stdexcept>

Shader::Shader(Shader &&other) noexcept
{
    m_id = other.m_id;
    other.m_id = 0;
}

Shader &Shader::operator=(Shader &&other) noexcept
{
    if (this != &other)
    {
        if (m_id) glDeleteProgram(m_id);
        m_id = other.m_id;
        other.m_id = 0;
    }
    return *this;
}

Shader::~Shader()
{
    if (m_id) glDeleteProgram(m_id);
}

Shader::Shader(GLuint programID) : m_id(programID) {}

Shader Shader::buildShaderProgram(const char *vertPath, const char *fragPath)
{
    // Load sources
    std::string vertSrc = loadFile(vertPath);
    std::string fragSrc = loadFile(fragPath);

    // Compile
    GLuint vertShader = compileShader(GL_VERTEX_SHADER, vertSrc.c_str());
    GLuint fragShader = compileShader(GL_FRAGMENT_SHADER, fragSrc.c_str());

    // Link
    GLuint prog = linkProgram(vertShader, fragShader);
    return Shader(prog);
}

void Shader::use() const { glUseProgram(m_id); }

GLuint Shader::id() const { return m_id; }

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    GLint loc = glGetUniformLocation(m_id, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setFloat(const std::string &name, float value) const
{
    GLint loc = glGetUniformLocation(m_id, name.c_str());
    glUniform1f(loc, value);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &vec) const
{
    GLint loc = glGetUniformLocation(m_id, name.c_str());
    glUniform2f(loc, vec.x, vec.y);
}

void Shader::setInt(const std::string &name, int value) const
{
    GLint loc = glGetUniformLocation(m_id, name.c_str());
    glUniform1i(loc, value);
}

// Private helpers
GLuint Shader::compileShader(GLenum type, const char *src)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLint len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, ' ');
        glGetShaderInfoLog(shader, len, nullptr, &log[0]);
        std::string typeName = (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
        glDeleteShader(shader);
        throw std::runtime_error("Shader compilation failed (" + typeName + "):\n" + log);
    }
    return shader;
}

std::string Shader::loadFile(const char *path)
{
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file) throw std::runtime_error(std::string("Failed to open file: ") + path);
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint Shader::linkProgram(GLuint vertShader, GLuint fragShader)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLint len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, ' ');
        glGetProgramInfoLog(program, len, nullptr, &log[0]);
        glDeleteProgram(program);
        throw std::runtime_error("Program linking failed:\n" + log);
    }

    // Cleanup shaders
    glDetachShader(program, vertShader);
    glDetachShader(program, fragShader);
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
}
