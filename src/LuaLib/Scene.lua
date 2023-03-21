-- id <-> entity
-- cpp_object : Scene wrapper
require("luaclass")
require("Entity")

Scene = class("Scene")

function Scene:init(path)
    -- TODO: SceneWrapper constructor
    self.cpp_object = Cpp.SceneWrapper(path)
    self.ent_map = {}
end

function Scene:CppObject()
    return self.cpp_object
end

function Scene:addEntity(name)
    -- create a new entity in cpp side
    -- then get the entity cpp wrapper obj
    -- TODO: SceneWrapper.createEntity
    local id = self.cpp_object:createEntity(name)
    local en = Entity.new(id)
    self.ent_map[en.id] = en
    return en
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