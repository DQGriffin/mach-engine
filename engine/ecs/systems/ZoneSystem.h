//
// Created by Dquavius Griffin on 6/11/25.
//

#ifndef ZONESYSTEM_H
#define ZONESYSTEM_H
#include <iostream>
#include "../../external/entt.h"
#include "../../logging/logging.h"
#include "../../rendering/debug/DebugLineRenderer.h"
#include "../components/Transform.h"
#include "../components/Zone.h"
#include "../components/ZoneTrigger.h"

namespace mach::ecs::systems
{
    using namespace mach::ecs;
    bool isWithinZone(mach::ecs::components::Zone& zone, mach::ecs::components::Transform entityTransform);

    inline void updateZones(entt::registry& registry)
    {
        auto view = registry.view<mach::ecs::components::Zone>();
        auto transformableEntities = registry.view<mach::ecs::components::Transform, mach::ecs::components::ZoneTrigger>();

        for (auto& zoneEntity : view)
        {
            auto& zone = view.get<mach::ecs::components::Zone>(zoneEntity);
            if (!zone.isActive)
            {
                return;
            }

            for (auto& entity : transformableEntities)
            {
                auto& transform = transformableEntities.get<mach::ecs::components::Transform>(entity);
                bool isWithin = isWithinZone(zone, transform);

                // todo: invoke lua script here (onEnter, onExit, onStay)
            }
        }
    }

    inline void drawZones(entt::registry& registry, mach::renderer::DebugLineRenderer* lineRenderer)
    {
        if (lineRenderer == nullptr)
        {
            return;
        }

        auto view = registry.view<mach::ecs::components::Zone>();
        for (auto& zoneEntity : view)
        {
            auto& zoneComponent = view.get<mach::ecs::components::Zone>(zoneEntity);
            auto corners = zoneComponent.bounds.getCorners();

            glm::vec3 color;
            if (zoneComponent.isActive)
            {
                color = glm::vec3(1.0f, 0.0f, 0.0f);
            }
            else
            {
                color = glm::vec3(0.5f, 0.5f, 0.5f);
            }

            lineRenderer->addLine(corners[0], corners[1], color);
            lineRenderer->addLine(corners[1], corners[2], color);
            lineRenderer->addLine(corners[2], corners[3], color);
            lineRenderer->addLine(corners[3], corners[0], color);

            lineRenderer->addLine(corners[4], corners[5], color);
            lineRenderer->addLine(corners[5], corners[6], color);
            lineRenderer->addLine(corners[6], corners[7], color);
            lineRenderer->addLine(corners[7], corners[4], color);

            lineRenderer->addLine(corners[0], corners[4], color);
            lineRenderer->addLine(corners[1], corners[5], color);
            lineRenderer->addLine(corners[2], corners[6], color);
            lineRenderer->addLine(corners[3], corners[7], color);
        }
    }

    inline bool isWithinZone(mach::ecs::components::Zone& zone, mach::ecs::components::Transform entityTransform)
    {
        return entityTransform.position.x >= zone.bounds.min.x && entityTransform.position.x <= zone.bounds.max.x &&
            entityTransform.position.y >= zone.bounds.min.y && entityTransform.position.y <= zone.bounds.max.y &&
            entityTransform.position.z >= zone.bounds.min.z && entityTransform.position.z <= zone.bounds.max.z;
    }
}

#endif //ZONESYSTEM_H
