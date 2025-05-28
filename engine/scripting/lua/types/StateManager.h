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
            mach::scripting::lua::registerEntityTypes(lua);
        }
        ~StateManager() = default;
    };
}

#endif //STATEMANAGER_H
