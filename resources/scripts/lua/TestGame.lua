require "Application"
require "Scene"
require "Entity"

App({name="Test", width = 1200, height = 900},
    function ()
        print("Lua : onInit")
    end,
    function()
        print("Lua : beforeUpdate")
    end ,
    function(duration)
        print("Lua : onUpdate : ", duration)
    end,
    function()
        print("Lua : afterUpdate")
        collectgarbage()
    end,
    function()
        print("Lua : onStop")
    end
)