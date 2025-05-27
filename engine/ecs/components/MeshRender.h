//
// Created by Dquavius Griffin on 5/26/25.
//

#ifndef MESHRENDER_H
#define MESHRENDER_H
#include "../../rendering/primatives/Model.h"
#include "../../rendering/shader/Shader.h"

namespace mach::ecs::components
{
    struct ModelRenderer
    {
        mach::renderer::Model* model;
        Shader* shader;
    };
}

#endif //MESHRENDER_H
