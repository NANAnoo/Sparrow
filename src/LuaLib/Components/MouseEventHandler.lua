require 'luaclass'

MouseEventHandler = class('MouseEventHandler', Component)

-- similiar to KeyEventHandler
function MouseCallback(callback, scene)
    if not callback or not scene then
        return nil
    end
    return function(id, code)
        local entity = scene:getEntityFromID(id)
        if callback then
            callback(entity, code)
        end
    end
end

function MouseScrollCallback(callback, scene)
    if not callback or not scene then
        return nil
    end
    -- @param offset: scroll offset double
    return function(id, offset)
        local entity = scene:getEntityFromID(id)
        if callback then
            callback(entity, offset)
        end
    end
end

function MouseMovementCallback(callback, scene)
    -- @param x: mouse x position
    -- @param y: mouse y position
    -- @param dx: mouse x movement
    -- @param dy: mouse y movement
    if not callback or not scene then
        return nil
    end
    return function(id, x, y, dx, dy)
        local entity = scene:getEntityFromID(id)
        if callback then
            callback(entity, x, y, dx, dy)
        end
    end
end

function MouseEventHandler:init(scene, onMouseDown, onMouseUp, onMouseHeld, onScroll, onMove)
    self.onMouseDown = MouseCallback(onMouseDown, scene)
    self.onMouseUp = MouseCallback(onMouseUp, scene)
    self.onMouseHeld = MouseCallback(onMouseHeld, scene)
    self.onScroll = MouseScrollCallback(onScroll, scene)
    self.onMove = MouseMovementCallback(onMove, scene)
end

-- setters
function MouseEventHandler:setOnMouseDown(callback, scene)
    self.onMouseDown = MouseCallback(callback, scene)
    self:update('onMouseDown', self.onMouseDown)
end

function MouseEventHandler:setOnMouseUp(callback, scene)
    self.onMouseUp = MouseCallback(callback, scene)
    self:update('onMouseUp', self.onMouseUp)
end

function MouseEventHandler:setOnMouseHeld(callback, scene)
    self.onMouseHeld = MouseCallback(callback, scene)
    self:update('onMouseHeld', self.onMouseHeld)
end

function MouseEventHandler:setOnScroll(callback, scene)
    self.onScroll = MouseScrollCallback(callback, scene)
    self:update('onScroll', self.onScroll)
end

function MouseEventHandler:setOnMove(callback, scene)
    self.onMove = MouseMovementCallback(callback, scene)
    self:update('onMove', self.onMove)
end