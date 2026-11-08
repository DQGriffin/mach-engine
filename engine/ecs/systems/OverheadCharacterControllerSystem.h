//
// Created by Dquavius Griffin on 11/7/26.
//

#ifndef OVERHEADCHARACTERCONTROLLERSYSTEM_H
#define OVERHEADCHARACTERCONTROLLERSYSTEM_H
#include "glm/detail/type_vec.hpp"
#include "LinearMath/btVector3.h"
#include "../Entity.h"
#include "../components/OverheadCharacterController.h"
#include "../components/RigidBody.h"
#include "../components/Camera.h"
#include "../components/Transform.h"

namespace mach::ecs::systems::overhead
{
    inline void processInput(int key, entt::registry& registry, Entity* playerEntity, Entity* activeCamera)
    {
        auto view = registry.view<mach::ecs::components::OverheadCharacterController>();

        for (auto entity : view)
        {
            if (key == MACH_KEY_W || key == MACH_KEY_A || key == MACH_KEY_S || key == MACH_KEY_D)
            {
                auto& rb = registry.get<mach::ecs::components::RigidBody>(entity);
                auto& cam = registry.get<mach::ecs::components::Camera>(activeCamera->handle());
                auto& characterController = registry.get<mach::ecs::components::OverheadCharacterController>(entity);
                auto& transform = registry.get<mach::ecs::components::Transform>(entity);

                if (!characterController.isGrounded || !characterController.canMove) continue;

                float speed = characterController.isSprinting ? characterController.sprintMovementSpeed : characterController.movementSpeed;

                float inputX = 0.0f;
                float inputY = 0.0f;

                if (key == MACH_KEY_W)
                {
                    transform.rotation.y = 0.0f;
                    inputY += 1.0f;
                }
                if (key == MACH_KEY_S)
                {
                    transform.rotation.y = -3.1f;
                    inputY -= 1.0f;
                }
                if (key == MACH_KEY_A)
                {
                    transform.rotation.y = 1.5f;
                    inputX -= 1.0f;
                }
                if (key == MACH_KEY_D)
                {
                    transform.rotation.y = -1.5f;
                    inputX += 1.0f;
                }

                glm::vec3 forward = cam.Front;
                forward.y = 0.0f;

                forward = glm::normalize(forward);


                glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
                glm::vec3 moveDir = forward * inputY + right * inputX;
                moveDir = glm::normalize(moveDir);

                auto vel = rb.body->getLinearVelocity();
                vel.setX(moveDir.x * speed);
                vel.setZ(moveDir.z * speed);
                rb.body->setActivationState(DISABLE_DEACTIVATION);
                rb.body->setDamping(0.97f, 0.0f);
                rb.body->setLinearVelocity(vel);
            }
            else if (key == MACH_KEY_SPACE)
            {
                auto& rb = registry.get<mach::ecs::components::RigidBody>(entity);
                rb.body->setDamping(0.0f, 0.0f);
                rb.body->applyCentralImpulse(btVector3(0.0f, 5.0f, 0.0f));
            }
        }
    }
}

#endif //OVERHEADCHARACTERCONTROLLERSYSTEM_H
