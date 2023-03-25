require('luaclass')

PerspectiveCamera = class('PerspectiveCamera', Component)

-- PerspectiveCamera init
function PerspectiveCamera:init(fov, aspect, near, far, is_main)
    self.fov = fov or 45
    self.aspect = aspect or 1
    self.near = near or 0.1
    self.far = far or 100
    self.is_main = is_main or false
end

-- setter and getters
function PerspectiveCamera:setFov(fov)
    self.fov = fov
    self:update("fov", fov)
end

function PerspectiveCamera:getFov()
    return self.fov
end

function PerspectiveCamera:setAspect(aspect)
    self.aspect = aspect
    self:update("aspect", aspect)
end

function PerspectiveCamera:getAspect()
    return self.aspect
end

function PerspectiveCamera:setNear(near)
    self.near = near
    self:update("near", near)
end

function PerspectiveCamera:getNear()
    return self.near
end

function PerspectiveCamera:setFar(far)
    self.far = far
    self:update("far", far)
end

function PerspectiveCamera:getFar()
    return self.far
end

function PerspectiveCamera:setIsMain(is_main)
    self.is_main = is_main
    self:update("is_main", is_main)
end

function PerspectiveCamera:getIsMain()
    return self.is_main
end