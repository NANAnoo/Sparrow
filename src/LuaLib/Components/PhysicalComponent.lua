require "luaclass"
require "Component"

RigidState = {
    Sleep = 0,
    Awake = 1,
}

ColliderState = {
    needSleep = 0,
    needAwake = 1,
    sleep = 2,
    awake = 3
}


Collider = class("Collider", Component)

function TriggerCallback(callback, scene)
    if not callback or not scene then
        return nil
    end
    return function(id)
        local entity = scene:getEntityFromID(id)
        if callback then
            callback(entity)
        end
    end
end

function ContactCallback(callback, scene)
    if not callback or not scene then
        return nil
    end
    return function(id1,id2)
        local entity1 = scene:getEntityFromID(id1)
        local entity2 = scene:getEntityFromID(id2)
        if callback then
            callback(entity1,entity2)
        end
    end
end

function Collider:init(scene,trigger_enter,trigger_leave,contact_enter,contact_leave)
    
    self.trigger_enter = TriggerCallback(trigger_enter,scene)
    self.trigger_leave = TriggerCallback(trigger_leave,scene)
    self.contact_enter = ContactCallback(contact_enter,scene)
    self.contact_leave = ContactCallback(contact_leave,scene)

end


-- setters
function Collider:setTriggerEnter(callback, scene)
    self.trigger_enter = TriggerCallback(callback, scene)
    self:update('trigger_enter', self.trigger_enter)
end
function Collider:setTriggerLeave(callback, scene)
    self.trigger_leave = TriggerCallback(callback, scene)
    self:update('trigger_leave', self.trigger_leave)
end
function Collider:setContactEnter(callback, scene)
    self.contact_enter = ContactCallback(callback, scene)
    self:update('contact_enter', self.contact_enter)
end
function Collider:setContactLeave(callback, scene)
    self.contact_enter = ContactCallback(callback, scene)
    self:update('contact_leave', self.contact_leave)
end



RigidStaticComponent = class("RigidStaticComponent", Component)

function RigidStaticComponent:init(rigidState)
    self.rigidState = rigidState;
end

function RigidStaticComponent:setState(rigidState)
    self:update("rigidState", rigidState)
    
end

function RigidStaticComponent:synchronize()
    self.rigidState = self:getCppValue("rigidState")
end




RigidDynamicComponent = class('RigidDynamicComponent', Component)

function RigidDynamicComponent:init(rigidState)
    self.rigidState = rigidState;
end

function RigidDynamicComponent:setState(rigidState)
    self:update("rigidState", rigidState)
    
end

function RigidDynamicComponent:synchronize()
    self.rigidState = self:getCppValue("rigidState")
end




SphereCollider = class('SphereCollider', Collider)

function SphereCollider:init(state,is_trigger,sphere_radius_)
    self.state_ = state or ColliderState.sleep
    self.is_trigger_ = is_trigger or false
    self.sphere_radius_ = sphere_radius_ or 0.1
end

function SphereCollider:synchronize()
    self.state = self:getCppValue("state_")
    self.is_trigger_ = self:getCppValue("is_trigger_")
    self.sphere_radius_ = self:getCppValue("sphere_radius_")
end



CapsuleCollider = class('CapsuleCollider', Collider)

function CapsuleCollider:init(state,is_trigger,capsule_radius_,capsule_half_height_,capsule_axis_,degree)
    self.state_ = state or ColliderState.sleep
    self.is_trigger_ = is_trigger or false
    self.capsule_radius_ = capsule_radius_ or 0
    self.capsule_half_height_ = capsule_half_height_ or 0
    self.capsule_axis_ = capsule_axis_ or glm.vec3(0, 0, 0)
    self.degree = degree or 0
end
function CapsuleCollider:setState(state)
    self:update("state_", state)

end
function CapsuleCollider:synchronize()
    self.state_ = self:getCppValue("state_")
    self.is_trigger_ = self:getCppValue("is_trigger_")
    self.capsule_radius_ = self:getCppValue("capsule_radius_")
    self.capsule_half_height_ = self:getCppValue("capsule_half_height_")
    self.capsule_axis_ = self:getCppValue("capsule_axis_")
    self.degree = self:getCppValue("degree")
end


BoxCollider = class('BoxCollider', Collider)

function BoxCollider:init(state,is_trigger,box_size_)
    self.state_ = state or ColliderState.sleep
    self.is_trigger_ = is_trigger or false
    self.box_size_ =  box_size_ or glm.vec3(0.1,0.1,0.1)
end

function BoxCollider:synchronize()
    self.state_ = self:getCppValue("state_")
    self.is_trigger_ = self:getCppValue("is_trigger_")
    self.box_size_ = self:getCppValue("box_size_")
end