-- Application declare --
global = {}

Application = {
    onInit = function () end,
    beforeUpdate = function () end,
    onUpdate = function (duration) end,
    afterUpdate = function () end,
    onStop = function () end
}

function Application:new(o, appInfo, onInit, beforeUpdate, onUpdate, afterUpdate, onStop)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    self.onInit = onInit or function () print('onInit not implemented') end
    self.beforeUpdate = beforeUpdate or function () print('beforeUpdate not implemented') end
    self.onUpdate = onUpdate or function(duration) print('onUpdate not implemented', duration) end
    self.afterUpdate = afterUpdate or function () print('afterUpdate not implemented') end
    self.onStop = onStop or function () print('onStop not implemented') end
    self.name = appInfo.name or "unknown"
    self.width = appInfo.width or 800
    self.height = appInfo.height or 600
    return o
end
function App(appInfo, onInit, beforeUpdate, onUpdate, afterUpdate, onStop)
    global.app = Application:new(nil, appInfo, onInit, beforeUpdate, onUpdate, afterUpdate, onStop)
end