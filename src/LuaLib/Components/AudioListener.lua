require "luaclass"
require "Component"

AudioListener = class("AudioListener", Component)

function AudioListener:init(listener_id)
    self.listener_id = listener_id or 0
end