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
#include "../components/CapsuleCollider.h"
#include "../components/MeshCollider.h"
#include "../components/Transform.h"
#include "../components/RigidBody.h"

namespace mach::physics
{
    using namespace mach::ecs::components;
    void createCapsuleCollider(entt::registry& registry, entt::entity entity, btDiscreteDynamicsWorld* world, renderer::Model model, float mass);

    inline void createPhysicsBody(entt::registry& registry, entt::entity entity, btDiscreteDynamicsWorld* world, renderer::Model model, bool useCapsuleCollider, float mass) {
        assert(world != nullptr);
        assert(registry.all_of<Transform>(entity));

        if (useCapsuleCollider)
        {
            createCapsuleCollider(registry, entity, world, model, mass);
            return;
        }

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

        registry.emplace<RigidBody>(entity, body, compoundShape, motionState, world);
        LOG_INFO("Created Physics body");
    }

    inline void createCapsuleCollider(entt::registry& registry, entt::entity entity, btDiscreteDynamicsWorld* world, renderer::Model model, float mass) {
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

        // Bullet wants half-extents
        btVector3 halfExtents(bounds.halfExtents().x,
                              bounds.halfExtents().y,
                              bounds.halfExtents().z);

        btCapsuleShape* shape = new btCapsuleShape(1.0f, 1.0f);

        // Local offset to center the collider to match the mesh
        btTransform localTransform;
        localTransform.setIdentity();
        localTransform.setOrigin(btVector3(bounds.center().x,
                                           bounds.center().y,
                                           bounds.center().z));

        // Calculate inertia only if it's dynamic
        float actualMass = mass;
        btVector3 inertia(0, 0, 0);
        if (actualMass > 0.0f) {
            shape->calculateLocalInertia(actualMass, inertia);
        }

        btDefaultMotionState* motionState = new btDefaultMotionState(bodyTransform);
        btRigidBody::btRigidBodyConstructionInfo info(actualMass, motionState, shape, inertia);
        btRigidBody* body = new btRigidBody(info);
        body->setAngularFactor(btVector3(0, 0, 0));


        world->addRigidBody(body);

        mach::ecs::components::RigidBody rb;
        rb.body = body;
        rb.shape = shape;
        rb.world = world;
        rb.motionState = motionState;

        registry.emplace<RigidBody>(entity, rb);
        LOG_INFO("Created Physics body");
    }

    inline void createMeshCollider(entt::registry& registry, entt::entity entity, btDiscreteDynamicsWorld* world, const std::vector<glm::vec3>& verts, float mass)
    {
        LOG_INFO("Creating mesh collider");
        auto& entityTransform = registry.get<Transform>(entity);
        btCollisionShape* shape = nullptr;

        if (mass == 0.0f) {
            // Create static mesh collider
            auto* triMesh = new btTriangleMesh();

            for (size_t i = 0; i + 2 < verts.size(); i += 3) {
                btVector3 v0(verts[i].x,     verts[i].y,     verts[i].z);
                btVector3 v1(verts[i + 1].x, verts[i + 1].y, verts[i + 1].z);
                btVector3 v2(verts[i + 2].x, verts[i + 2].y, verts[i + 2].z);

                triMesh->addTriangle(v0, v1, v2);
            }

            shape = new btBvhTriangleMeshShape(triMesh, true);
        } else {
            // Create dynamic convex hull collider
            auto* hull = new btConvexHullShape();

            for (const auto& v : verts) {
                hull->addPoint(btVector3(v.x, v.y, v.z), false);
            }

            hull->recalcLocalAabb(); // finalize bounds
            shape = hull;
        }

        // Default transform (could be pulled from a component)
        btVector3 position(entityTransform.position.x, entityTransform.position.y, entityTransform.position.z);
        btTransform transform;
        transform.setIdentity();
        transform.setOrigin(position);

        // Motion state
        btDefaultMotionState* motionState = new btDefaultMotionState(transform);

        // Inertia
        btVector3 localInertia(0, 0, 0);
        if (mass > 0.0f)
            shape->calculateLocalInertia(mass, localInertia);

        // Rigid body
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
        btRigidBody* body = new btRigidBody(rbInfo);

        // Add to physics world
        world->addRigidBody(body);

        // Optionally, store a pointer to the body on the entity for later use

        mach::ecs::components::MeshCollider collider;
        collider.body = body;
        collider.shape = shape;
        collider.world = world;
        collider.motionState = motionState;

        registry.emplace_or_replace<mach::ecs::components::MeshCollider>(entity, collider);
        LOG_INFO("Created mesh collider");
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

    inline void onRigidBodyDestroyed(entt::registry& registry, entt::entity entity)
    {
        auto& rb = registry.get<RigidBody>(entity);
        if (rb.body) {
            rb.world->removeRigidBody(rb.body);
            delete rb.body;
            delete rb.motionState;
            delete rb.shape;
            rb.body = nullptr;
        }
    }

}

#endif //PHYSICSSYSTEM_H
