require("luaclass")

Component = class("Component")

function Component:init()
    self.cpp_component = nil
end

function Component:set_cpp_object(cpp_component)
    self.cpp_component = cpp_component
end

function Component:update(key, value)
    local wrapper = {}
    wrapper["value"] = value
    self.cpp_component:update(key, wrapper)
end

function Component:getCppValue(key)
    return self.cpp_component:getCppValue(self, key)
end

-- remove from entity, call cpp object remove
function Component:remove()
    self.cpp_component:remove()
end