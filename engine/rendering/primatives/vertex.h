//
// Created by Dquavius Griffin on 5/26/25.
//

#ifndef VERTEX_H
#define VERTEX_H
#include "../../math/vec.h"

namespace mach::renderer
{
    struct Vertex
    {
        mach::vec3 Position;
        mach::vec3 Normal;
        mach::vec2 TexCoords;
    };
}

#endif //VERTEX_H
