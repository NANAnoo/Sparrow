require "luaclass"
require "Scene" 
require "Entity"
require "Components/TransformComponent"
require "Components/DirectionalLight"
require "Components/PointLight"
require "Components/KeyEventHandler"

function CreateMainLight(scene)
    local MainLight = scene:addEntity("MainLight")
    local lightTrans = MainLight:addComponent(Transform)
    MainLight:addComponent(DirectionalLight, glm.vec3(1, 1, 1), glm.vec3(1, 1, 0), glm.vec3(1.0, 1.0, 0.0))
    lightTrans:setRotation(glm.vec3(30, 60, 0))

    local LightController = MainLight:addComponent(KeyEventHandler)

    LightController:setOnKeyHeld(
        function(entity, code)
            local transform = entity:getComponent(Transform)
            local rotation = transform:getRotation()
            if code == KeyCode.R then
                rotation.y = rotation.y + 5
            end
            transform:setRotation(rotation)
        end
        , scene
    )

    print("MainLight id : ", MainLight.id)
    return MainLight
end

function CreateSubLight(scene)
    local SubLight = scene:addEntity("SubLight")
    local lightTrans = SubLight:addComponent(Transform)
    SubLight:addComponent(PointLight, glm.vec3(0.2, 0.2, 0.2), glm.vec3(0, 1, 0), glm.vec3(0.0, 1.0, 0.0), 0.1, 1, 0.1)

    lightTrans:setPosition(glm.vec3(0, 0, 0.5))

    print("SubLight id : ", SubLight.id)
    return SubLight
end