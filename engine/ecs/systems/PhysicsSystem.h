//
// Created by Dquavius Griffin on 5/27/25.
//

#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../../external/entt.h"
#include "../../logging/logging.h"
#include "../components/Transform.h"
#include "../components/RigidBody.h"

namespace mach::physics
{
    using namespace mach::ecs::components;

    inline void createPhysicsBody(entt::registry& registry, entt::entity entity, btDiscreteDynamicsWorld* world, renderer::Model model, float mass) {
        assert(world != nullptr);
        assert(registry.all_of<Transform>(entity));

        auto& transform = registry.get<Transform>(entity);
        glm::vec3 pos = transform.position;
        glm::vec3 rot = transform.rotation;

        // Bullet transform (world transform of the body)
        btTransform bodyTransform;
        bodyTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
        btQuaternion quat;
        quat.setEulerZYX(rot.z, rot.y, rot.x);
        bodyTransform.setRotation(quat);

        // === Build shape from model bounds ===
        const AABB& bounds = model.bounds;

        std::cout << "Center x: " << bounds.center().x << std::endl;
        std::cout << "Center y: " << bounds.center().y << std::endl;
        std::cout << "Center z: " << bounds.center().z << std::endl;

        std::cout << "half extents x: " << bounds.halfExtents().x << std::endl;
        std::cout << "half extents y: " << bounds.halfExtents().y << std::endl;
        std::cout << "half extents z: " << bounds.halfExtents().z << std::endl;

        // Bullet wants half-extents
        btVector3 halfExtents(bounds.halfExtents().x,
                              bounds.halfExtents().y,
                              bounds.halfExtents().z);

        btBoxShape* boxShape = new btBoxShape(halfExtents);

        // Local offset to center the collider to match the mesh
        btTransform localTransform;
        localTransform.setIdentity();
        localTransform.setOrigin(btVector3(bounds.center().x,
                                           bounds.center().y,
                                           bounds.center().z));

        // Wrap it in a compound shape to support offset
        btCompoundShape* compoundShape = new btCompoundShape();
        compoundShape->addChildShape(localTransform, boxShape);

        // Calculate inertia only if it's dynamic
        float actualMass = mass;
        btVector3 inertia(0, 0, 0);
        if (actualMass > 0.0f) {
            compoundShape->calculateLocalInertia(actualMass, inertia);
        }

        btDefaultMotionState* motionState = new btDefaultMotionState(bodyTransform);
        btRigidBody::btRigidBodyConstructionInfo info(actualMass, motionState, compoundShape, inertia);
        btRigidBody* body = new btRigidBody(info);

        world->addRigidBody(body);

        registry.emplace<RigidBody>(entity, body, compoundShape, motionState);
        LOG_INFO("Created Physics body");
    }

}

namespace mach::ecs::systems
{
    using namespace mach::ecs::components;

    inline void syncTransforms(entt::registry& registry) {
        auto view = registry.view<Transform, RigidBody>();

        for (auto entity : view) {
            auto& transform = view.get<Transform>(entity);
            auto& phys = view.get<RigidBody>(entity);

            if (!phys.body) continue;

            btMotionState* motionState = phys.body->getMotionState();
            if (!motionState) continue;

            btTransform trans;
            phys.body->getMotionState()->getWorldTransform(trans);
            btVector3 pos = trans.getOrigin();
            btQuaternion rot = trans.getRotation();

            transform.position = glm::vec3(pos.x(), pos.y(), pos.z());
            btVector3 euler;
            rot.getEulerZYX(euler[2], euler[1], euler[0]); // ZYX = yaw, pitch, roll

            // Assign to your rotation vector (assuming XYZ ordering)
            transform.rotation = glm::vec3(euler.x(), euler.y(), euler.z());
        }
    }

}

#endif //PHYSICSSYSTEM_H
