#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include <glm/glm.hpp>

#include "../../logging/logging.h"
#include "glm/gtc/matrix_transform.hpp"

// Default camera values
const float DEFAULT_YAW         = -90.0f;
const float DEFAULT_PITCH       =  0.0f;
const float DEFAULT_SPEED       =  2.5f;
const float DEFAULT_SENSITIVITY =  0.1f;
const float DEFAULT_ZOOM        =  45.0f;

namespace mach::ecs::components
{
    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum Camera_Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    struct Camera
    {
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;

        float Yaw;
        float Pitch;

        float MovementSpeed;
        float MouseSensitivity;
        float Zoom;

        bool firstMouse = true;
        float lastX;
        float lastY;

        Camera()
        {
            Up = glm::vec3(0.0f, 1.0f, 0.0f);
            WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
            Front = glm::vec3(0.0f, 0.0f, -1.0f);
            Yaw = DEFAULT_YAW;
            Pitch = DEFAULT_PITCH;
            MovementSpeed = DEFAULT_SPEED;
            MouseSensitivity = DEFAULT_SENSITIVITY;
            Zoom = DEFAULT_ZOOM;
            firstMouse = true;
        }

        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
        [[nodiscard]] glm::mat4 GetViewMatrix(glm::vec3 position) const
        {
            return glm::lookAt(position, position + Front, Up);
        }
    };
}

#endif //CAMERA_COMPONENT_H
