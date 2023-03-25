require "luaclass"
require "Component"

DirectionalLight = class("DirectionalLight", Component)

function DirectionalLight:init(ambient, diffuse, specular)
    self.ambient = ambient
    self.diffuse = diffuse
    self.specular = specular
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