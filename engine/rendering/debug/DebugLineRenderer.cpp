//
// Created by Dquavius Griffin on 5/27/25.
//

#include "DebugLineRenderer.h"

#include "../../logging/logging.h"

mach::renderer::DebugLineRenderer::DebugLineRenderer(Shader* shader)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    this->shader = shader;
}

mach::renderer::DebugLineRenderer::~DebugLineRenderer()
{
}

void mach::renderer::DebugLineRenderer::init()
{
}

void mach::renderer::DebugLineRenderer::shutdown()
{
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void mach::renderer::DebugLineRenderer::addLine(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color)
{
    lines.push_back({start, end, color});
}

void mach::renderer::DebugLineRenderer::clear()
{
    lines.clear();
}

void mach::renderer::DebugLineRenderer::render(const glm::mat4 &viewProj)
{
    if (lines.empty()) return;

    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
    };

    std::vector<Vertex> verts;
    verts.reserve(lines.size() * 2);
    for (const auto& line : lines) {
        verts.push_back({line.start, line.color});
        verts.push_back({line.end, line.color});
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1); // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

    shader->use();
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "uViewProj"), 1, GL_FALSE, &viewProj[0][0]);

    glLineWidth(5.0f);
    glDrawArrays(GL_LINES, 0, verts.size());

    glBindVertexArray(0);
    glUseProgram(0);
}
