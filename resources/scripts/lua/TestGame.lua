require "Application"
require "Scene"
require "Entity"

local scene = {}

App({name="Test", width = 1200, height = 900,
    onInit = function ()
        scene = Scene.new("haha")
        local en = scene:addEntity("TestObj")
        print("Create new entity : ", en.id)
        print("Lua : onInit")
    end,
    beforeUpdate = function()
        scene:removeEntity()
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