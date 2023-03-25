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
    if self.component_map[component_type.__cname] then
        return self.component_map[component_type.__cname]
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
    local com = self.component_map[component_type.__cname]
    return com
end

function Entity:hasComponent(component_type)
    if self.component_map[component_type.__cname] then
        return true
    end
    return false
end
