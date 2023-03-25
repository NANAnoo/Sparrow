require "luaclass"
require "Component"

PointLight = class("PointLight", Component)

-- init
function PointLight:init(ambient, diffuse, specular, constant, linear, quadratic)
    self.ambient = ambient or {0.2, 0.2, 0.2}
    self.diffuse = diffuse or {1, 1, 1}
    self.specular = specular or {1.0, 1.0, 1.0}
    self.constant = constant or 1.0
    self.linear = linear    or 0.09
    self.quadratic = quadratic or 0.032
end

-- setters and getters
function PointLight:setAmbient(ambient)
    self.ambient = ambient
    self:update('ambient', ambient)
end

function PointLight:setDiffuse(diffuse)
    self.diffuse = diffuse
    self:update('diffuse', diffuse)
end

function PointLight:setSpecular(specular)
    self.specular = specular
    self:update('specular', specular)
end

function PointLight:setConstant(constant)
    self.constant = constant
    self:update('constant', constant)
end

function PointLight:setLinear(linear)
    self.linear = linear
    self:update('linear', linear)
end

function PointLight:setQuadratic(quadratic)
    self.quadratic = quadratic
    self:update('quadratic', quadratic)
end

function PointLight:getAmbient()
    return self.ambient
end

function PointLight:getDiffuse()
    return self.diffuse
end

function PointLight:getSpecular()
    return self.specular
end

function PointLight:getConstant()
    return self.constant
end

function PointLight:getLinear()
    return self.linear
end

function PointLight:getQuadratic()
    return self.quadratic
end