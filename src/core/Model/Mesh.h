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

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,std::shared_ptr<RenderBackEndI> renderBackEnd)
        {
            this->vertices = vertices;
            this->indices = indices;
            // now that we have all the required data, set the vertex buffers and its attribute pointers.
            setupMesh(renderBackEnd);
        }
        void Draw(std::shared_ptr<RenderBackEndI> renderBackEnd)
        {

            renderBackEnd->DrawElement(VBuffer,EBO);
        }
        void setupMesh(std::shared_ptr<RenderBackEndI> renderBackEnd)
        {
            EBO = renderBackEnd->createIndexBuffer(indices);
            VBuffer = renderBackEnd->createVertexBuffer();
            VBuffer->VertexBufferData(vertices);
        }
    public:
        std::shared_ptr<IndexBuffer> EBO = nullptr;
        std::shared_ptr<VertexBufferI> VBuffer = nullptr;
    };
}

#endif //SPARROW_MESH_H
