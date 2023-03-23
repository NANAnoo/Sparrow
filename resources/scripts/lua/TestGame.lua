require "Application"
require "Scene"
require "Entity"
require "TransformComponent"

local scene = {}
local id = nil
local i = 0

App({name="Test", width = 1200, height = 900,
    onInit = function ()
        scene = Scene.new("haha")
        local en = scene:addEntity("TestObj")
        local com = en:addComponent(Transform)
        com.position = glm.vec3(0, 1, 10)
        id = en.id
        print("Create new entity : ", en.id)
        print("Lua : onInit")
    end,
    beforeUpdate = function()
        local en = scene:getEntityFromID(id)
        if en then
            local com = en:getComponent(Transform)
            if com then
                com.rotation = glm.vec3(i, 0, 0)
                i = (i + 1) % 100;
            end
        end
        scene:update()
        --print("Lua : beforeUpdate")
    end ,
    onUpdate = function(duration)
        --print("Lua : onUpdate : ", duration)
    end,
    afterUpdate = function()
        --print("Lua : afterUpdate")
        collectgarbage()
    end,
    onStop = function()
        print("Lua : onStop")
    end
})