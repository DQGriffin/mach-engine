//
// Created by Dquavius Griffin on 5/26/25.
//

#ifndef TRANSFORM_H
#define TRANSFORM_H
#include <glm/glm.hpp>

namespace mach::ecs::components
{
    struct Transform
    {
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
    };
}

#endif //TRANSFORM_H
