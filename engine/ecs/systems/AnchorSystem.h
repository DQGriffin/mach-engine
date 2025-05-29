//
// Created by Dquavius Griffin on 5/28/25.
//

#ifndef ANCHORSYSTEM_H
#define ANCHORSYSTEM_H
#include "../../logging/logging.h"
#include "../components/Anchor.h"
#include "../components/Transform.h"
#include <iostream>

namespace mach::ecs::systems
{
    using namespace mach::ecs;
    inline void updateAnchorPositions(entt::registry& registry)
    {
        auto view = registry.view<mach::ecs::components::Anchor, components::Transform>();

        for (auto& entity : view)
        {
            auto& anchor = view.get<mach::ecs::components::Anchor>(entity);
            auto& transform = view.get<components::Transform>(entity);

            if (anchor.entity == nullptr)
            {
                LOG_INFO(std::format("Entity cannot be anchored because its anchor entity is null"));
            }

            auto& parentTransform = view.get<components::Transform>(anchor.entity->handle());

            if (anchor.x)
                transform.position.x = parentTransform.position.x + anchor.offset.x;
            if (anchor.y)
                transform.position.y = parentTransform.position.y + anchor.offset.y;
            if (anchor.z)
                transform.position.z = parentTransform.position.z + anchor.offset.z;

            if (registry.any_of<components::RigidBody>(entity)) {
                auto& rb = registry.get<mach::ecs::components::RigidBody>(entity);
                if (rb.body) {
                    btTransform btTrans;
                    btTrans.setIdentity();
                    btTrans.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
                    rb.body->setWorldTransform(btTrans);
                    rb.body->getMotionState()->setWorldTransform(btTrans);
                }
            }
        }
    }
}

#endif //ANCHORSYSTEM_H
