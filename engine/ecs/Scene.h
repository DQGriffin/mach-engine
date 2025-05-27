//
// Created by Dquavius Griffin on 5/26/25.
//
#pragma once
#ifndef SCENE_H
#define SCENE_H

#include "../external/entt.h"
#include "../input/input.h"
#include "../rendering/camera/Camera.h"
#include "systems/CameraSystem.h"

namespace mach::ecs
{
    class Entity;

    class Scene
    {
    public:
        Entity* activeCamera;
        mach::input::InputEngine* inputEngine;

    private:
        entt::registry registry;
        float deltaTime;

    public:
        Scene();
        ~Scene() = default;
        mach::ecs::Entity createEntity(const std::string& name);
        mach::ecs::Entity createCameraEntity(const std::string& name, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
        void destroyEntity(entt::entity entity);
        void update(float deltaTime);
        entt::registry& getRegistry() { return registry; }
        void registerInputCallbacks();

    private:
    };
}


#endif //SCENE_H
