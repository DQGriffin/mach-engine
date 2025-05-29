//
// Created by Dquavius Griffin on 5/26/25.
//

#include "Scene.h"
#include "components/RigidBody.h"
#include "Entity.h"
#include "components/Name.h"
#include "components/PendingDeletion.h"
#include "systems/AnchorSystem.h"
#include "systems/MeshRendererSystem.h"
#include "systems/PhysicsSystem.h"
#include "systems/CameraSystem.h"
#include "systems/ScriptSystem.h"

mach::ecs::Scene::Scene()
{
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
    registry.emplace<components::Transform>(entity.handle(), position, position, glm::vec3(1.0, 1.0, 1.0f));
    registry.emplace<components::Camera>(entity.handle());
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
    mach::ecs::systems::updateCameras(registry);
    mach::ecs::systems::callUpdateScrips(registry, deltaTime);
    mach::ecs::systems::updateAnchorPositions(registry);
}

void mach::ecs::Scene::registerInputCallbacks()
{
    inputEngine->registerKeyPressCallback([this](int key, int duration) -> bool {
        // systems::processKeyboard(this->registry, this->activeCamera->handle(), key, this->deltaTime);
            if (key == MACH_KEY_W || key == MACH_KEY_A || key == MACH_KEY_S || key == MACH_KEY_D)
            {
                auto& rb = registry.get<mach::ecs::components::RigidBody>(playerEntity->handle());
                auto& cam = registry.get<mach::ecs::components::Camera>(activeCamera->handle());

                float movementSpeed = 7.0f;

                float inputX = 0.0f;
                float inputY = 0.0f;

                if (key == MACH_KEY_W) inputY += 1.0f;
                if (key == MACH_KEY_S) inputY -= 1.0f;
                if (key == MACH_KEY_A) inputX -= 1.0f;
                if (key == MACH_KEY_D) inputX += 1.0f;

                glm::vec3 forward = cam.Front;
                forward.y = 0.0f;

                forward = glm::normalize(forward);


                glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
                glm::vec3 moveDir = forward * inputY + right * inputX;
                moveDir = glm::normalize(moveDir);

                auto vel = rb.body->getLinearVelocity();
                vel.setX(moveDir.x * movementSpeed);
                vel.setZ(moveDir.z * movementSpeed);
                rb.body->setActivationState(DISABLE_DEACTIVATION);
                rb.body->setDamping(0.97f, 0.2f);
                rb.body->setLinearVelocity(vel);
            }

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
