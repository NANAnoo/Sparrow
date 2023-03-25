require "luaclass"
require "PerspectiveCamera"
require "Scene" 
require "Entity"
require "Components/TransformComponent"
require "Components/MouseEventHandler"
require "Components/KeyEventHandler"
require "Codes/KeyCode"
require "Codes/MouseCode"

function bindFPSController(scene, entity, speed)
    local keyContoller = entity:addComponent(KeyEventHandler)
    local mouseController = entity:addComponent(MouseEventHandler)

    keyContoller:setOnKeyHeld(
        function(entity, code)
            local transform = entity:getComponent(Transform)
            local rotation = transform:getRotation()
            local rotMat = glm.eulerAngleYXZ(glm.radians(rotation.y), glm.radians(rotation.x), glm.radians(rotation.z))
            local forward = glm.toVec3(rotMat * glm.vec4(0, 0, -1, 0))
            local up = glm.vec3(0, 1, 0)
            local right = glm.cross(forward, up)
            
            if code == KeyCode.W then
                transform:translate(forward * speed)
            elseif code == KeyCode.S then
                transform:translate(forward * -speed)
            elseif code == KeyCode.A then
                transform:translate(right * -speed)
            elseif code == KeyCode.D then
                transform:translate(right * speed)
            elseif code == KeyCode.Q then
                transform:translate(up * -speed)
            elseif code == KeyCode.E then
                transform:translate(up * speed)
            end
        end
        , scene
    )

    mouseController:setOnMove(
        function(entity, x, y, dx, dy)
            local transform = entity:getComponent(Transform)
            local rotation = transform:getRotation()
            rotation.x = rotation.x - dy * 0.1
            rotation.y = rotation.y - dx * 0.1
            transform:setRotation(rotation)
        end
        , scene
    )
end

function CreateMainCamera(scene, width, height)
    local MainCamera = scene:addEntity("MainCamera")
    MainCamera:addComponent(Transform)
    MainCamera:addComponent(PerspectiveCamera, 60, width / height, 0.1, 100, true)
    bindFPSController(scene, MainCamera, 0.01)
    print("MainCamera id : ", MainCamera.id)
    return MainCamera
end