//
// Created by Dquavius Griffin on 5/26/25.
//

#ifndef REGISTRY_H
#define REGISTRY_H

#include "../../external/entt.h"

namespace mach::ecs
{
    class Registry
    {
    private:
        entt::registry registry;

    public:
        Registry() = default;
        ~Registry() = default;
        entt::entity createEntity();
        void destroyEntity(entt::entity entity);

    };
}



#endif //REGISTRY_H
