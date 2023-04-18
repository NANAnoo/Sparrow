require "Scene" 
require "Components/TransformComponent"
require "Components/ModelComponent"
require "Components/AudioComponent"
require "Components/KeyEventHandler"

-- create model from resources/models/mantis
function CreateMantisModel(scene, camera_id)
    local MantisModel = scene:addEntity("Model")
    local trans = MantisModel:addComponent(Transform)
    trans:setPosition(glm.vec3(0, -0.3, 0))
    trans:setRotation(glm.vec3(0, 0, 0))
    trans:setScale(glm.vec3(0.1, 0.1, 0.1))
    
    -- render model
    local model = MantisModel:addComponent(Model, camera_id, "resources/models/mantis/scene.gltf")
    local handle = SPW.ShaderHandle("pbrShadow", "./resources/shaders/simpleVs.vert", "./resources/shaders/pbrShadow.frag")
    model:setObjectHandle(handle)

    local shadow = SPW.ShaderHandle("shadow", "./resources/shaders/shadowMap.vert", "./resources/shaders/shadowMap.frag")
    model:setShadowHandle(shadow)

    print("Mantis Model id : ", MantisModel.id)
    
    return MantisModel
end