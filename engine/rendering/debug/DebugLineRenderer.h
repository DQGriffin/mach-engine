//
// Created by Dquavius Griffin on 5/27/25.
//

#ifndef DEBUGLINERENDERER_H
#define DEBUGLINERENDERER_H

#include <vector>
#include <glm/glm.hpp>
#include "../shader/Shader.h"

namespace mach::renderer
{
    struct DebugLine {
        glm::vec3 start;
        glm::vec3 end;
        glm::vec3 color;
    };

    class DebugLineRenderer {
    private:
        std::vector<DebugLine> lines;
        Shader* shader;

        unsigned int vao = 0;
        unsigned int vbo = 0;

    public:
        DebugLineRenderer(Shader* r);
        ~DebugLineRenderer();
        void init();
        void shutdown();

        void addLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color);
        void clear();
        void render(const glm::mat4& viewProj);
    };
}

#endif //DEBUGLINERENDERER_H
