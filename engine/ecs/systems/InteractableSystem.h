//
// Created by Dquavius Griffin on 11/6/26.
//

#ifndef INTERACTABLESYSTEM_H
#define INTERACTABLESYSTEM_H
#include "ScriptSystem.h"
#include "../../logging/logging.h"
#include  "../Entity.h"
#include "../components/Interactable.h"
#include "../components/Transform.h"

namespace mach::ecs::systems
{
    using namespace mach::ecs;

    inline void interactNearby(entt::registry& registry, Entity* playerEntity)
    {
        auto& playerTransform = registry.get<components::Transform>(playerEntity->handle());

        auto view = registry.view<mach::ecs::components::Interactable>();
        for (auto entity : view)
        {
            auto& interactComponent = registry.get<mach::ecs::components::Interactable>(entity);
            auto& interactableTransform = registry.get<mach::ecs::components::Transform>(entity);
            auto length = glm::length(playerTransform.position - interactableTransform.position);

            if (length <= interactComponent.range * interactComponent.range)
            {
                // LOG_INFO("Interactable interaction detected");
                mach::ecs::systems::callOnInteractScript(registry, entity);
            }
        }
    }
}

#endif //INTERACTABLESYSTEM_H
