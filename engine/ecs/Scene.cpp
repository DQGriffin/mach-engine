//
// Created by Dquavius Griffin on 5/26/25.
//

#include "Scene.h"
#include "components/RigidBody.h"
#include "Entity.h"
#include "../serialization/PlainTextSceneSerializer.h"
#include "../serialization/SceneSerialization.h"
#include "components/Name.h"
#include "components/PendingDeletion.h"
#include "systems/AnchorSystem.h"
#include "systems/MeshRendererSystem.h"
#include "systems/PhysicsSystem.h"
#include "systems/CameraSystem.h"
#include "systems/FpsCharacterControllerSystem.h"
#include "systems/OverheadCharacterControllerSystem.h"
#include "systems/InteractableSystem.h"
#include "systems/ScriptSystem.h"
#include "systems/ZoneSystem.h"

mach::ecs::Scene::Scene()
{
    debugLineRenderer = nullptr;
    isFlyCam = false;
    registerEntityTypes();
    registry.on_destroy<components::RigidBody>().connect<&systems::onRigidBodyDestroyed>();
}

void mach::ecs::Scene::setPlayerEntity(Entity* entity)
{
    playerEntity = entity;
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
    auto& c = registry.emplace<components::Transform>(entity.handle(), position, position, glm::vec3(1.0, 1.0, 1.0f));
    auto& cam = registry.emplace<components::Camera>(entity.handle());

    // For testing top-down camera
    cam.Pitch = -90.0f;
    return entity;
}

void mach::ecs::Scene::attachScript(mach::ecs::Entity entity, const std::string& path)
{
    registry.emplace<components::ScriptComponent>(entity.handle());
    systems::attachScriptToEntity(registry, entity, path, luaManager.lua);
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
    mach::ecs::systems::syncMeshColliderTransforms(registry);
    mach::ecs::systems::updateCameras(registry);
    mach::ecs::systems::callUpdateScrips(registry, deltaTime);
    mach::ecs::systems::updateAnchorPositions(registry);
    mach::ecs::systems::updateZones(registry);
    mach::ecs::systems::drawZones(registry, debugLineRenderer);
    // mach::ecs::systems::interactNearby(registry, playerEntity);
}

void mach::ecs::Scene::registerInputCallbacks()
{

    inputEngine->registerKeyPressCallback([this](int key, int duration) -> bool {
        if (key == MACH_KEY_SLASH)
        {
            isFlyCam = !isFlyCam;
        }
        else if (key == MACH_KEY_E)
        {
            mach::ecs::systems::interactNearby(registry, playerEntity);
        }

        return false;
    }, input::CallbackPriority::Gameplay, input::KeyEventTrigger::Released);

    inputEngine->registerKeyPressCallback([this](int key, int duration) -> bool {
        if (isFlyCam)
        {
            systems::processKeyboard(this->registry, this->activeCamera->handle(), key, this->deltaTime);
        }
        else
        {
            mach::ecs::systems::fps::processInput(key, registry, playerEntity, activeCamera);
            mach::ecs::systems::overhead::processInput(key, registry, playerEntity, activeCamera);
        }
        // systems::processKeyboard(this->registry, this->activeCamera->handle(), key, this->deltaTime);
        return false;
    }, input::CallbackPriority::Gameplay, input::KeyEventTrigger::Pressed);

    inputEngine->registerMousePositionCallback([this](int x, int y) -> bool {
        if (!processMouseInput) return false;

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

void mach::ecs::Scene::toggleFlyCamera(bool value)
{
    // isFlyCam = value;
    // if (isFlyCam)
    // {
    //     auto& anchor = registry.get<mach::ecs::components::Anchor>(activeCamera->handle());
    //     anchor.x = false;
    //     anchor.y = false;
    //     anchor.z = false;
    // }
    // else
    // {
    //     auto& anchor = registry.get<mach::ecs::components::Anchor>(activeCamera->handle());
    //     anchor.x = true;
    //     anchor.y = true;
    //     anchor.z = true;
    // }
}

bool mach::ecs::Scene::getProcessMouseInput()
{
    return processMouseInput;
}

void mach::ecs::Scene::setProcessMouseInput(bool value)
{
    processMouseInput = value;
}

void mach::ecs::Scene::serialize(const std::string& path, SerializationType type)
{
    switch (type)
    {
        case SerializationType::PLAIN_TEXT:
            serialization::PlainTextSceneSerializer serializer;
            serializer.serialize(registry, path);
            break;
        case SerializationType::JSON:
            LOG_ERROR("JSON serialization is not yet supported");
            break;
        case SerializationType::BINARY:
            LOG_ERROR("Binary serialization is not yet supported");
            break;
        case SerializationType::YAML:
            LOG_ERROR("YAML serialization is not yet supported");
            break;
        default:
            LOG_ERROR("Unknown serialization type");
    }
}

void mach::ecs::Scene::registerEntityTypes()
{
    luaManager.lua.new_usertype<glm::vec3>("Vec3",
        sol::call_constructor, sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
        "x", &glm::vec3::x,
        "y", &glm::vec3::y,
        "z", &glm::vec3::z,
        sol::meta_function::to_string, [](const glm::vec3& v) {
            return std::format("x: {}, y: {}, z: {}", v.x, v.y, v.z);
        }
    );

    luaManager.lua.new_usertype<mach::ecs::Entity>("Entity",
            "get_name", &mach::ecs::Entity::getName,
            "get_position", &mach::ecs::Entity::getPosition,
            "set_position", &mach::ecs::Entity::setPosition,
            "apply_force", &mach::ecs::Entity::applyForce,
            "destroy", &mach::ecs::Entity::destroy
    );

    luaManager.lua["delete_entity"] = [this](entt::entity entity) {
        if (registry.valid(entity))
        {
            registry.emplace<components::PendingDeletion>(entity);
            LOG_DEBUG("Marked entity for deletion");
        }
    };

    luaManager.lua["apply_impulse"] = [this](entt::entity entity, glm::vec3 force) {
        auto& rigidBodyComponent = registry.get<mach::ecs::components::RigidBody>(entity);
        rigidBodyComponent.applyImpulse(force);
    };

    luaManager.lua["translate"] = [this](entt::entity entity, glm::vec3 translation) {
        auto& transform = registry.get<mach::ecs::components::Transform>(entity);
        transform.position += translation;

        if (registry.any_of<components::RigidBody>(entity)) {
            auto& rb = registry.get<mach::ecs::components::RigidBody>(entity);
            if (rb.body) {
                btTransform btTrans;
                btTrans.setIdentity();
                btTrans.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
                rb.body->setWorldTransform(btTrans);
                rb.body->getMotionState()->setWorldTransform(btTrans);
            }
        }
    };
}
