require "Scene" 
require "Components/TransformComponent"
require "Components/MeshComponent"
require "Components/AudioComponent"
require "Components/KeyEventHandler"

require "Render/RenderDefines"

-- create model from resources/models/mantis
function CreateMantisModel(scene, camera_id)
    local MantisModel = scene:addEntity("Model")
    local trans = MantisModel:addComponent(Transform)
    trans:setPosition(glm.vec3(0, -0.3, 0))
    trans:setRotation(glm.vec3(0, 0, 0))
    trans:setScale(glm.vec3(0.1, 0.1, 0.1))
    
    -- render model
    local model = MantisModel:addComponent(MeshComponent, camera_id, "mantis")
    model:setGraphId(scene:getGraphID(DefaultGraph.kDefferShadingGraph))
    model:setRenderProgram(scene:getRenderNodeID(DefaultNode.kPointShadowNode), scene:getShaderID(DefaultShader.kPointShadowShader))
    model:setRenderProgram(scene:getRenderNodeID(DefaultNode.kDirectionalShadowNode), scene:getShaderID(DefaultShader.kDirectionalShadowShader))
    model:setRenderProgram(scene:getRenderNodeID(DefaultNode.kGBufferNode), scene:getShaderID(DefaultShader.kGBufferShader))

    print("Mantis Model id : ", MantisModel.id)
    
    return MantisModel
end