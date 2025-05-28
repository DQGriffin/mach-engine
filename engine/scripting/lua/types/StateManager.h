//
// Created by Dquavius Griffin on 5/27/25.
//

#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <sol/sol.hpp>

namespace mach::scripting::lua
{
    class StateManager
    {
    public:
        sol::state lua;

    public:
        StateManager()
        {
            lua.open_libraries(sol::lib::base);
        }
        ~StateManager() = default;

        void registerTypes()
        {
            // lua.new_usertype<mach::ecs::Entity>("Entity",
            //     "get_name", &mach::ecs::Entity::getName
            // );
        }
    };
}

#endif //STATEMANAGER_H
