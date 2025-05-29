//
// Created by Dquavius Griffin on 5/28/25.
//

#ifndef ANCHOR_H
#define ANCHOR_H
#include "../../external/entt.h"
#include "glm/detail/type_vec.hpp"
#include <glm/glm.hpp>

namespace mach::ecs::components
{
    struct Anchor
    {
        mach::ecs::Entity* entity;
        glm::vec3 offset;
        bool x;
        bool y;
        bool z;
    };
}

#endif //ANCHOR_H
