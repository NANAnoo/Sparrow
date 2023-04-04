require "luaclass"
require "Component"

AudioListener = class("AudioListener", Component)

function AudioListener:init(listener_id)
    self.listener_id = listener_id or 0
end

function AudioListener:synchronize()
    self.listener_id = self:getCppValue("listener_id")
end