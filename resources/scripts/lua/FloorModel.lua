require "Scene" 
require "Components/TransformComponent"
require "Components/MeshComponent"
require "Render/RenderDefines"
require "Render/ShaderDesc"

-- create model from ./resources/models/cube.obj
function CreateFloor(scene, camera_id)
    local floor = scene:addEntity("Floor")

    local trans = floor:addComponent(Transform)
    trans:setPosition(glm.vec3(0, -0.35, 0))
    trans:setRotation(glm.vec3(0, 0, 0))
    trans:setScale(glm.vec3(5.0, 0.05, 5.0))

    local model = floor:addComponent(MeshComponent, camera_id, "sand_cube")

    -- custom shader example
    local tiledPBRShader = ShaderDesc.new()
    local a_shader = SPW.ShaderHandle("tiledPBRShader", "./resources/shaders/simpleVs.vert", "./resources/shaders/pbrShadowTiled.frag")
    tiledPBRShader:setShader(a_shader)
    tiledPBRShader:setMatInput(MaterialUniform.AlbedoType, "albedoMap")
    tiledPBRShader:setMatInput(MaterialUniform.NormalType, "normalMap")
    tiledPBRShader:setMatInput(MaterialUniform.MetallicType, "metallicMap")
    tiledPBRShader:setMatInput(MaterialUniform.RoughnessType, "roughnessMap")
    tiledPBRShader:setMatInput(MaterialUniform.AOType, "AoMap")

    tiledPBRShader:setContextInput(ContextUniform.CameraPosition, "camPos")
    tiledPBRShader:setContextInput(ContextUniform.RandomNumber, "RandomSeed")

    tiledPBRShader:setLightInput(LightUniform.PointLight, "PLights")
    tiledPBRShader:setLightInput(LightUniform.DirectionalLight, "DLights")
    tiledPBRShader:setLightInput(LightUniform.DLightArraySize, "DLightCount")
    tiledPBRShader:setLightInput(LightUniform.PLightArraySize, "PLightCount")
    tiledPBRShader:setLightInput(LightUniform.DLightTransform, "lightSpaceMatrix")

    tiledPBRShader:setTransformInput(TransformUniform.M, "M")
    tiledPBRShader:setTransformInput(TransformUniform.V, "V")
    tiledPBRShader:setTransformInput(TransformUniform.P, "P")

    tiledPBRShader:setDependencyInput(
        SPW.AttachmentPort(scene:getRenderNodeID(DefaultNode.PointShadowNode), 0),
        "P_shadowMap"
    )

    tiledPBRShader:setDependencyInput(
        SPW.AttachmentPort(scene:getRenderNodeID(DefaultNode.DirectionalShadowNode), 0),
        "shadowMap"
    )

    scene:registerShader(tiledPBRShader)

    model:setGraphId(scene:getGraphID(DefaultGraph.PBRShadowGraph))
    model:setRenderProgram(scene:getRenderNodeID(DefaultNode.PBRShadowLightingNode), tiledPBRShader:getID())
    
    print("floor id : ", floor.id)
    
    return floor
end