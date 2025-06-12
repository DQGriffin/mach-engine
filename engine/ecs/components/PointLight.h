//
// Created by Dquavius Griffin on 6/10/25.
//

#ifndef POINTLIGHT_H
#define POINTLIGHT_H
#include <glm/glm.hpp>

namespace mach::ecs::components
{
    struct PointLight
    {
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.35f);
        glm::vec3 specularColor = lightColor * glm::vec3(1.0f);
        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
    };
}

#endif //POINTLIGHT_H
