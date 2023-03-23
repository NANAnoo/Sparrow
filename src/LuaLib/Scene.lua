-- id <-> entity
-- cpp_object : Scene wrapper
require("luaclass")
require("Entity")

Scene = class("Scene")

function Scene:init(path)
    -- SceneWrapper constructor
    self.cpp_object = SPW.SceneWrapper(path)
    self.ent_map = {}
end

function Scene:addEntity(name)
    -- create a new entity in cpp side
    -- then get the entity cpp wrapper obj
    -- SceneWrapper.createEntity
    local cpp_entity = self.cpp_object:createEntity(name)
    -- cpp_entity : SPW:EntityWrapper
    local en = Entity.new(name, cpp_entity)
    self.ent_map[en.id] = en
    return en
end

function Scene:update()
    for _, ent in pairs(self.ent_map) do
        ent:updateAllComponent()
    end
end

function Scene:getEntityFromID(id)
    return self.ent_map[id]
end

function Scene:removeEntity(en)
    -- en : @Entity
    -- SceneWrapper.removeEntity() by id
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