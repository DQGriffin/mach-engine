//
// Created by Dquavius Griffin on 5/26/25.
//

#ifndef ENTITY_H
#define ENTITY_H
#include "../external/entt.h"
#include "Scene.h"

namespace mach::ecs
{
    class Scene;

    class Entity {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* scene);

        template<typename T, typename... Args>
    T& addComponent(Args&&... args);

        template<typename T>
        bool hasComponent() const;

        template<typename T>
        T& getComponent();

        entt::entity handle() const { return m_Entity; }
        bool isValid() const;

        std::string getName();
        glm::vec3 getPosition();
        void setPosition(const glm::vec3& position);
        void applyForce(const glm::vec3& force);

    private:
        entt::entity m_Entity = entt::null;
        Scene* m_Scene = nullptr;
    };
}

#endif //ENTITY_H
