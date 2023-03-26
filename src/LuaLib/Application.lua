-- Application declare --
global = {}

Application = {
    name = "unknown",
    width = 0,
    height = 0,
    onInit = function () end,
    beforeUpdate = function () end,
    onUpdate = function (duration) end,
    afterUpdate = function () end,
    onStop = function () end,
    onWindowResize = function (width, height) end
}

function Application:new(o, name, width, height, onInit, beforeUpdate, onUpdate, afterUpdate, onStop, onWindowResize)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    self.onInit = onInit or function () print('onInit not implemented') end
    self.beforeUpdate = beforeUpdate or function () print('beforeUpdate not implemented') end
    self.onUpdate = onUpdate or function(duration) print('onUpdate not implemented', duration) end
    self.afterUpdate = afterUpdate or function () print('afterUpdate not implemented') end
    self.onStop = onStop or function () print('onStop not implemented') end
    self.onWindowResize = onWindowResize or function (width, height) print('onWindowResize not implemented', width, height) end
    self.name = name or "unknown"
    self.width = width or 800
    self.height = height or 600
    return o
end

function App(app)
    global.app = Application:new(nil, app.name, app.width, app.height, app.onInit, app.beforeUpdate, app.onUpdate, app.afterUpdate, app.onStop, app.onWindowResize)
end