require 'luaclass'

KeyEventHandler = class('KeyEventHandler', Component)

-- wrapper for lua callback
function KeyCallback(callback, scene)
    if not callback or not scene then
        return nil
    end
    -- from cpp side, we will call this callback
    return function(id, code)
        local entity = scene:getEntityFromID(id)
        if callback then
            callback(entity, code)
        end
    end
end

function KeyEventHandler:init(scene, onKeyDown, onKeyUp, onKeyHeld)
    self.onKeyDown = KeyCallback(onKeyDown, scene)
    self.onKeyUp = KeyCallback(onKeyUp, scene)
    self.onKeyHeld = KeyCallback(onKeyHeld, scene)
end

-- setters
function KeyEventHandler:setOnKeyDown(callback, scene)
    self.onKeyDown = KeyCallback(callback, scene)
    self:update('onKeyDown', self.onKeyDown)
end

function KeyEventHandler:setOnKeyUp(callback, scene)
    self.onKeyUp = KeyCallback(callback, scene)
    self:update('onKeyUp', self.onKeyUp)
end

function KeyEventHandler:setOnKeyHeld(callback, scene)
    self.onKeyHeld = KeyCallback(callback, scene)
    self:update('onKeyHeld', self.onKeyHeld)
end