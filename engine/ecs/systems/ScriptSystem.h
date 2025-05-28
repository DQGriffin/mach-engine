//
// Created by Dquavius Griffin on 5/27/25.
//

#ifndef SCRIPTSYSTEM_H
#define SCRIPTSYSTEM_H

#include "../components/ScriptComponent.h"
#include <string>
#include <sol/sol.hpp>

namespace mach::ecs::systems
{
    using namespace mach::ecs;

    inline void attachScriptToEntity(entt::registry& registry, mach::ecs::Entity& entity, const std::string& path, sol::state& lua)
    {
        auto& scriptComponent = registry.get<components::ScriptComponent>(entity.handle());

        scriptComponent.env = std::make_unique<sol::environment>(lua, sol::create, lua.globals());
        lua.script_file(path, *scriptComponent.env);

        scriptComponent.on_start = std::make_unique<sol::function>((*scriptComponent.env)["on_start"]);
        scriptComponent.on_update = std::make_unique<sol::function>((*scriptComponent.env)["on_update"]);

        if (scriptComponent.on_start->valid())
        {
            LOG_DEBUG("Invoking on_start script")
            (*scriptComponent.on_start)(entity);
        }
        else
        {
            LOG_DEBUG("on_start script is not valid")
        }
    }

    inline void callUpdateScrips(entt::registry& registry, float deltaTime)
    {
        auto view = registry.view<components::ScriptComponent>();

        for (auto entity : view)
        {
            auto& scriptComponent = view.get<components::ScriptComponent>(entity);
            if (scriptComponent.on_update->valid())
            {
                (*scriptComponent.on_update)(entity, deltaTime);
            }
        }
    }
}

#endif //SCRIPTSYSTEM_H
