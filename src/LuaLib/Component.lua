require("luaclass")

Component = class("Component")

function Component:init()
    self.cpp_component = nil
    self.need_update = true
end

function Component:set_cpp_object(cpp_component)
    self.cpp_component = cpp_component
end

function Component:update()
    self.cpp_component:update(self)
    self.need_update = false
end

function Component:__newindex( index, value )
    rawset( self, index, value )
    rawset( self, 'need_update', true)
end

function Component:__index( index )
    return rawget( self, index )
end