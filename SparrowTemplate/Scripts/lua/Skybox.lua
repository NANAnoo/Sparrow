require "Scene" 
require "Components/TransformComponent"
require "Components/MeshComponent"

require "Render/RenderDefines"

function CreateSkyBox(scene, camera_id)
    local skybox = scene:addEntity("skybox")
    skybox:addComponent(Transform)
    
    -- render model
    local model = skybox:addComponent(MeshComponent, camera_id, "skybox")
    model:setGraphId(scene:getGraphID(DefaultGraph.SkyBoxGraph))
    model:setRenderProgram(scene:getRenderNodeID(DefaultNode.SkyBoxNode), scene:getShaderID(DefaultShader.kSkyBoxShader))

    print("skybox Model id : ", skybox.id)
    
    return skybox
end