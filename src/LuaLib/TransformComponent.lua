require("luaclass")
require("Component")

Transform = class("Transform", Component)

function Transform:init(position, rotation, scale)
    self.position = position or glm.vec3(0, 0, 0)
    self.rotation = rotation or glm.vec3(0, 0, 0)
    self.scale = scale or glm.vec3(1, 1, 1)
end