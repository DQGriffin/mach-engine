//
// Created by Dquavius Griffin on 11/8/26.
//

#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H
#include <vector>
#include <string>

#include "TextVertex.h"
#include "../../math/vec.h"
#include "../../external/stb_truetype.h"
#include "../shader/Shader.h"
#include "../text/Font.h"

class TextRenderer {
public:
    explicit TextRenderer(Shader& shader);
    ~TextRenderer();
    void init();
    std::vector<TextVertex> generateVertices(mach::vec2 position, mach::rendering::text::Font& font, std::string text);
    void drawText(mach::rendering::text::Font& font, std::string text, mach::vec2 position, mach::vec4 color);
    void checkGLError(const char* location);
private:
    GLuint m_vao;
    GLuint m_vbo;
    Shader& m_shader;
};

#endif //TEXTRENDERER_H
