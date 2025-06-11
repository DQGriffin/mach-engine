//
// Created by Dquavius Griffin on 6/1/25.
//

#ifndef PLAINTEXTSCENESERIALIZER_H
#define PLAINTEXTSCENESERIALIZER_H

#include <iostream>
#include <fstream>

#include <glm/glm.hpp>
#include "../external/entt.h"
#include "../physics/PhysicsEngine.h"

namespace mach
{
    class AssetManager;
}

namespace mach::serialization
{
    class PlainTextSceneSerializer {
    public:
        PlainTextSceneSerializer() = default;
        ~PlainTextSceneSerializer() = default;

        void serialize(entt::registry& registry, const std::string& path);
        void deserialize(entt::registry& registry, mach::AssetManager& assets, mach::physics::PhysicsEngine& physics, const std::string& path);

    private:
        void serializeNameComponent(std::ofstream& stream, const entt::registry& registry, const entt::entity& entity);
        void serializeTransformComponent(std::ofstream& stream, const entt::registry& registry, const entt::entity& entity);
        void serializeModelComponent(std::ofstream& stream, const entt::registry& registry, const entt::entity& entity);
        void serializeRigidBodyComponent(std::ofstream& stream, const entt::registry& registry, const entt::entity& entity);
        void serializeMeshColliderComponent(std::ofstream& stream, const entt::registry& registry, const entt::entity& entity);
        void serializeVec3(std::ofstream& stream, const glm::vec3& vec);
        glm::vec3 parseVec3(const std::string& str);


    };
}

#endif //PLAINTEXTSCENESERIALIZER_H
