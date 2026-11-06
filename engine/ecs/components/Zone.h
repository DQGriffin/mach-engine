//
// Created by Dquavius Griffin on 6/11/25.
//

#ifndef ZONE_H
#define ZONE_H

#include <vector>

#include "../../external/entt.h"
#include "../../physics/AABB.h"

namespace mach::ecs::components
{
    struct Zone
    {
        AABB bounds;
        bool triggerOnce = false;
        bool isActive = true;
        std::vector<entt::entity*> containingEntities;
    };
}

#endif //ZONE_H
