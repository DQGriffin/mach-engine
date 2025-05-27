//
// Created by Dquavius Griffin on 5/26/25.
//

#include "Registry.h"

entt::entity mach::ecs::Registry::createEntity()
{
    entt::entity e = registry.create();
    return e;
}

void mach::ecs::Registry::destroyEntity(entt::entity entity)
{
    registry.destroy(entity);
}
