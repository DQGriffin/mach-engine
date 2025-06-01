//
// Created by Dquavius Griffin on 6/1/25.
//

#include "PlainTextSceneSerializer.h"

#include <xpc/xpc.h>

#include "../assets/AssetManager.h"
#include "../assets/loaders/shaders.h"
#include "../ecs/components/Transform.h"
#include "../ecs/components/Name.h"
#include "../ecs/components/MeshRender.h"
#include "../ecs/components/RigidBody.h"
#include "../ecs/systems/PhysicsSystem.h"
#include "../physics/PhysicsEngine.h"
#include "../utils/string_utils.h"

enum class ParseState { None, Entity, Transform, Model, Rigidbody };

void mach::serialization::PlainTextSceneSerializer::serialize(entt::registry& registry, const std::string& path)
{
    std::ofstream file(path);
    if (!file.is_open())
    {
        LOG_ERROR("Scene could not be serialized. File failed to open")
        return;
    }

    for (auto& entity : registry.storage<entt::entity>())
    {
        auto name = registry.try_get<mach::ecs::components::Name>(entity);
        if (name == nullptr || name->name == "Editor Camera")
        {
            continue;
        }

        file << "[" << static_cast<int>(entity) << "]" << std::endl;
        serializeNameComponent(file, registry, entity);
        serializeTransformComponent(file, registry, entity);
        serializeModelComponent(file, registry, entity);
        serializeRigidBodyComponent(file, registry, entity);
        file << std::endl;
    }

    file.close();
}

void mach::serialization::PlainTextSceneSerializer::deserialize(entt::registry& registry, mach::AssetManager& assets, mach::physics::PhysicsEngine& physics, const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        LOG_ERROR("Scene could not be deserialized. File failed to open")
        return;
    }

    std::unordered_map<int, entt::entity> entities;
    ParseState state = ParseState::None;
    entt::entity current;
    std::string line;

    while (std::getline(file, line)) {
        if (line.starts_with('[')) {
            auto entityId = line.substr(1, line.size() - 2);
            LOG_INFO(std::format("Deserializing entity: {}", entityId))
            current = registry.create();
            state = ParseState::Entity;
        } else if (line.starts_with("Name:")) {
            auto entityName = line.substr(line.find_last_of(":") + 2, line.size() - 2);
            registry.emplace<mach::ecs::components::Name>(current, entityName);
        } else if (line.starts_with("Transform:")) {
            state = ParseState::Transform;
            registry.emplace<mach::ecs::components::Transform>(current);
        } else if (line.starts_with("Model:")) {
            state = ParseState::Model;
        } else if (line.starts_with("Rigidbody:")) {
            state = ParseState::Rigidbody;
        } else if (state == ParseState::Transform) {
            if (line.contains("Position:"))
            {
                auto transform = registry.try_get<mach::ecs::components::Transform>(current);
                if (transform)
                {
                    transform->position = parseVec3(line);
                }
            }
            else if (line.contains("Rotation:"))
            {
                auto transform = registry.try_get<mach::ecs::components::Transform>(current);
                if (transform)
                {
                    transform->rotation = parseVec3(line);
                }
            }
            else if (line.contains("Scale:"))
            {
                auto transform = registry.try_get<mach::ecs::components::Transform>(current);
                if (transform)
                {
                    transform->scale = parseVec3(line);
                }
            }
        } else if (state == ParseState::Model) {
            if (line.contains("Path:"))
            {
                auto modelPath = line.substr(line.find_last_of(":") + 2, line.size() - 2);
                auto model = assets.tryGetModel(modelPath);

                if (model == nullptr)
                {
                    LOG_ERROR(std::format("Failed to load model: {}", modelPath))
                }

                // TODO: Don't hardcode the shader when deserializing scenes
                auto& shader = assets.requireShader("lighting");

                mach::ecs::components::ModelRenderer modelRenderer;
                modelRenderer.model = model;
                modelRenderer.shader = &shader;
                registry.emplace<mach::ecs::components::ModelRenderer>(current, modelRenderer);
            }
            // current->model.path = parse_string(line);
        } else if (state == ParseState::Rigidbody) {
            // Parse rigidbody fields
            if (line.contains("Mass:"))
            {
                // Create the rigid body here. Need the model to be loaded first
                auto modelComponent = registry.try_get<mach::ecs::components::ModelRenderer>(current);
                if (modelComponent == nullptr)
                {
                    LOG_ERROR("Cannot deserialize rigidbody because the associated model was not loaded")
                    continue;
                }

                auto massString = line.substr(line.find_last_of(":") + 2, line.size() - 2);
                float mass = std::stof(massString);

                mach::physics::createPhysicsBody(registry, current, physics.world, *modelComponent->model, false, mass);
            }
        }
    }

    file.close();
}

void mach::serialization::PlainTextSceneSerializer::serializeNameComponent(std::ofstream& stream, const entt::registry& registry, const entt::entity& entity)
{
    auto nameComponent = registry.try_get<mach::ecs::components::Name>(entity);
    if (nameComponent == nullptr)
    {
        return;
    }

    stream << "Name: " << nameComponent->name << std::endl;
}

void mach::serialization::PlainTextSceneSerializer::serializeTransformComponent(std::ofstream& stream, const entt::registry& registry, const entt::entity& entity)
{
    auto transform = registry.try_get<mach::ecs::components::Transform>(entity);
    if (transform == nullptr)
    {
        return;
    }

    stream << "Transform:" << std::endl;
    stream << "  Position: ";
    serializeVec3(stream, transform->position);
    stream << "  Rotation: ";
    serializeVec3(stream, transform->rotation);
    stream << "  Scale: ";
    serializeVec3(stream, transform->scale);
}

void mach::serialization::PlainTextSceneSerializer::serializeModelComponent(std::ofstream& stream, const entt::registry& registry, const entt::entity& entity)
{
    auto modelComponent = registry.try_get<mach::ecs::components::ModelRenderer>(entity);
    if (modelComponent == nullptr || modelComponent->model == nullptr)
    {
        return;
    }

    stream << "Model: " << std::endl;
    stream << "  Path: " << modelComponent->model->getPath() << std::endl;
}

void mach::serialization::PlainTextSceneSerializer::serializeRigidBodyComponent(std::ofstream& stream, const entt::registry& registry, const entt::entity& entity)
{
    auto rb = registry.try_get<mach::ecs::components::RigidBody>(entity);
    if (rb == nullptr)
    {
        return;
    }

    stream << "Rigidbody:" << std::endl;
    stream << "  Mass: " << rb->body->getMass() << std::endl;
    stream << "  Capsule: " << false << std::endl;
}

void mach::serialization::PlainTextSceneSerializer::serializeVec3(std::ofstream& stream, const glm::vec3& vec)
{
    stream << vec.x << " " << vec.y << " " << vec.z << std::endl;
}

glm::vec3 mach::serialization::PlainTextSceneSerializer::parseVec3(const std::string& str)
{
    auto values = str.substr(str.find_last_of(":") + 2, str.size() - 1);
    auto componentStrings = mach::utils::split_str(values, ' ');

    float x = std::stof(componentStrings[0]);
    float y = std::stof(componentStrings[1]);
    float z = std::stof(componentStrings[2]);

    return glm::vec3(x, y, z);
}
