require("luaclass")

Component = class("Component")

function Component:set_cpp_object(cpp_component)
    self.cpp_component = cpp_component
    self.members = {}
end

function Component:__newindex( index, value )
    -- @cpp ComponentWrapper.syncValue
    if index == "cpp_component" then
        rawset( self, index, value )
    else
        self.cpp_component:syncValue(index, value)
        self.members[index] = value
    end
end

function MyClass:__index( index )
    if index == "cpp_component" then
        return rawget( self, index)
    else
        return self.members[index]
    end
end