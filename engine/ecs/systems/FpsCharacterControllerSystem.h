//
// Created by Dquavius Griffin on 5/29/25.
//

#ifndef FPSCHARACTERCONTROLLERSYSTEM_H
#define FPSCHARACTERCONTROLLERSYSTEM_H
#include "../Entity.h"
#include "../../external/entt.h"
#include "glm/detail/type_vec.hpp"
#include "LinearMath/btVector3.h"
#include "../components/RigidBody.h"
#include "../components/FpsCharacterController.h"
#include "../components/Camera.h"


namespace mach::ecs::systems::fps
{
    inline void processInput(int key, entt::registry& registry, Entity* playerEntity, Entity* activeCamera)
    {
        auto view = registry.view<mach::ecs::components::FpsCharacterController>();

        for (auto& entity : view)
        {
            if (key == MACH_KEY_W || key == MACH_KEY_A || key == MACH_KEY_S || key == MACH_KEY_D)
            {
                auto& rb = registry.get<mach::ecs::components::RigidBody>(entity);
                auto& cam = registry.get<mach::ecs::components::Camera>(activeCamera->handle());
                auto& characterController = registry.get<mach::ecs::components::FpsCharacterController>(entity);

                if (!characterController.isGrounded || !characterController.canMove) continue;

                float speed = characterController.isSprinting ? characterController.sprintMovementSpeed : characterController.movementSpeed;

                float inputX = 0.0f;
                float inputY = 0.0f;

                if (key == MACH_KEY_W) inputY += 1.0f;
                if (key == MACH_KEY_S) inputY -= 1.0f;
                if (key == MACH_KEY_A) inputX -= 1.0f;
                if (key == MACH_KEY_D) inputX += 1.0f;

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

            if (key == MACH_KEY_SPACE)
            {
                auto& rb = registry.get<mach::ecs::components::RigidBody>(entity);
                rb.body->setDamping(0.0f, 0.0f);
                rb.body->applyCentralImpulse(btVector3(0.0f, 5.0f, 0.0f));
            }
        }
    }
}

#endif //FPSCHARACTERCONTROLLER_H
