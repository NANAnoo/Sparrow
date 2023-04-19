require("luaclass")

RenderGraph = class("RenderGraph")

function RenderGraph:init(cpp_object)
    self.cpp_object = cpp_object or nil
    self.render_nodes = {}
end

function RenderGraph:set_cpp_object(cpp_obj)
    self.cpp_object = cpp_obj
end

function RenderGraph:createRenderNode(node_type, ...)
    if self.cpp_object == nil then
        print("RenderGraph not initialized")
        return
    end
    local render_node = node_type.new(...)
    local cpp_node = self.cpp_object:createRenderNode(node_type.__cname, render_node)
    render_node:set_cpp_object(cpp_node)
    table.insert(self.render_nodes, render_node)
    return render_node
end

function RenderGraph:getID() 
    return self.cpp_object:getID()
end