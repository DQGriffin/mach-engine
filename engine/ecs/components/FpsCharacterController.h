//
// Created by Dquavius Griffin on 5/29/25.
//

#ifndef FPSCHARACTERCONTROLLER_H
#define FPSCHARACTERCONTROLLER_H

namespace mach::ecs::components
{
    struct FpsCharacterController
    {
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
#endif //FPSCHARACTERCONTROLLER_H
