require "Scene" 
require "Components/TransformComponent"
require "Components/MeshComponent"

require "Render/RenderDefines"

function CreateSkyBox(scene, camera_id)
    local MantisModel = scene:addEntity("skybox")
    MantisModel:addComponent(Transform)
    
    -- render model
    local model = MantisModel:addComponent(MeshComponent, camera_id, "skybox")
    model:setGraphId(scene:getGraphID(DefaultGraph.SkyBoxGraph))
    model:setRenderProgram(scene:getRenderNodeID(DefaultNode.SkyBoxNode), scene:getShaderID(DefaultShader.kSkyBoxShader))

    print("Mantis Model id : ", MantisModel.id)
    
    return MantisModel
end