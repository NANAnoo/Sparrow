/*
 * Created by elecfrog@du
 * 2023/02/16
 * Stay Empty for API specification
 */

#pragma once

#include "Maths/MathUtils.hpp"
#include "Utils/MacroUtils.h"
#include "Material.h"

#include <vector>

namespace SPW
{
    struct BasicVertex
    {
        glm::vec3 position;
        glm::vec2 texCoords;
    };

    // Complete POD AttribVertex Definition with Constructors
    struct AttribVertex
    {
        AttribVertex()
            :position(glm::vec3(0.0f))
            ,colors(glm::vec4(0.0f))
            ,texCoords(glm::vec2(0.0f))
            ,normal(glm::vec3(0.0f))
            ,tangent(glm::vec3(0.0f))
        {        }

        // Constructor with Parameters
        AttribVertex(glm::vec3 _position, glm::vec4 _colors, glm::vec2 _texCoords, glm::vec3 _normal, glm::vec3 _tangent)
            :position(_position)
            ,colors(_colors)
            ,texCoords(_texCoords)
            ,normal(_normal)
            ,tangent(_tangent)
        {        }

        glm::vec3 position;
        glm::vec4 colors;
        glm::vec2 texCoords;
        glm::vec3 normal;
        glm::vec3 tangent;

        bool operator==(const AttribVertex& other) const
        {
            return position == other.position 
                    && texCoords == other.texCoords 
                    && colors == other.colors 
                    && normal == other.normal 
                    && tangent == other.tangent;
        }
    };

    // Is this needed?
    class SubMesh
    {

    };
    
    class Mesh
    {
    // TODO: Revise to Protected
    public:
        std::string m_Name;
        std::vector<AttribVertex> m_Vertices;
        SharedPtr<Material> m_Material;
        std::vector<Count> m_Indices;
    };
}
