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
            addPrintGlobal();
        }
        ~StateManager() = default;

        void addPrintGlobal()
        {
            lua["print"] = [](sol::variadic_args args) {
                std::ostringstream oss;
                oss << "[Script:Lua] ";

                for (auto arg : args) {
                    oss << arg.get<sol::object>().as<std::string>() << " ";
                }

                std::cout << oss.str() << std::endl;
            };
        }
    };
}

#endif //STATEMANAGER_H
