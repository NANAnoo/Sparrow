require("luaclass")
require("Component")

Entity = class("Entity")
function Entity:init(name, cpp_obj)
    -- call EntityWrapper.getID
    self.id = cpp_obj:getID()
    self.name = name
    self.cpp_object = cpp_obj
    self.component_map = {}
end

function Entity:addComponent(component_type, ...)
    -- precheck type
    if not is_sub_class_of(component_type, Component) then
        print("Not a valid component : ", component_type)
        return
    end
    if self:getComponent(component_type) ~= nil then
        return self:getComponent(component_type)
    end
    -- init value
    local component = component_type.new(...)
    -- create cpp object in lua component
    local cpp_component = self.cpp_object:createComponent(component_type.__cname, component)
    component:set_cpp_object(cpp_component)
    -- record current component
    self.component_map[component_type.__cname] = component
    return component
end

function Entity:getComponent(component_type) 
    -- check if the component is in the map
    local com = self.component_map[component_type.__cname]

    -- if not, try to get the component from cpp_object
    if com == nil then
        local cpp_com = self.cpp_object:getComponent(component_type.__cname)
        if cpp_com ~= nil and cpp_com:isValid() then
            com = component_type.new()
            com:set_cpp_object(cpp_com)
            com:synchronize()
            self.component_map[component_type.__cname] = com
        end
    end
    return com
end

function Entity:hasComponent(component_type)
    return self:getComponent(component_type) ~= nil
end
