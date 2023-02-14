//
// Created by sgsdi on 2023/2/14.
//

#ifndef SPARROW_MESH_H
#define SPARROW_MESH_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

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
        // bitangent
        glm::vec3 Bitangent;
    };


    class Mesh
    {
    public:
        // mesh Data
        std::vector<Vertex>       vertices;
        std::vector<unsigned int> indices;

    public:
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
        {
            this->vertices = vertices;
            this->indices = indices;
        }

        void draw();
    };
}



#endif //SPARROW_MESH_H
