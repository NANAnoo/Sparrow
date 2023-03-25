require "Scene" 
require "TransformComponent"
require "ModelComponent"

function CreateMonaModel(scene, camera_id)
    local MonaModel = scene:addEntity("MonaModel")
    local trans = MonaModel:addComponent(Transform)
    trans:setPosition(glm.vec3(0, -0.3, 0))
    trans:setRotation(glm.vec3(-90, 90, 0))
    trans:setScale(glm.vec3(0.5, 0.5, 0.5))
    MonaModel:addComponent(Model, camera_id, "resources/models/mona2/mona.fbx")
    
    local model = MonaModel:getComponent(Model)
    local handle = SPW.ShaderHandle("main", "./resources/shaders/simpleVs.vert", "./resources/shaders/simplefrag.frag")
    model:setObjectHandle(handle)

    local shadow = SPW.ShaderHandle("shadow", "./resources/shaders/shadowMap.vert", "./resources/shaders/shadowMap.frag")
    model:setShadowHandle(shadow)

    print("MonaModel id : ", MonaModel.id)
    
    return MonaModel
end