require "Scene" 
require "Components/TransformComponent"
require "Components/MeshComponent"
require "Components/AudioComponent"
require "Components/KeyEventHandler"
require "Components/PhysicalComponent"

require "Render/RenderDefines"

-- create model from resources/models/mantis
function CreateMantisModel(scene, camera_id)
    local MantisModel = scene:addEntity("Model")
    local trans = MantisModel:addComponent(Transform)
    trans:setPosition(glm.vec3(0, 10, 0))
    trans:setRotation(glm.vec3(0, 0, 0))
    trans:setScale(glm.vec3(0.1, 0.1, 0.1))

    local rigid = MantisModel:addComponent(RigidDynamicComponent,RigidState.Awake)
    --rigid:setState(RigidState.Sleep)

    --local sphereCollider = MantisModel:addComponent(SphereCollider,ColliderState.needAwake,false,0.5)
    --print(sphereCollider.state,sphereCollider.is_trigger_,sphereCollider.sphere_radius_)
    --local boxCollider = MantisModel:addComponent(BoxCollider,ColliderState.needAwake,false,glm.vec3(0.1, 0.1, 0.1))
    --local sphereCollider = MantisModel:addComponent(SphereCollider,ColliderState.needAwake,false,0.1)
    local capsuleCollider = MantisModel:addComponent(CapsuleCollider,ColliderState.needAwake,false,0.4,1.9,glm.vec3(0,0,1),30)

    --print(boxCollider.state_,boxCollider.is_trigger_,boxCollider.box_size_)
    --SphereCollider(ColliderState.awake)


    -- render model
    local model = MantisModel:addComponent(MeshComponent, camera_id, "resources/models/mantis/scene.gltf")
    model:setGraphId(scene:getGraphID(DefaultGraph.PBRShadowGraph))
    model:setRenderProgram(scene:getRenderNodeID(DefaultNode.PointShadowNode), scene:getShaderID(DefaultShader.PointShadowShader))
    model:setRenderProgram(scene:getRenderNodeID(DefaultNode.DirectionalShadowNode), scene:getShaderID(DefaultShader.DirectionalShadowShader))
    model:setRenderProgram(scene:getRenderNodeID(DefaultNode.PBRShadowLightingNode), scene:getShaderID(DefaultShader.PBRShadowShader))

    print("Mantis Model id : ", MantisModel.id)

    return MantisModel
    end