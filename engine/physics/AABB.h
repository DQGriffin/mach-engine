//
// Created by Dquavius Griffin on 5/27/25.
//

#ifndef AABB_H
#define AABB_H

#include <glm/glm.hpp>

struct AABB {
    glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());

    void expandToFit(const glm::vec3& point) {
        min = glm::min(min, point);
        max = glm::max(max, point);
    }

    glm::vec3 center() const { return (min + max) * 0.5f; }
    glm::vec3 halfExtents() const { return (max - min) * 0.5f; }
    std::array<glm::vec3, 8> getCorners() const {
        glm::vec3 c = center();
        glm::vec3 e = halfExtents();

        return {
            c + glm::vec3(-e.x, -e.y, -e.z), // 0
            c + glm::vec3(+e.x, -e.y, -e.z), // 1
            c + glm::vec3(+e.x, +e.y, -e.z), // 2
            c + glm::vec3(-e.x, +e.y, -e.z), // 3
            c + glm::vec3(-e.x, -e.y, +e.z), // 4
            c + glm::vec3(+e.x, -e.y, +e.z), // 5
            c + glm::vec3(+e.x, +e.y, +e.z), // 6
            c + glm::vec3(-e.x, +e.y, +e.z)  // 7
        };
    }
};


#endif //AABB_H
