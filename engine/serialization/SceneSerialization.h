//
// Created by Dquavius Griffin on 5/31/25.
//

#ifndef SCENESERIALIZATION_H
#define SCENESERIALIZATION_H

#include <iostream>
#include <fstream>

#include "../ecs/components/MeshRender.h"
#include "../ecs/components/Name.h"
#include "../external/entt.h"
#include "../logging/logging.h"

namespace mach::serialization
{
    void serializeNameComponent(std::ofstream& stream, const entt::registry& registry, const entt::entity& entity);
    void serializeTransformComponent(std::ofstream& stream, const entt::registry& registry, const entt::entity& entity);
    void serializeModelComponent(std::ofstream& stream, const entt::registry& registry, const entt::entity& entity);
    void serializeRigidBodyComponent(std::ofstream& stream, const entt::registry& registry, const entt::entity& entity);
    void serializeVec3(std::ofstream& stream, const glm::vec3& vec);

    inline void serializeScenePlainText(entt::registry& registry, const std::string& filePath)
    {
        std::ofstream file(filePath);
        if (!file.is_open())
        {
            LOG_ERROR("Scene could not be serialized. File failed to open")
            return;
        }

        for (auto& entity : registry.storage<entt::entity>())
        {
            file << "[" << static_cast<int>(entity) << "]" << std::endl;
            serializeNameComponent(file, registry, entity);
            serializeTransformComponent(file, registry, entity);
            serializeModelComponent(file, registry, entity);
            serializeRigidBodyComponent(file, registry, entity);
            file << std::endl;
        }

        file.close();
    }

    inline void serializeNameComponent(std::ofstream& stream, const entt::registry& registry, const entt::entity& entity)
    {
        auto nameComponent = registry.try_get<mach::ecs::components::Name>(entity);
        if (nameComponent == nullptr)
        {
            return;
        }

        stream << "Name: " << nameComponent->name << std::endl;
    }

    inline void serializeTransformComponent(std::ofstream& stream, const entt::registry& registry, const entt::entity& entity)
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

    inline void serializeModelComponent(std::ofstream& stream, const entt::registry& registry, const entt::entity& entity)
    {
        auto modelComponent = registry.try_get<mach::ecs::components::ModelRenderer>(entity);
        if (modelComponent == nullptr || modelComponent->model == nullptr)
        {
            return;
        }

        stream << "Model: " << std::endl;
        stream << "  Path: " << modelComponent->model->getPath() << std::endl;
    }

    inline void serializeRigidBodyComponent(std::ofstream& stream, const entt::registry& registry, const entt::entity& entity)
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

    inline void serializeVec3(std::ofstream& stream, const glm::vec3& vec)
    {
        stream << vec.x << " " << vec.y << " " << vec.z << std::endl;
    }


}

#endif //SCENESERIALIZATION_H
