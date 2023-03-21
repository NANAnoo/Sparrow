-- id <-> entity
-- cpp_object : Scene wrapper

require("Entity")

Scene = {ent_map = {}}

function Scene:new(o, path)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    -- TODO: SceneWrapper constructor
    self.cpp_object = Cpp.SceneWrapper(path)
    return o
end

function Scene:CppObject()
    return self.cpp_object
end

function Scene:addEntity()
    -- create a new entity in cpp side
    -- then get the entity cpp wrapper obj
    -- TODO: SceneWrapper.createEntity
    local obj = self.cpp_object:createEntity()
    local en = Entity:new(nil, obj)
    table.insert(self.ent_map, en.id, en)
end

function Scene:removeEntity(en)
    -- en : @Entity
    -- TODO: SceneWrapper.removeEntity() by id
    self.cpp_object:remove(en.id)
    table.remove(self.ent_map, en.id)
end

function Scene:forEach(component_type, callback)
    for _, ent in pairs(self.ent_map) do
        if ent.hasComponent(component_type) then
            callback(ent)
        end
    end
end