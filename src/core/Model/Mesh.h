//
// Created by sgsdi on 2023/2/19.
//

#ifndef SPARROW_MESH_H
#define SPARROW_MESH_H
#include "glm/glm.hpp"
#include "vector"

#include "memory"
#include "Render/IndexBuffer.h"
#include "Render/VertexBuffer.h"
#include "Render/RenderBackEndI.h"
#include "Render/RenderPass.hpp"
#include "Render/shader.h"
#include "Render/Material.h"
#include "Vertex.h"

namespace SPW
{
    class Mesh
    {
    public:
        // mesh Data
        std::vector<Vertex>         vertices;
        std::vector<unsigned int>   indices;

        Mesh() = default;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
        {
            this->vertices = std::move(vertices);
            this->indices = std::move(indices);
        }

        void Draw(const std::shared_ptr<RenderBackEndI>& renderBackEnd)
        {
            shader->Bind();
            renderBackEnd->DrawElement(VBuffer,EBO);
        }

        void setupMesh(std::shared_ptr<RenderBackEndI>& renderBackEnd)
        {
            EBO = renderBackEnd->createIndexBuffer(indices);
            VBuffer = renderBackEnd->createVertexBuffer();
            VBuffer->VertexBufferData(vertices);
        }

    	void setShader(std::shared_ptr<RenderBackEndI>& renderBackEnd, const ShaderHandle& handle)
        {
            shader = renderBackEnd->getShader(handle);
        }

    	void SetMaterial(std::shared_ptr<Material> material) { this->m_Material = std::move(material); }

    	std::shared_ptr<Material> GetMaterial() const { return this->m_Material; }

    public:
        std::shared_ptr<IndexBuffer> EBO = nullptr;
        std::shared_ptr<VertexBufferI> VBuffer = nullptr;
        std::shared_ptr<Shader> shader = nullptr;
    private:
        std::shared_ptr<Material>   m_Material;

    };
}

#endif //SPARROW_MESH_H
