//
// Created by Dquavius Griffin on 5/26/25.
//

#ifndef TEXTURE_H
#define TEXTURE_H
#include <string>

namespace mach::renderer
{
    struct Texture
    {
        unsigned int id;
        std::string type;
        std::string path;
    };
}

#endif //TEXTURE_H
