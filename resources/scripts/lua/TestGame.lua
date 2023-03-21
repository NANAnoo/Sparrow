require "Application"
require "Scene"
require "Entity"

local scene = {}

App({name="Test", width = 1200, height = 900},
    function ()
        scene = Scene.new("haha")
        local en = scene:addEntity("TestObj")
        print("Create new entity : ", en.id)
        print("Lua : onInit")
    end,
    function()
        scene:removeEntity()
        --print("Lua : beforeUpdate")
    end ,
    function(duration)
        --print("Lua : onUpdate : ", duration)
    end,
    function()
        --print("Lua : afterUpdate")
        collectgarbage()
    end,
    function()
        print("Lua : onStop")
    end
)