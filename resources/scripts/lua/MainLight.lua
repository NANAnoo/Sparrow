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
    MainLight:addComponent(DirectionalLight, glm.vec3(0.4, 0.4, 0.4), glm.vec3(1, 1, 1), glm.vec3(1, 1, 1))
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
    SubLight:addComponent(DirectionalLight, glm.vec3(0.4, 0.4, 0.4), glm.vec3(0, 0.5, 1), glm.vec3(0.0, 0.5, 1.0))
    lightTrans:setRotation(glm.vec3(30, -60, 0))

    local LightController = SubLight:addComponent(KeyEventHandler)

    LightController:setOnKeyHeld(
        function(entity, code)
            local transform = entity:getComponent(Transform)
            local rotation = transform:getRotation()
            if code == KeyCode.Left then
                rotation.y = rotation.y + 5
            end
            if code == KeyCode.Right then
                rotation.y = rotation.y - 5
            end
            if code == KeyCode.Up then
                rotation.x = rotation.x + 5
            end
            if code == KeyCode.Down then
                rotation.x = rotation.x - 5
            end
            transform:setRotation(rotation)
        end
        , scene
    )

    print("SubLight id : ", SubLight.id)
    return SubLight
end

function createPointLight(scene, color, position, index)
    local SubLight = scene:addEntity("SubLight")
    local lightTrans = SubLight:addComponent(Transform)
    SubLight:addComponent(PointLight, color, color, color, 0.1, 0.1, 1);
    lightTrans:setPosition(position)

    local LightController = SubLight:addComponent(KeyEventHandler)

    local current_index = 1;
    LightController:setOnKeyHeld(
        function(entity, code)
            -- set current_index as key 1, 2, ..
            if code == KeyCode.D1 then
                current_index = 1
            end
            if code == KeyCode.D2 then
                current_index = 2
            end
            if code == KeyCode.D3 then
                current_index = 3
            end
            if code == KeyCode.D4 then
                current_index = 4
            end

            if index ~= current_index then
                return
            end

            local transform = entity:getComponent(Transform)
            local position = transform:getPosition()
            if code == KeyCode.I then
                position.z = position.z + 0.01
            end
            if code == KeyCode.K then
                position.z = position.z - 0.01
            end
            if code == KeyCode.J then
                position.x = position.x - 0.01
            end
            if code == KeyCode.L then
                position.x = position.x + 0.01
            end
            transform:setPosition(position)
        end
        , scene
    )

    print("PointLight id : ", SubLight.id)
    return SubLight
end