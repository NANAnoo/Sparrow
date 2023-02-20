//
// Created by sgsdi on 2023/2/19.
//

#ifndef SPARROW_MESH_H
#define SPARROW_MESH_H
#include "glm/glm.hpp"
#include "vector"
#include "Render/IndexBuffer.h"
#include "Render/VertexBuffer.h"
#include "memory"
#include "Render/RenderBackEndI.h"
namespace SPW
{
    struct Vertex
    {
        // position
        glm::vec3 Position;
        // normal
        glm::vec3 Normal;
        // texCoords
        glm::vec2 TexCoords;
        // tangent
        glm::vec3 Tangent;
    };

    class Mesh
    {
    public:
        // mesh Data
        std::vector<Vertex>       vertices;
        std::vector<unsigned int> indices;
        unsigned int VAO;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
        {
            this->vertices = vertices;
            this->indices = indices;
            // now that we have all the required data, set the vertex buffers and its attribute pointers.
            setupMesh();
        }
        void Draw()
        {
            EBO->Bind();
            VBuffer->Bind();
            RenderBackEndI::DrawElement(VBuffer,EBO);
        }
        void setupMesh()
        {

        }
    public:
        std::shared_ptr<IndexBuffer> EBO;
        std::shared_ptr<VertexBufferI> VBuffer;
    };
}

#endif //SPARROW_MESH_H
