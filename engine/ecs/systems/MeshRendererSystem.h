//
// Created by Dquavius Griffin on 5/26/25.
//

#ifndef MESHRENDERERSYSTEM_H
#define MESHRENDERERSYSTEM_H

#include "../../external/entt.h"
#include "../components/Transform.h"
#include "../components/MeshRender.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace mach::ecs::systems
{
    inline void render(entt::registry& registry)
    {
        auto view = registry.view<components::Transform, components::ModelRenderer>();

        for (auto entity : view)
        {
            auto& transform = view.get<components::Transform>(entity);
            auto& meshRenderer = view.get<components::ModelRenderer>(entity);

            glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.position);
            model = glm::rotate(model, transform.rotation.x, glm::vec3(1, 0, 0));
            model = glm::rotate(model, transform.rotation.y, glm::vec3(0, 1, 0));
            model = glm::rotate(model, transform.rotation.z, glm::vec3(0, 0, 1));
            model = glm::scale(model, transform.scale);

            int modelLoc = glGetUniformLocation(meshRenderer.shader->ID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            meshRenderer.model->Draw(*meshRenderer.shader);
        }
    }
}

#endif //MESHRENDERERSYSTEM_H
