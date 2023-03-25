//
// Created by sgsdi on 2023/2/17.
//

#ifndef SPARROW_OPENGLSHADER_H
#define SPARROW_OPENGLSHADER_H
#include "Render/shader.h"
#include <vector>

namespace SPW
{
    class OpenGLShader:public Shader
    {
    public:
        explicit OpenGLShader( const ShaderHandle &handle);

        void Bind() const override
        {


            glUseProgram(ID);
        }
        void Unbind() const override
        {
            glUseProgram(0);
        }
        void setBool(const std::string& name, bool value) const final;
        void setInt(const std::string& name, int value) const final;
        void setFloat(const std::string& name, float value) const final;
        void setVec2(const std::string& name, glm::vec2 value) const final;
        void setVec3(const std::string& name, glm::vec3 value) const final;
        void setVec4(const std::string& name, glm::vec4 value) const final;
        void setMat2(const std::string& name, glm::mat2 mat) const final;
        void setMat3(const std::string& name, glm::mat3 mat) const final;
        void setMat4(const std::string& name, glm::mat4 mat) const final;
        void setDLight(unsigned int idx, const DLight &light) const final;
        void setPLight(unsigned int idx, const PLight &light) const final;
        void bindTex2D(unsigned int slot,unsigned int texid) const final;
        void bindTexArray(unsigned int slot,unsigned int texid) const final;
    public:
        unsigned int ID;
        std::string m_name;

    private:
        void checkCompileErrors(GLuint shader, std::string type);
        //std::vector<char> binary;

    };
}




#endif //SPARROW_OPENGLSHADER_H
