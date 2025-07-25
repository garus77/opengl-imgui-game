#pragma once // shader.h
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class Shader
{
  public:
    Shader() = default;
    ~Shader();

    // non-copyable
    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;
    // movable
    Shader(Shader &&other) noexcept;
    Shader &operator=(Shader &&other) noexcept;

    // Build from vertex & fragment shader paths
    static Shader buildShaderProgram(const char *vertPath, const char *fragPath);

    void use() const;
    GLuint id() const;

    // Uniform utilities
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setFloat(const std::string &name, float value) const;
    void setVec2(const std::string &name, const glm::vec2 &vec) const;
    void setInt(const std::string &name, int value) const;

  private:
    static GLuint compileShader(GLenum type, const char *src);
    static std::string loadFile(const char *path);
    static GLuint linkProgram(GLuint vertShader, GLuint fragShader);

    explicit Shader(GLuint programID);
    GLuint m_id{0};
};