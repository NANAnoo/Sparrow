require "Scene" 
require "Components/TransformComponent"
require "Components/MeshComponent"
require "Components/AudioComponent"
require "Components/KeyEventHandler"
require "Components/AnimationComponent"
require "Components/PhysicalComponent"

require "Render/RenderDefines"

-- create model from resources/models/mantis
function CreateMantisModel(scene, camera_id)
    local MantisModel = scene:addEntity("Model")
    local trans = MantisModel:addComponent(Transform)
    trans:setPosition(glm.vec3(0, 50, 0))
    trans:setRotation(glm.vec3(0, 0, 0))
    trans:setScale(glm.vec3(0.01, 0.01, 0.01))

    local rigid = MantisModel:addComponent(RigidDynamicComponent,RigidState.Awake)
    --rigid:setState(RigidState.Sleep)

    --local sphereCollider = MantisModel:addComponent(SphereCollider,ColliderState.needAwake,false,0.5)
    --print(sphereCollider.state,sphereCollider.is_trigger_,sphereCollider.sphere_radius_)
    local boxCollider = MantisModel:addComponent(BoxCollider,ColliderState.needAwake,false,glm.vec3(1, 0.001, 1))
    --local sphereCollider = MantisModel:addComponent(SphereCollider,ColliderState.needAwake,false,0.1)
    --local capsuleCollider = MantisModel:addComponent(CapsuleCollider,ColliderState.needAwake,false,0.01,0.01,glm.vec3(0,1,0),90)

    --print(boxCollider.state_,boxCollider.is_trigger_,boxCollider.box_size_)
    --SphereCollider(ColliderState.awake)


    -- render model
    local model = MantisModel:addComponent(MeshComponent, camera_id, "mantis")
    model:setGraphId(scene:getGraphID(DefaultGraph.kDefferShadingGraph))
    model:setRenderProgram(scene:getRenderNodeID(DefaultNode.kPointShadowNode), scene:getShaderID(DefaultShader.kAniPointShadowShader))
    model:setRenderProgram(scene:getRenderNodeID(DefaultNode.kDirectionalShadowNode), scene:getShaderID(DefaultShader.kAniDirectionalShadowShader))
    model:setRenderProgram(scene:getRenderNodeID(DefaultNode.kGBufferNode), scene:getShaderID(DefaultShader.kAniGBufferShader))

    --render animation
    local animation = MantisModel:addComponent(AnimationComponent,"mantis")
    animation:setState(2)
    animation:setLoop(true)
    animation:swapAnimation("mantis_anim")
    print("Mantis Model id : ", MantisModel.id)

    print("Mantis animation state.", animation.onGoingAnimState)
    print("Mantis animation name",animation.onGoingAnim)
    print("Mantis animation state.",animation.onGoingAnimState)
    return MantisModel
    end