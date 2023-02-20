//
// Created by sgsdi on 2023/2/20.
//

#ifndef SPARROW_VERTEX_H
#define SPARROW_VERTEX_H
#include "glm/glm.hpp"
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

        glm::vec3 Bitangent;

        Vertex(glm::vec3 position,glm::vec3 normal,glm::vec2 texCoords,glm::vec3 tangent,glm::vec3 bitangent):
        Position(position),Normal(normal),TexCoords(texCoords),Tangent(tangent),Bitangent(bitangent){}
    };
}
#endif //SPARROW_VERTEX_H
