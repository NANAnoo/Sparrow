require "Scene" 
require "Components/TransformComponent"
require "Components/MeshComponent"
require "Components/AudioComponent"
require "Components/KeyEventHandler"

-- create model from resources/models/mona2/mona.fbx
function CreateMonaModel(scene, camera_id)
    local MonaModel = scene:addEntity("MonaModel")
    local trans = MonaModel:addComponent(Transform)
    trans:setPosition(glm.vec3(1, -0.3, 0))
    trans:setRotation(glm.vec3(-90, 90, 90))
    trans:setScale(glm.vec3(0.5, 0.5, 0.5))
    
    -- render model
    local model = MonaModel:addComponent(MeshComponent, camera_id, "mona")
    model:setGraphId(scene:getGraphID(DefaultGraph.kDefferShadingGraph))
    model:setRenderProgram(scene:getRenderNodeID(DefaultNode.kPointShadowNode), scene:getShaderID(DefaultShader.kPointShadowShader))
    model:setRenderProgram(scene:getRenderNodeID(DefaultNode.kDirectionalShadowNode), scene:getShaderID(DefaultShader.kDirectionalShadowShader))
    model:setRenderProgram(scene:getRenderNodeID(DefaultNode.kGBufferNode), scene:getShaderID(DefaultShader.kGBufferShader))

    -- add audio component
    local audio = MonaModel:addComponent(AudioComponent, { "resources/sounds/test.wav" })
    audio:setLoop("resources/sounds/test.wav", true)
    audio:set3D("resources/sounds/test.wav", true)
    audio:setState("resources/sounds/test.wav", AudioState.Play)
    audio:setVolume("resources/sounds/test.wav", 0.3)

    -- add key event handler
    local keyHandler = MonaModel:addComponent(KeyEventHandler)
    keyHandler:setOnKeyDown(
        function(entity, code)
            if code == KeyCode.Space then
                local audio = entity:getComponent(AudioComponent)
                local oldState = audio:getState("resources/sounds/test.wav")
                if oldState == AudioState.Play or oldState == AudioState.Continue then
                    audio:setState("resources/sounds/test.wav", AudioState.Pause)
                else
                    audio:setState("resources/sounds/test.wav", AudioState.Continue)
                end
            end

            if code == KeyCode.PageUp then
                local audio = entity:getComponent(AudioComponent)
                local volume = audio:getVolume("resources/sounds/test.wav")
                audio:setVolume("resources/sounds/test.wav", volume + 0.1)
            end

            if code == KeyCode.PageDown then
                local audio = entity:getComponent(AudioComponent)
                local volume = audio:getVolume("resources/sounds/test.wav")
                audio:setVolume("resources/sounds/test.wav", volume - 0.1)
            end
        end
        , scene
    )

    print("MonaModel id : ", MonaModel.id)
    
    return MonaModel
end