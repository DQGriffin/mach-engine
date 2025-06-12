//
// Created by Dquavius Griffin on 5/26/25.
//
#pragma once
#ifndef SCENE_H
#define SCENE_H

#include "../external/entt.h"
#include "../input/input.h"
#include "../physics/PhysicsEngine.h"
#include "../rendering/camera/Camera.h"
#include "../rendering/debug/BulletDebugDrawer.h"
#include "../scripting/lua/types/lua_types.h"
#include "../scripting/lua/types/StateManager.h"
#include "systems/CameraSystem.h"

namespace mach::ecs
{
    class Entity;

    enum class SerializationType
    {
        PLAIN_TEXT = 1,
        JSON,
        YAML,
        BINARY,
    };

    class Scene
    {
    public:
        Entity* activeCamera;
        Entity* playerEntity;
        mach::input::InputEngine* inputEngine;
        mach::renderer::DebugLineRenderer* debugLineRenderer;
        mach::physics::PhysicsEngine physics;
        mach::scripting::lua::StateManager luaManager;
        bool isFlyCam = true;

    private:
        entt::registry registry;
        float deltaTime;
        bool processMouseInput = true;

    public:
        Scene();
        ~Scene() = default;
        void setPlayerEntity(Entity* entity);
        mach::ecs::Entity createEntity(const std::string& name);
        mach::ecs::Entity createCameraEntity(const std::string& name, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
        void attachScript(mach::ecs::Entity entity, const std::string& path);
        void destroyEntity(entt::entity entity);
        void update(float deltaTime);
        entt::registry& getRegistry() { return registry; }
        void registerInputCallbacks();
        void toggleFlyCamera(bool value);
        bool getProcessMouseInput();
        void setProcessMouseInput(bool value);
        void serialize(const std::string& path, SerializationType type);

    private:
        void registerEntityTypes();

    private:
    };
}


#endif //SCENE_H
