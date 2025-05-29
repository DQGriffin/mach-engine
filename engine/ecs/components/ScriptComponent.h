//
// Created by Dquavius Griffin on 5/27/25.
//

#ifndef SCRIPTCOMPONENT_H
#define SCRIPTCOMPONENT_H

#include <sol/sol.hpp>

namespace mach::ecs::components
{
    struct ScriptComponent {
        std::unique_ptr<sol::environment> env;
        std::unique_ptr<sol::function> on_start;
        std::unique_ptr<sol::function> on_update;
        std::unique_ptr<sol::function> on_keypress;
    };
}

#endif //SCRIPTCOMPONENT_H
