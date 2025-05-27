//
// Created by Dquavius Griffin on 5/27/25.
//

#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H
#include "../../external/entt.h"
#include "../components/Camera.h"
#include "../components/Transform.h"

namespace mach::ecs::systems
{
    using namespace mach::ecs;

    inline void updateCameraVectors(components::Camera& cameraComponent);
    inline void processCameraInput(components::Camera& cameraComponent, components::Transform& transformComponent, components::Camera_Movement direction, float deltaTime);

    inline void updateCameras(entt::registry& registry)
    {
        auto view = registry.view<components::Transform, components::Camera>();

        for (auto& entity : view)
        {
            auto& transformComponent = view.get<components::Transform>(entity);
            auto& cameraComponent = view.get<components::Camera>(entity);

            updateCameraVectors(cameraComponent);
        }
    }

    inline void updateCameraVectors(components::Camera& cameraComponent)
    {
        glm::vec3 front;
        front.x = cos(glm::radians(cameraComponent.Yaw)) * cos(glm::radians(cameraComponent.Pitch));
        front.y = sin(glm::radians(cameraComponent.Pitch));
        front.z = sin(glm::radians(cameraComponent.Yaw)) * cos(glm::radians(cameraComponent.Pitch));
        cameraComponent.Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        cameraComponent.Right = glm::normalize(glm::cross(cameraComponent.Front, cameraComponent.WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        cameraComponent.Up    = glm::normalize(glm::cross(cameraComponent.Right, cameraComponent.Front));
    }

    inline void processKeyboard(entt::registry& registry, entt::entity activeCameraEntity, int key, float deltaTime)
    {
        if (components::Camera* cam = registry.try_get<components::Camera>(activeCameraEntity)) {
            if (components::Transform* transform = registry.try_get<components::Transform>(activeCameraEntity)) {
                if (key == GLFW_KEY_W)
                    processCameraInput(*cam, *transform, components::FORWARD, deltaTime);
                if (key == GLFW_KEY_S)
                    processCameraInput(*cam, *transform, components::BACKWARD, deltaTime);
                if (key == GLFW_KEY_A)
                    processCameraInput(*cam, *transform, components::LEFT, deltaTime);
                if (key == GLFW_KEY_D)
                    processCameraInput(*cam, *transform, components::RIGHT, deltaTime);
            }
        }
    }

    inline void processCameraInput(components::Camera& cameraComponent, components::Transform& transformComponent, components::Camera_Movement direction, float deltaTime)
    {
        float velocity = cameraComponent.MovementSpeed * deltaTime;
        if (direction == components::FORWARD)
            transformComponent.position += cameraComponent.Front * velocity;
        if (direction == components::BACKWARD)
            transformComponent.position -= cameraComponent.Front * velocity;
        if (direction == components::LEFT)
            transformComponent.position -= cameraComponent.Right * velocity;
        if (direction == components::RIGHT)
            transformComponent.position += cameraComponent.Right * velocity;
    }

    inline void processMouseMovement(components::Camera& cameraComponent, float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= cameraComponent.MouseSensitivity;
        yoffset *= cameraComponent.MouseSensitivity;

        cameraComponent.Yaw   += xoffset;
        cameraComponent.Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (cameraComponent.Pitch > 89.0f)
                cameraComponent.Pitch = 89.0f;
            if (cameraComponent.Pitch < -89.0f)
                cameraComponent.Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors(cameraComponent);
    }

}

#endif //CAMERASYSTEM_H
