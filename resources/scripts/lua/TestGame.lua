require "Application"
require "Scene"
require "Entity"

require "MainCamera"
require "MonaModel"
require "MantisModel"
require "MainLight"
require "FloorModel"
require "SkyBox"
require "ThirdPersonController"

local scene = {}    
local MainCamera = {}
local model = {}
App({name="Test", width = 1280, height = 720,
    onInit = function ()
        global.app:loadAsset("mantis/mantis.json")
        global.app:loadAsset("skybox/skybox.json")
        global.app:loadAsset("sand_cube/sand_cube.json")
        scene = Scene.new("LuaTestGame")
        MainCamera = CreateCamera(scene, 1280, 720)
        ThirdPersonController(scene,MainCamera,MainCamera,0.005,true)
        local main_camera_id = MainCamera.id
        model = CreateModel(scene,MainCamera, main_camera_id)
        
        CreateSkyBox(scene, main_camera_id)
        local floor = CreateFloor(scene, main_camera_id)
        local MainLight = CreateMainLight(scene)
        local SubLight = CreateSubLight(scene)
        createPointLight(scene, glm.vec3(0, 1, 1), glm.vec3(0.5, -0.5, 0), 1)
        createPointLight(scene, glm.vec3(1, 0, 1), glm.vec3(-0.5, 0.5, 0), 2)
        scene:initScene()
    end,
    beforeUpdate = function()
        TPSPlayer(scene,MainCamera.id,model,MainCamera,0.08,0.4)
        scene:beforeUpdate()
    end ,
    onUpdate = function(duration)
        scene:onUpdate(duration)
    end,
    afterUpdate = function()
        scene:afterUpdate()
        collectgarbage()
    end,
    onStop = function()
        scene:onStop()
        print("Lua : onStop")
    end,
    onWindowResize = function(width, height)
        if width == 0 or height == 0 then
            return
        end
        local camera = MainCamera:getComponent(PerspectiveCamera)
        camera:setAspect(width / height)
    end
})

function ThirdPersonController(scene,entity,camera,speed,isCamera)
    local keyContoller = entity:addComponent(KeyEventHandler)
    local mouseController = entity:addComponent(MouseEventHandler)

    local speed_up = 1
    local velocity_dir = -1
    if isCamera then 
        velocity_dir = 1
    end
        
    local maxXRotation = 20
    local minXRotation = -45


    keyContoller:setOnKeyHeld(
        function(entity, code)
            local transform = entity:getComponent(Transform)
            local cam_transform = camera:getComponent(Transform)
            local cam_rotation = cam_transform:getRotation()
            local rotation = transform:getRotation()
            local rotMat = glm.eulerAngleYXZ(glm.radians(rotation.y) + glm.radians(cam_rotation.y), glm.radians(rotation.x)+ glm.radians(cam_rotation.x), glm.radians(rotation.z)+glm.radians(cam_rotation.z))
            
            local forward = glm.toVec3(rotMat * glm.vec4(0, 0, -1, 0))
            local up = glm.vec3(0, 1, 0)
            local right = glm.cross(forward, up)

            print("forward",forward)
            if code == KeyCode.W  and isCamera == false then
                transform:translate(forward * speed * speed_up * velocity_dir)  
            elseif code == KeyCode.S and isCamera == false then
                transform:translate(forward * -speed * speed_up* velocity_dir)
            elseif code == KeyCode.A and isCamera == false then
                transform:translate(right * -speed * speed_up* velocity_dir)
            elseif code == KeyCode.D and isCamera == false then
                transform:translate( right * speed * speed_up* velocity_dir)
            elseif code == KeyCode.Q then
                if isCamera then
                    --transform:translate(up * -speed * speed_up)
                end
            elseif code == KeyCode.E then
                if isCamera then
                    --transform:translate(up * speed * speed_up)
                end
            end
        end
    , scene
    )

    keyContoller:setOnKeyDown(
        function(entity, code)
            if code == KeyCode.LeftShift then
                speed_up = 10
            end
        end
    , scene
    )

    keyContoller:setOnKeyUp(
        function(entity, code)
            if code == KeyCode.LeftShift then
                speed_up = 1
            end
        end
    , scene
    )

    mouseController:setOnMove(
            function(entity, x, y, dx, dy)
                local transform = entity:getComponent(Transform)
                local rotation = transform:getRotation()
                local position = transform:getPosition()
                if isCamera then
                    --rotation.x = rotation.x - dy * 0.1
                    --rotation.y = rotation.y - dx * 0.1
                    position.x = position.x + dx *0.001
                    position.z = position.z + dy *0.001
                    --position.y = position.y - dy * 0.001

                else
                    --rotation.y = rotation.y - dx * 0.1
                end
                transform:setPosition(position)
            end
            , scene
        )
end


function CreateCamera(scene, width, height)
    local MainCamera = scene:addEntity("MainCamera")
    local trans = MainCamera:addComponent(Transform)
    trans:setPosition(glm.vec3(0, 0.3, 1))
    trans:setRotation(glm.vec3(0, 0, 0))
    trans:setScale(glm.vec3(1, 1, 1))
    MainCamera:addComponent(PerspectiveCamera, 60, width / height, 0.1, 100, true)

    MainCamera:addComponent(AudioListener, 0)
    
    print("MainCamera id : ", MainCamera.id)
    
    return MainCamera
end

function CreateModel(scene,camera, camera_id)
    local MantisModel = scene:addEntity("Model")
    local trans = MantisModel:addComponent(Transform)

    trans:setPosition(glm.vec3(0, -0.3, 0))
    trans:setRotation(glm.vec3(0, 180, 0))
    trans:setScale(glm.vec3(0.05, 0.05, 0.05))
    ThirdPersonController(scene,MantisModel,camera,0.005,false)
    -- render model
    local model = MantisModel:addComponent(MeshComponent, camera_id, "mantis")
    model:setGraphId(scene:getGraphID(DefaultGraph.kDefferShadingGraph))
    model:setRenderProgram(scene:getRenderNodeID(DefaultNode.kPointShadowNode), scene:getShaderID(DefaultShader.kPointShadowShader))
    model:setRenderProgram(scene:getRenderNodeID(DefaultNode.kDirectionalShadowNode), scene:getShaderID(DefaultShader.kDirectionalShadowShader))
    model:setRenderProgram(scene:getRenderNodeID(DefaultNode.kGBufferNode), scene:getShaderID(DefaultShader.kGBufferShader))

    print("Mantis Model id : ", MantisModel.id)

    return MantisModel
end

function TPSPlayer(scene,camera_id,model,camera,height,radius)
    --camera part
    local trans_camera = camera:getComponent(Transform)

    --model part
    local trans_model = model:getComponent(Transform)

    --set position
    local cam_pos = trans_camera:getPosition()
    local model_pos = trans_model:getPosition()
    local new_model_pos = glm.vec3(model_pos.x,model_pos.y + height,model_pos.z)
    local dis = new_model_pos - cam_pos
    local normalise_dis = glm.normalize(dis)
    local length = math.sqrt(dis.x * dis.x + dis.z * dis.z) --radius

    local target_len = length - radius;
    local target_pos = normalise_dis * target_len;
    target_pos = cam_pos + target_pos

    trans_camera:setPosition(target_pos)


    local model_temp_pos = glm.vec3(new_model_pos.x,new_model_pos.y ,new_model_pos.z)
    
    --setRotation to a certain horizontal level
    local cam_lookat = glm.vec3(0,0,-1)
    local target_dir = model_temp_pos - cam_pos
    target_dir = glm.normalize(target_dir)
    
    local rotation_axis = glm.cross(cam_lookat,target_dir)
    local rotation_angle = glm.dot(cam_lookat,target_dir)
    
    rotation_axis = glm.normalize(rotation_axis)
    
    local euler_rotation_angle = math.acos( rotation_angle )

   

    --setRotation to model center
    local cam_lookat_new = glm.normalize(model_temp_pos - cam_pos)
    local new_target = glm.vec3(model_pos.x,model_pos.y + height,model_pos.z)
    local target_dir_new = glm.normalize(new_target - cam_pos)
    
    local rotation_axis_new = glm.cross(cam_lookat_new,target_dir_new)
    local rotation_angle_new = glm.dot(cam_lookat_new,target_dir_new)
    
    rotation_axis_new = glm.normalize(rotation_axis_new)
    
    local euler_rotation_angle_new = math.acos( rotation_angle_new )

    local rotate_x = euler_rotation_angle_new * 180 / 3.1415926
    local rotate_y = euler_rotation_angle * 180 / 3.1415926

    if rotate_x > 45 then
        rotate_x = 45
    end
        

    if rotation_axis.y > 0  then
        trans_camera:setRotation(glm.vec3( 0,rotate_y,0))
    end
    if rotation_axis.y < 0 then
        trans_camera:setRotation(glm.vec3( 0,-rotate_y,0))
    end

end
