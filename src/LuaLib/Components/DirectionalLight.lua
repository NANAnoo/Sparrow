-- require "luaclass"
require "Component"

DirectionalLight = class("DirectionalLight", Component)

function DirectionalLight:init(ambient, diffuse, specular)
    self.ambient = ambient or glm.vec3(0.2, 0.2, 0.2)
    self.diffuse = diffuse  or glm.vec3(0.5, 0.5, 0.5)
    self.specular = specular or glm.vec3(1.0, 1.0, 1.0)
end

-- setters and getters
function DirectionalLight:setAmbient(ambient)
    self.ambient = ambient
    self:update('ambient', ambient)
end

function DirectionalLight:setDiffuse(diffuse)
    self.diffuse = diffuse
    self:update('diffuse', diffuse)
end

function DirectionalLight:setSpecular(specular)
    self.specular = specular
    self:update('specular', specular)
end

function DirectionalLight:getAmbient()
    return self.ambient
end

function DirectionalLight:getDiffuse()
    return self.diffuse
end

function DirectionalLight:getSpecular()
    return self.specular
end

-- synchronize
function DirectionalLight:synchronize()
    self.ambient = self:getCppValue('ambient')
    self.diffuse = self:getCppValue('diffuse')
    self.specular = self:getCppValue('specular')
end