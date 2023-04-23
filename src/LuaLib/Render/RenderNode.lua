require("luaclass")
require("Render/RenderDefines")

-- base class for all render nodes
RenderNode = class("RenderNode")

function RenderNode:init(clear_type, depth_comp, depth_test)
    self.cpp_object = nil
    self.clear_type = clear_type or ClearType.NoClear
    self.depth_comp = depth_comp or DepthComp.LESS
    self.depth_test = depth_test or true
    self.input_ports = {}
end

function setClearType(clear_type)
    self.clear_type = clear_type
    self.cpp_object:update("clear_type", clear_type)
end

function setDepthComp(depth_comp)
    self.depth_comp = depth_comp
    self.cpp_object:update("depth_comp", depth_comp)
end

function setDepthTest(depth_test)
    self.depth_test = depth_test
    self.cpp_object:update("depth_test", depth_test)
end

function RenderNode:set_cpp_object(cpp_obj)
    self.cpp_object = cpp_obj
end

function RenderNode:getID()
    return self.cpp_object:getID()
end

function RenderNode:bindInputPort(port)
    table.insert(self.input_ports, port)
    self.cpp_object:bindInputPort(port)
end

-- ModelPassNode
ModelPassNode = class("ModelPassNode", RenderNode)

function ModelPassNode:init(attachment_type, width, height)
    self.attachment_type = attachment_type or AttachmentType.ColorType
    self.width = width or 0
    self.height = height or 0
    self.attachments = {}
end

function ModelPassNode:setWidth(width)
    self.width = width
    self.cpp_object:update("width", width)
end

function ModelPassNode:setHeight(height)
    self.height = height
    self.cpp_object:update("height", height)
end

function ModelPassNode:setAttachmentType(attachment_type)
    self.attachment_type = attachment_type
    self.cpp_object:update("attachment_type", attachment_type)
end

function ModelPassNode:addAttachment(attachment_format)
    table.insert(self.attachments, attachment_format)
    local port = self.cpp_object:addAttachment(attachment_format)
    return port
end


-- ModelRepeatPassNode
ModelRepeatPassNode = class("ModelRepeatPassNode", ModelPassNode)

function ModelRepeatPassNode:init(repeat_type, repeat_count)
    self.repeat_type = repeat_type or RepeatType.NoRepeat
    self.repeat_count = repeat_count or 0
end

function ModelRepeatPassNode:setRepeatType(repeat_type)
    self.repeat_type = repeat_type
    self.cpp_object:update("repeat_type", repeat_type)
end

function ModelRepeatPassNode:setRepeatCount(repeat_count)
    self.repeat_count = repeat_count
    self.cpp_object:update("repeat_count", repeat_count)
end

-- ModelToScreenNode
ModelToScreenNode = class("ModelToScreenNode", RenderNode)

function ModelToScreenNode:init()
    self.screen_output_type = ScreenAttachmentType.UnknownScreenAttachmentType
end

function ModelToScreenNode:setScreenOutputType(attachment_type)
    self.screen_output_type = attachment_type 
    return self.cpp_object:setScreenOutputType(attachment_type)
end

-- PresentNode
PresentNode = class("PresentNode", RenderNode)

function PresentNode:init(shader_desc)
    self.shader_desc = shader_desc
end

-- ScreenPassNode
ScreenPassNode = class("ScreenPassNode", RenderNode)

function ScreenPassNode:init(shader_desc)
    self.shader_desc = shader_desc
end

function ScreenPassNode:setScreenAttachment(attachment_type)
    self.screen_output_type = attachment_type
    return self.cpp_object:setScreenOutputType(attachment_type)
end