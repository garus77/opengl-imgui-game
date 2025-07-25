#pragma once // texture.h

#include <GL/glew.h>
#include <string>

class Texture
{
  public:
    // Loads the texture from 'path'. Throws on failure.
    Texture(const std::string &path, bool flipVertically = true);
    Texture() = default;

    // Cleans up the GPU resource.
    ~Texture();

    /// Bind to the given texture unit (0,1,2...)
    void Bind(GLuint unit = 0) const;

    // Unbinds any texture from GL_TEXTURE_2D
    static void Unbind();

    // Returns the raw OpenGL texture handle
    GLuint GetID() const { return m_id; }

  private:
    GLuint m_id = 0;
    int m_width = 0, m_height = 0, m_channels = 0;
};
