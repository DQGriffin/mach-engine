//
// Created by Dquavius Griffin on 5/26/25.
//

#include "Scene.h"
#include "Entity.h"
#include "components/Name.h"
#include "systems/MeshRendererSystem.h"

mach::ecs::Entity mach::ecs::Scene::createEntity(const std::string &name)
{
    entt::entity raw = registry.create();
    registry.emplace<components::Name>(raw, name);
    return Entity(raw, this);
}

void mach::ecs::Scene::destroyEntity(entt::entity entity)
{
    registry.destroy(entity);
}

void mach::ecs::Scene::update(float deltaTime)
{
    mach::ecs::systems::render(registry);
}
