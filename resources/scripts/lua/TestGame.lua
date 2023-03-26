require "Application"
require "Scene"
require "Entity"

require "MainCamera"
require "MonaModel"
require "MainLight"
require "FloorModel"

local scene = {}
local MainCamera = {}

App({name="Test", width = 1280, height = 720,
    onInit = function ()
        scene = Scene.new("LuaTestGame")
        MainCamera = CreateMainCamera(scene, 1280, 720)
        local main_camera_id = MainCamera.id
        local MonaModel = CreateMonaModel(scene, main_camera_id)
        local floor = CreateFloor(scene, main_camera_id)
        local MainLight = CreateMainLight(scene)
        local SubLight = CreateSubLight(scene)
        scene:initScene()
        local testcpp = scene:getEntityFromID("ada92a74-0000-4f3c-0000-d966f5af169e")
        local testTran = testcpp:getComponent(Transform)
        local testAu = testcpp:getComponent(AudioComponent)
        testAu:setState(testAu.audioFiles[1], AudioState.Play)
        testAu:set3D(testAu.audioFiles[1], true)
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
    end,
    onWindowResize = function(width, height)
        if width == 0 or height == 0 then
            return
        end
        local camera = MainCamera:getComponent(PerspectiveCamera)
        camera:setAspect(width / height)
    end
})