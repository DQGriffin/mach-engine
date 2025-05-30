//
// Created by Dquavius Griffin on 5/29/25.
//

#ifndef CAPSULCOLLIDER_H
#define CAPSULCOLLIDER_H

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

namespace mach::ecs::components
{
    struct CapsuleCollider
    {
        btRigidBody* body;
        btCapsuleShape* shape;
        btDefaultMotionState* motionState;
        btDiscreteDynamicsWorld* world;
    };
}

#endif //CAPSULCOLLIDER_H
