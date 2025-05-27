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
};


#endif //AABB_H
