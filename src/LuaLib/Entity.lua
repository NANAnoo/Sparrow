Entity = {id = "", component_map = {}}

function Entity:new(o, cpp_obj)
    o = o or {}
    setmetatable(o, self)
    self.cpp_entity = cpp_obj
    self.id = cpp_obj.getID()
end

function Entity:addComponent(component_type, ...)
    if self.component_map[component_type.__cname] then
        return self.component_map[component_type.__cname]
    end
    local component = component_type.new()
    -- create cpp object in lua component
    component:Init(self.id, ...)
    self.component_map[component_type.__cname] = component
    return component
end

function Entity:removeComponent(component_type)
    if self.component_map[component_type.__cname] then
        comp = self.component_map[component_type.__cname]
        -- remove cpp object as well
        comp.remove()
        table.remove(self.component_map, component_type.__cname)
    end
end

function Entity:hasComponent(component_type)
    if self.component_map[component_type.__cname] then
        return true
    end
    return false
end
