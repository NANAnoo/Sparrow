require('luaclass')
require('Component')

Model = class('Model', Component)

-- init only with a unchandled camera_id, store the camera_id in an array
function Model:init(camera_id, path, shadow_handle, object_handle)
    self.camera_id = camera_id
    self.path = path or nil
    self.shadow_handle = shadow_handle or nil
    self.object_handle = object_handle or nil
end


-- set model path : string, store the path in the component
function Model:setModelPath(path)
    self.path = path
    self:update('path', path)
end

-- set shadow handle : array of string, store the handle in the component   
function Model:setShadowHandle(shadow_handle)
    self.shadow_handle = shadow_handle
    self:update('shadow_handle', shadow_handle)
end

-- set object handle : array of string, store the handle in the component
function Model:setObjectHandle(object_handle)
    self.object_handle = object_handle
    self:update('object_handle', object_handle)
end

-- get model path : string, return the path stored in the component
function Model:getModelPath()
    return self.path
end