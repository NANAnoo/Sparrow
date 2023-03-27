function clone(object)
    local lookup_table = {}
    local function _copy(_object)
        if type(_object) ~= "table" then
            return _object
        elseif lookup_table[_object]  then
            return lookup_table[_object]
        end
        local new_table
        new_table = {}
        lookup_table[_object] = new_table
        for key, value in pairs(_object) do
            new_table[_copy(key)] = _copy(value)
        end
        return setmetatable(new_table, getmetatable(_object))
    end
    return _copy(object)
end

--Create an class.
function class(classname, super)
    local superType = type(super)
    local cls

    if superType ~= "table" then
        superType = nil
        super = nil
    end

    -- inherited from Lua Object
    if super then
        cls = clone(super)
        cls.super = super
    else
        cls = {init = function() end}
    end

    cls.__cname = classname
    cls.__index = cls
    

    function cls.new(...)
        local instance = setmetatable({}, cls)
        instance.class = cls
        instance:init(...)
        return instance
    end

    return cls
end

--- check the inherit info
function is_sub_class_of(sub,super)
    if sub.__index==nil or sub.__index.super==nil then
        return false
    end
    if sub.__index.super==super then
        return true
    else
        return is_sub_class_of(sub.__index.super,super)
    end
end

function Set (list)
    local set = {}
    for _, l in ipairs(list) do set[l] = true end
    return set
  end