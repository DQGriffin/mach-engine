//
// Created by Dquavius Griffin on 5/27/25.
//

#include "PhysicsEngine.h"

mach::physics::PhysicsEngine::PhysicsEngine()
{
    broadphase = new btDbvtBroadphase();
    config = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(config);
    solver = new btSequentialImpulseConstraintSolver();
    world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config);

    setGravity(glm::vec3(0.0f, -12.81f, 0.0f));
    setPaused(false);
}

mach::physics::PhysicsEngine::~PhysicsEngine()
{
    delete broadphase;
    delete config;
    delete dispatcher;
    delete solver;
    delete world;
}

void mach::physics::PhysicsEngine::step(float deltaTime, int maxSubSteps)
{
    if (isPaused) return;

    world->stepSimulation(deltaTime, 10);
}

void mach::physics::PhysicsEngine::setGravity(glm::vec3 gravity)
{
    this->gravity = gravity;
    world->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
}

void mach::physics::PhysicsEngine::setPaused(bool paused)
{
    isPaused = paused;
}

void mach::physics::PhysicsEngine::setDebugDrawer(btIDebugDraw *drawer)
{
    this->drawer = drawer;
    world->setDebugDrawer(drawer);
}

void mach::physics::PhysicsEngine::setDrawWireframe(bool drawWireframe)
{
    drawWireframes = drawWireframe;

    // This is done this way mainly for scripting purposes
    if (drawWireframes)
        drawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
}
