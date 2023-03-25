require("luaclass")
require("Component")

Transform = class("Transform", Component)

function Transform:init(position, rotation, scale)
    self.position = position or glm.vec3(0, 0, 0)
    self.rotation = rotation or glm.vec3(0, 0, 0)
    self.scale = scale or glm.vec3(1, 1, 1)
end

function Transform:setPosition(position)
    self:update("position", position)
    self.position = position
end

function Transform:setRotation(rotation)
    self:update("rotation", rotation)
    self.rotation = rotation
end

function Transform:setScale(scale)
    self:update("scale", scale)
    self.scale = scale
end

function Transform:getPosition()
    return self.position
end

function Transform:getRotation()
    return self.rotation
end

function Transform:getScale()
    return self.scale
end