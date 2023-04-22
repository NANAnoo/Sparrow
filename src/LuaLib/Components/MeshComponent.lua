require('luaclass')
require('Component')

MeshComponent = class('MeshComponent', Component)

function MeshComponent:init(cam_id, mesh_path, graph_id)
    self.cam_id = cam_id or ""
    self.mesh_path = mesh_path or ""
    self.graph_id = graph_id or 0
    self.renderPrograms = {}
end

function MeshComponent:setMeshPath(mesh_path)
    self.mesh_path = mesh_path
    self:update('mesh_path', mesh_path)
end

function MeshComponent:setGraphId(graph_id)
    self.graph_id = graph_id
    self:update('graph_id', graph_id)
end

function MeshComponent:setRenderProgram(pass_id, shader_id)
    self.renderPrograms[pass_id] = shader_id
    local pair = {}
    pair.pass_id = pass_id
    pair.shader_id = shader_id
    self:update('renderPrograms', pair)
end

function MeshComponent:synchronize()
    self.cam_id = self:getCppValue('cam_id')
    self.mesh_path = self:getCppValue('mesh_path')
    self.graph_id = self:getCppValue('graph_id')
    self.renderPrograms = self:getCppValue('renderPrograms')
end