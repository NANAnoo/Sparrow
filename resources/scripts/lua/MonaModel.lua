require "Scene" 
require "Components/TransformComponent"
require "Components/ModelComponent"
require "Components/AudioComponent"
require "Components/KeyEventHandler"

-- create model from resources/models/mona2/mona.fbx
function CreateMonaModel(scene, camera_id)
    local MonaModel = scene:addEntity("MonaModel")
    local trans = MonaModel:addComponent(Transform)
    trans:setPosition(glm.vec3(0, -0.3, 0))
    trans:setRotation(glm.vec3(-90, 90, 0))
    trans:setScale(glm.vec3(0.5, 0.5, 0.5))
    
    -- render model
    local model = MonaModel:addComponent(Model, camera_id, "resources/models/mona2/mona.fbx")
    local handle = SPW.ShaderHandle("main", "./resources/shaders/simpleVs.vert", "./resources/shaders/simplefrag.frag")
    model:setObjectHandle(handle)

    local shadow = SPW.ShaderHandle("shadow", "./resources/shaders/shadowMap.vert", "./resources/shaders/shadowMap.frag")
    model:setShadowHandle(shadow)

    -- add audio component
    local audio = MonaModel:addComponent(AudioComponent, { "resources/sounds/test.wav" })
    audio:setLoop("resources/sounds/test.wav", true)
    audio:set3D("resources/sounds/test.wav", true)
    audio:setState("resources/sounds/test.wav", AudioState.Play)

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
        end
        , scene
    )

    print("MonaModel id : ", MonaModel.id)
    
    return MonaModel
end