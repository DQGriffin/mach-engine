//
// Created by Dquavius Griffin on 11/7/26.
//

#ifndef FONT_H
#define FONT_H

#include <string>
#include <vector>
#include "../../external/stb_truetype.h"
// #include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace mach::rendering::text
{
    class Font {
    public:
        Font() = default;
        bool load(const std::vector<unsigned char>& fontData)
        {
            // LOG_INFO("Loading font from memory"/**/)
            std::vector<unsigned char> bitmap(
                m_atlasWidth * m_atlasHeight, 0
            );

            stbtt_pack_context packContext{};

            if (!stbtt_PackBegin(
                &packContext,
                bitmap.data(),
                m_atlasWidth,
                m_atlasHeight,
                0,
                2,
                nullptr))
            {
                return false;
            }

            // Improve glyph quality at smaller sizes.
            stbtt_PackSetOversampling(&packContext, 2, 2);

            constexpr int firstChar = 32;
            constexpr int charCount = 95;

            bool success = stbtt_PackFontRange(
                &packContext,
                fontData.data(),
                0,
                32.0f,
                firstChar,
                charCount,
                m_glyphs
            ) != 0;

            stbtt_PackEnd(&packContext);

            if (!success)
                return false;

            // Upload bitmap to OpenGL.
            glGenTextures(1, &m_texture);
            glBindTexture(GL_TEXTURE_2D, m_texture);

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_R8,
                m_atlasWidth,
                m_atlasHeight,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                bitmap.data()
            );

            glTexParameteri(
                GL_TEXTURE_2D,
                GL_TEXTURE_MIN_FILTER,
                GL_LINEAR
            );

            glTexParameteri(
                GL_TEXTURE_2D,
                GL_TEXTURE_MAG_FILTER,
                GL_LINEAR
            );

            glTexParameteri(
                GL_TEXTURE_2D,
                GL_TEXTURE_WRAP_S,
                GL_CLAMP_TO_EDGE
            );

            glTexParameteri(
                GL_TEXTURE_2D,
                GL_TEXTURE_WRAP_T,
                GL_CLAMP_TO_EDGE
            );

            glBindTexture(GL_TEXTURE_2D, 0);

            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

            // LOG_INFO("Successfully loaded font")
            return true;
        }

        void unload();

        GLuint getTexture() const
        {
            return m_texture;
        }

        stbtt_packedchar* getGlyphs()
        {
            return m_glyphs;
        }

        int getAtlasWidth() const
        {
            return m_atlasWidth;
        }

        int getAtlasHeight() const
        {
            return m_atlasHeight;
        }

        private:
        GLuint m_texture = 0;

        int m_atlasWidth = 1024;
        int m_atlasHeight = 1024;

        stbtt_packedchar m_glyphs[96];
    };
}
#endif //FONT_H
