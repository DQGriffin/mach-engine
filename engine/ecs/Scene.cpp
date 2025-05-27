//
// Created by Dquavius Griffin on 5/26/25.
//

#include "Scene.h"
#include "Entity.h"
#include "components/Name.h"
#include "systems/MeshRendererSystem.h"
#include "systems/PhysicsSystem.h"
#include "systems/CameraSystem.h"

mach::ecs::Scene::Scene()
{
    // camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f));
}

mach::ecs::Entity mach::ecs::Scene::createEntity(const std::string &name)
{
    entt::entity raw = registry.create();
    registry.emplace<components::Name>(raw, name);
    return Entity(raw, this);
}

mach::ecs::Entity mach::ecs::Scene::createCameraEntity(const std::string &name, glm::vec3 position)
{
    auto entity = createEntity(name);
    registry.emplace<components::Transform>(entity.handle(), position, position, glm::vec3(1.0, 1.0, 1.0f));
    registry.emplace<components::Camera>(entity.handle());
    return entity;
}

void mach::ecs::Scene::destroyEntity(entt::entity entity)
{
    registry.destroy(entity);
}

void mach::ecs::Scene::update(float deltaTime)
{
    this->deltaTime = deltaTime;
    mach::ecs::systems::render(registry);
    mach::ecs::systems::syncTransforms(registry);
    mach::ecs::systems::updateCameras(registry);
}

void mach::ecs::Scene::registerInputCallbacks()
{
    inputEngine->registerKeyPressCallback([this](int key, int duration) -> bool {
        systems::processKeyboard(this->registry, this->activeCamera->handle(), key, this->deltaTime);
        return false;
    }, input::CallbackPriority::Gameplay, input::KeyEventTrigger::Pressed);

    inputEngine->registerMousePositionCallback([this](int x, int y) -> bool {
        auto& cameraComponent = registry.get<mach::ecs::components::Camera>(activeCamera->handle());

        float xpos = static_cast<float>(x);
        float ypos = static_cast<float>(y);

        if (cameraComponent.firstMouse)
        {
            cameraComponent.lastX = xpos;
            cameraComponent.lastY = ypos;
            cameraComponent.firstMouse = false;
        }

        float xoffset = xpos - cameraComponent.lastX;
        float yoffset = cameraComponent.lastY - ypos; // reversed since y-coordinates go from bottom to top

        cameraComponent.lastX = xpos;
        cameraComponent.lastY = ypos;

        systems::processMouseMovement(cameraComponent, xoffset, yoffset, true);
        return false;
    }, input::CallbackPriority::Gameplay);
}
