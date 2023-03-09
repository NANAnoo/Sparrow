#pragma once
#include <glm/glm.hpp>

namespace SPW {
    struct DLight
    {
        glm::vec3 direction;
    
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };  
    struct PLight
    {    
        glm::vec3 position;
        
        float constant;
        float linear;
        float quadratic;  

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    }; 
}