//
// Created by Dquavius Griffin on 5/27/25.
//

#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include <glm/glm.hpp>

namespace mach::physics
{
    class PhysicsEngine
    {
    public:
        btDiscreteDynamicsWorld* world;
        bool isPaused;
    private:
        btBroadphaseInterface* broadphase;
        btDefaultCollisionConfiguration* config;
        btCollisionDispatcher* dispatcher;
        btSequentialImpulseConstraintSolver* solver;
        btIDebugDraw* drawer;

        bool drawWireframes;
        glm::vec3 gravity;

    public:
        PhysicsEngine();
        ~PhysicsEngine();
        void step(float deltaTime, int maxSubSteps = 10);
        void setGravity(glm::vec3 gravity);
        void setPaused(bool paused);
        void setDebugDrawer(btIDebugDraw* drawer);
        void setDrawWireframe(bool drawWireframe);
    };
}


#endif //PHYSICSENGINE_H
