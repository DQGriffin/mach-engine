//
// Created by Dquavius Griffin on 5/26/25.
//
#include "Entity.h"

#include "../math/vec.h"
#include "components/Transform.h"
#include "components/MeshRender.h"
#include "components/Name.h"
#include "components/RigidBody.h"

namespace mach::ecs
{
    Entity::Entity(entt::entity handle, Scene* scene)
        : m_Entity(handle), m_Scene(scene) {}

    bool Entity::isValid() const {
        return m_Entity != entt::null && m_Scene != nullptr;
    }

    std::string Entity::getName()
    {
        auto nameComponent = getComponent<mach::ecs::components::Name>();
        return nameComponent.name;
    }

    glm::vec3 Entity::getPosition()
    {
        if (hasComponent<mach::ecs::components::Transform>())
        {
            auto nameComponent = getComponent<mach::ecs::components::Transform>();
            return nameComponent.position;
        }

        return glm::vec3();
    }

    void Entity::setPosition(const glm::vec3 &position)
    {
        LOG_DEBUG(std::format("Entity::setPosition: x:{}, y:{}, z:{}", position.x, position.y, position.z));
        if (hasComponent<mach::ecs::components::Transform>())
        {
            auto& transformComponent = getComponent<mach::ecs::components::Transform>();
            transformComponent.position = position;
        }
    }

    void Entity::applyForce(const glm::vec3 &force)
    {
        LOG_DEBUG(std::format("Entity::applyForce: x:{}, y:{}, z:{}", force.x, force.y, force.z));
        if (hasComponent<mach::ecs::components::RigidBody>())
        {
            auto& rigidBodyComponent = getComponent<mach::ecs::components::RigidBody>();
            rigidBodyComponent.applyForce(force);
            LOG_DEBUG("applied force")
        }
    }

    template<typename T, typename... Args>
    T& Entity::addComponent(Args&&... args) {
        return m_Scene->getRegistry().emplace<T>(m_Entity, std::forward<Args>(args)...);
    }

    template<typename T>
    bool Entity::hasComponent() const {
        return m_Scene->getRegistry().all_of<T>(m_Entity);
    }

    template<typename T>
    T& Entity::getComponent() {
        return m_Scene->getRegistry().get<T>(m_Entity);
    }

    template mach::ecs::components::Transform& mach::ecs::Entity::addComponent<mach::ecs::components::Transform, mach::vec3>(mach::vec3&&);
    template bool mach::ecs::Entity::hasComponent<mach::ecs::components::Transform>() const;
    template mach::ecs::components::Transform& mach::ecs::Entity::getComponent<mach::ecs::components::Transform>();

    template mach::ecs::components::ModelRenderer& Entity::addComponent<mach::ecs::components::ModelRenderer, mach::renderer::Model*, Shader*>(mach::renderer::Model*&&, Shader*&&);
    template mach::ecs::components::Transform& mach::ecs::Entity::addComponent<mach::ecs::components::Transform, glm::tvec3<float, (glm::precision)0>, glm::tvec3<float, (glm::precision)0>, glm::tvec3<float, (glm::precision)0>>(glm::tvec3<float, (glm::precision)0>&&, glm::tvec3<float, (glm::precision)0>&&, glm::tvec3<float, (glm::precision)0>&&);
    template bool mach::ecs::Entity::hasComponent<mach::ecs::components::ModelRenderer>() const;
    template mach::ecs::components::ModelRenderer& mach::ecs::Entity::getComponent<mach::ecs::components::ModelRenderer>();
}
