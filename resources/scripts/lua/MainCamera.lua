require "luaclass"
require "PerspectiveCamera"
require "Scene" 
require "Entity"
require "TransformComponent"

function CreateMainCamera(scene, width, height)
    local MainCamera = scene:addEntity("MainCamera")
    MainCamera:addComponent(Transform)
    MainCamera:addComponent(PerspectiveCamera, 60, width / height, 0.1, 100, true)
    print("MainCamera id : ", MainCamera.id)
    return MainCamera
end