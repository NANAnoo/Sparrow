require "Application"
require "Scene"
require "Entity"

require "MainCamera"
require "MonaModel"
require "MainLight"
require "FloorModel"

local scene = {}

App({name="Test", width = 1280, height = 720,
    onInit = function ()
        scene = Scene.new("LuaTestGame")
        local MainCamera = CreateMainCamera(scene, 1280, 720)
        local main_camera_id = MainCamera.id
        local MonaModel = CreateMonaModel(scene, main_camera_id)
        local floor = CreateFloor(scene, main_camera_id)
        local MainLight = CreateMainLight(scene)
        local SubLight = CreateSubLight(scene)
        scene:initScene()
    end,
    beforeUpdate = function()
        scene:beforeUpdate()
    end ,
    onUpdate = function(duration)
        scene:onUpdate(duration)
    end,
    afterUpdate = function()
        scene:afterUpdate()
        collectgarbage()
    end,
    onStop = function()
        scene:onStop()
        print("Lua : onStop")
    end
})