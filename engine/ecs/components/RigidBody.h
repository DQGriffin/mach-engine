//
// Created by Dquavius Griffin on 5/27/25.
//

#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

namespace mach::ecs::components
{
    struct RigidBody
    {
        btRigidBody* body;
        btCollisionShape* shape;
        btDefaultMotionState* motionState;
    };
}

#endif //RIGIDBODY_H
