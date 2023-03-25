require "luaclass"
require "Scene" 
require "Entity"
require "TransformComponent"
require "DirectionalLight"

function CreateMainLight(scene)
    local MainLight = scene:addEntity("MainLight")
    local lightTrans = MainLight:addComponent(Transform)
    MainLight:addComponent(DirectionalLight, glm.vec3(0.2, 0.2, 0.2), glm.vec3(1, 1, 0), glm.vec3(1.0, 1.0, 0.0))

    lightTrans:setRotation(glm.vec3(0, 60, 0))

    print("MainLight id : ", MainLight.id)
    return MainLight
end