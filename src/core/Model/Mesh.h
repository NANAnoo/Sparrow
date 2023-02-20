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
#include "Vertex.h"

namespace SPW
{
    class Mesh
    {
    public:
        // mesh Data
        std::vector<Vertex>       vertices;
        std::vector<unsigned int> indices;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,const std::shared_ptr<RenderBackEndI>& renderBackEnd)
        {
            this->vertices = vertices;
            this->indices = indices;
            // now that we have all the required data, set the vertex buffers and its attribute pointers.
            setupMesh(renderBackEnd);
        }
        void Draw(const std::shared_ptr<RenderBackEndI>& renderBackEnd)
        {
            EBO->Bind();
            VBuffer->Bind();
            shader->Bind();
            renderBackEnd->DrawElement(VBuffer,EBO);
        }
        void setupMesh(const std::shared_ptr<RenderBackEndI>& renderBackEnd)
        {
            EBO = renderBackEnd->createIndexBuffer(indices);
            VBuffer = renderBackEnd->createVertexBuffer();
            VBuffer->VertexBufferData(vertices);
        }
        void setshader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc,const std::shared_ptr<RenderBackEndI>& renderBackEnd)
        {
            shader = renderBackEnd->getshader(name,vertexSrc,fragmentSrc);
        }
    public:
        std::shared_ptr<IndexBuffer> EBO = nullptr;
        std::shared_ptr<VertexBufferI> VBuffer = nullptr;
        std::shared_ptr<Shader> shader = nullptr;
    };
}

#endif //SPARROW_MESH_H
