//
// Created by Dquavius Griffin on 11/6/26.
//

#ifndef INTERACTABLE_H
#define INTERACTABLE_H
#include <string>

namespace mach::ecs::components
{
    struct Interactable
    {
        std::string prompt = "Interact";
        float range = 1.5f;
        bool enabled = true;
    };
}

#endif //INTERACTABLE_H
