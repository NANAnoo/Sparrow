require('luaclass')

OrthoCamera = class('OrthoCamera', Component)

function OrthoCamera:init(camera_id, left, right, bottom, top, near, far)
    self.left = left or -1
    self.right = right or 1
    self.bottom = bottom or -1
    self.top = top or 1
end

-- setter and getters
function OrthoCamera:setLeft(left)
    self.left = left
    self:update("left", left)
end

function OrthoCamera:getLeft()
    return self.left
end

function OrthoCamera:setRight(right)
    self.right = right
    self:update("right", right)
end

function OrthoCamera:getRight()
    return self.right
end

function OrthoCamera:setBottom(bottom)
    self.bottom = bottom
    self:update("bottom", bottom)
end

function OrthoCamera:getBottom()
    return self.bottom
end

function OrthoCamera:setTop(top)
    self.top = top
    self:update("top", top)
end

function OrthoCamera:getTop()
    return self.top
end

-- synchronize
function OrthoCamera:synchronize()
    self.left = self:getCppValue("left")
    self.right = self:getCppValue("right")
    self.bottom = self:getCppValue("bottom")
    self.top = self:getCppValue("top")
end