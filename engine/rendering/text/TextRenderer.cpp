//
// Created by Dquavius Griffin on 11/8/26.
//

#include "TextRenderer.h"

#include "glm/gtc/matrix_transform.hpp"

TextRenderer::TextRenderer(Shader& shader): m_shader(shader)
{
    init();
}

TextRenderer::~TextRenderer()
{

}

void TextRenderer::init()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(TextVertex) * 6 * 1024,
        nullptr,
        GL_DYNAMIC_DRAW
    );

    // Position: attribute 0
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(TextVertex),
        (void*)offsetof(TextVertex, position)
    );

    glEnableVertexAttribArray(0);

    // Texture coordinates: attribute 1
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(TextVertex),
        (void*)offsetof(TextVertex, texCoord)
    );

    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

std::vector<TextVertex> TextRenderer::generateVertices(mach::vec2 position, mach::rendering::text::Font& font, std::string text)
{
    float x = position.x;
    float y = position.y;
    std::vector<TextVertex> vertices;

    for (char c : text)
    {
        if (c < 32 || c > 126)
            continue;

        stbtt_aligned_quad quad{};

        stbtt_GetPackedQuad(
            font.getGlyphs(),
            font.getAtlasWidth(),
            font.getAtlasHeight(),
            c - 32,
            &x,
            &y,
            &quad,
            1
        );

        // ...
        TextVertex v0 = {
            {quad.x0, quad.y0},
            {quad.s0, quad.t0}
        };

        TextVertex v1 = {
            {quad.x1, quad.y0},
            {quad.s1, quad.t0}
        };

        TextVertex v2 = {
            {quad.x0, quad.y1},
            {quad.s0, quad.t1}
        };

        TextVertex v3 = {
            {quad.x1, quad.y1},
            {quad.s1, quad.t1}
        };

        vertices.insert(vertices.end(), {
            v0, v1, v2,
            v2, v1, v3
        });
    }

    return vertices;
}

void TextRenderer::drawText(mach::rendering::text::Font& font, std::string text, mach::vec2 position,
    mach::vec4 color)
{
    std::vector<TextVertex> vertices;

    float x = position.x;
    float y = position.y;

    for (char c : text)
    {
        if (c < 32 || c > 126)
            continue;

        stbtt_aligned_quad quad{};

        stbtt_GetPackedQuad(
            font.getGlyphs(),
            font.getAtlasWidth(),
            font.getAtlasHeight(),
            c - 32,
            &x,
            &y,
            &quad,
            1
        );

        TextVertex v0 = {
            {quad.x0, quad.y0},
            {quad.s0, quad.t0}
        };

        TextVertex v1 = {
            {quad.x1, quad.y0},
            {quad.s1, quad.t0}
        };

        TextVertex v2 = {
            {quad.x0, quad.y1},
            {quad.s0, quad.t1}
        };

        TextVertex v3 = {
            {quad.x1, quad.y1},
            {quad.s1, quad.t1}
        };

        vertices.insert(vertices.end(), {
            v0, v1, v2,
            v2, v1, v3
        });
    }

    if (vertices.empty())
        return;

    // Bind shader
    m_shader.use();

    m_shader.setVec4("textColor", color);
    m_shader.setInt("fontAtlas", 0);

    glm::mat4 projection = glm::ortho(
        0.0f,
        static_cast<float>(1200),
        static_cast<float>(800),
        0.0f
    );

    m_shader.setMat4("projection", projection);

    glEnable(GL_BLEND);

    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA
    );

    // Bind font texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font.getTexture());

    // Upload vertices
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        vertices.size() * sizeof(TextVertex),
        vertices.data()
    );

    GLint bufferSize = 0;

    glGetBufferParameteriv(
        GL_ARRAY_BUFFER,
        GL_BUFFER_SIZE,
        &bufferSize
    );

    glDisable(GL_DEPTH_TEST);

    // Draw
    glDrawArrays(
        GL_TRIANGLES,
        0,
        static_cast<GLsizei>(vertices.size())
    );

    glEnable(GL_DEPTH_TEST);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::checkGLError(const char* location)
{
    GLenum error;

    while ((error = glGetError()) != GL_NO_ERROR)
    {
        std::cerr
            << "[OpenGL Error] "
            << location
            << ": ";

        switch (error)
        {
            case GL_INVALID_ENUM:
                std::cerr << "GL_INVALID_ENUM";
                break;

            case GL_INVALID_VALUE:
                std::cerr << "GL_INVALID_VALUE";
                break;

            case GL_INVALID_OPERATION:
                std::cerr << "GL_INVALID_OPERATION";
                break;

            case GL_OUT_OF_MEMORY:
                std::cerr << "GL_OUT_OF_MEMORY";
                break;

            case GL_INVALID_FRAMEBUFFER_OPERATION:
                std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;

            default:
                std::cerr << "Unknown error (" << error << ")";
                break;
        }

        std::cerr << " [" << error << "]" << std::endl;
    }
}