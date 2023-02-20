//
// Created by sgsdi on 2023/2/14.
//

#include "shader.h"
#include "RenderBackEndI.h"
#include "Platforms/OPENGL/OpenGLShader.h"
namespace SPW
{

    template<class T>
    void Shader::SetUniformValue(const std::string& str,T& value)
    {
        if(std::is_same<T,bool>::value)
            setBool(str,value);
        else if(std::is_same<T,int>::value)
            setInt(str,value);
        else if(std::is_same<T,float>::value)
            setFloat(str,value);
        else if(std::is_same<T,glm::vec2>::value)
            setVec2(str,value);
        else if(std::is_same<T,glm::vec3>::value)
            setVec3(str,value);
        else if(std::is_same<T,glm::vec4>::value)
            setVec4(str,value);
        else if(std::is_same<T,glm::mat2>::value)
            setMat2(str,value);
        else if(std::is_same<T,glm::mat3>::value)
            setMat3(str,value);
        else if(std::is_same<T,glm::mat4>::value)
            setMat4(str,value);
    }


}