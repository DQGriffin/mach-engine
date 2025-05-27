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

        void applyForce(const glm::vec3& force) const
        {
            body->applyForce(btVector3(force.x, force.y, force.z), btVector3(0, 1, 0));
        }

        void applyImpulse(const glm::vec3& impulse) const
        {
            body->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
        }
    };
}

#endif //RIGIDBODY_H
