//
// Created by Dquavius Griffin on 5/29/25.
//

#ifndef MESHCOLLIDER_H
#define MESHCOLLIDER_H

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

namespace mach::ecs::components
{
    struct MeshCollider
    {
        btRigidBody* body;
        btCollisionShape* shape;
        btDefaultMotionState* motionState;
        btDiscreteDynamicsWorld* world;
    };
}

#endif //MESHCOLLIDER_H
