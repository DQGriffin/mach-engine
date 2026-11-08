//
// Created by Dquavius Griffin on 11/7/26.
//

#ifndef OVERHEADCHARACTERCONTROLLER_H
#define OVERHEADCHARACTERCONTROLLER_H

namespace mach::ecs::components
{
    enum class OverheadDirection
    {
        FORWARD = 1,
        BACKWARD,
        LEFT,
        RIGHT
    };

    struct OverheadCharacterController
    {
        OverheadDirection direction = OverheadDirection::FORWARD;
        float movementSpeed = 7.0f;
        float crouchMovementSpeed = 3.5f;
        float sprintMovementSpeed = 15.0f;
        bool canMove = true;
        bool isCrouching = false;
        bool isSprinting = false;
        bool isGrounded = true;
        bool canJump = true;
    };
}

#endif //OVERHEADCHARACTERCONTROLLER_H
