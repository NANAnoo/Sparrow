require "luaclass"
require("Render/RenderDefines")

ShaderDesc = class("ShaderDesc")

function ShaderDesc:init()
    self.cpp_object = SPW.createShaderDesc()
    self.shader = nil
    -- inputs
    self.mat_inputs = {}
    self.transform_inputs = {}
    self.context_inputs = {}
    self.light_inputs = {}
    
    -- self.dependency_inputs = {}
end

function ShaderDesc:getID()
    return self.cpp_object:getID()
end

function ShaderDesc:setMatInput(type, value)
    self.mat_inputs[type] = value
    self.cpp_object:setMatInput(type, value)
end

function ShaderDesc:setTransformInput(type, value)
    self.transform_inputs[type] = value
    self.cpp_object:setTransformInput(type, value)
end

function ShaderDesc:setContextInput(type, value)
    self.context_inputs[type] = value
    self.cpp_object:setContextInput(type, value)
end

function ShaderDesc:setLightInput(type, value)
    self.light_inputs[type] = value
    self.cpp_object:setLightInput(type, value)
end

function ShaderDesc:setDependencyInput(port, value)
    self.cpp_object:setDependencyInput(port, value)
end

function ShaderDesc:setShader(shader)
    self.shader = shader
    self.cpp_object:setShader(shader)
end
