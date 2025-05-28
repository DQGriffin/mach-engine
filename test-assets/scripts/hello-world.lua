function on_start(entity)
    print("Hello from on_start() in Lua")
    print("My name is " .. entity:get_name())
end

function on_update(entity, dt)
    local force = Vec3(0.01, 0.0, 0.0)
    translate(entity, force)
end