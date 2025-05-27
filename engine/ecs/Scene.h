//
// Created by Dquavius Griffin on 5/26/25.
//
#pragma once
#ifndef SCENE_H
#define SCENE_H

#include "../external/entt.h"

namespace mach::ecs
{
    class Entity;

    class Scene
    {
    private:
        entt::registry registry;

    public:
        Scene() = default;
        ~Scene() = default;
        mach::ecs::Entity createEntity(const std::string& name);
        void destroyEntity(entt::entity entity);
        void update(float deltaTime);
        entt::registry& getRegistry() { return registry; }
    };
}


#endif //SCENE_H
