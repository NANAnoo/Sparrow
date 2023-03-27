require "Scene" 
require "Components/TransformComponent"
require "Components/ModelComponent"

-- create model from ./resources/models/cube.obj
function CreateFloor(scene, camera_id)
    local floor = scene:addEntity("Floor")

    local trans = floor:addComponent(Transform)
    trans:setPosition(glm.vec3(0, -0.35, 0))
    trans:setRotation(glm.vec3(0, 0, 0))
    trans:setScale(glm.vec3(5.0, 0.05, 5.0))

    local model = floor:addComponent(Model, camera_id, "resources/models/cube.obj")

    local handle = SPW.ShaderHandle("main", "./resources/shaders/simpleVs.vert", "./resources/shaders/simplefrag.frag")
    model:setObjectHandle(handle)

    local shadow = SPW.ShaderHandle("shadow", "./resources/shaders/shadowMap.vert", "./resources/shaders/shadowMap.frag")
    model:setShadowHandle(shadow)

    print("floor id : ", floor.id)
    
    return floor
end