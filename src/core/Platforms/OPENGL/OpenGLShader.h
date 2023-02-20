//
// Created by sgsdi on 2023/2/17.
//

#ifndef SPARROW_OPENGLSHADER_H
#define SPARROW_OPENGLSHADER_H
#include "Render/shader.h"
namespace SPW
{
    class OpenGLShader:public Shader
    {
    public:
        OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
        virtual std::shared_ptr<Shader>createShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)final
        {
            return std::make_shared<OpenGLShader>(name,vertexSrc,fragmentSrc);
        }

        virtual void Bind() const
        {
            glUseProgram(ID);
        }
        virtual void Unbind() const
        {
            glUseProgram(0);
        }
        virtual void setBool(const std::string& name, bool value) const final;
        virtual void setInt(const std::string& name, int value) const final;
        virtual void setFloat(const std::string& name, float value) const final;
        virtual void setVec2(const std::string& name, const glm::vec2& value) const final;
        virtual void setVec3(const std::string& name, const glm::vec3& value) const final;
        virtual void setVec4(const std::string& name, const glm::vec4& value) const final;
        virtual void setMat2(const std::string& name, const glm::mat2& mat) const final;
        virtual void setMat3(const std::string& name, const glm::mat3& mat) const final;
        virtual void setMat4(const std::string& name, const glm::mat4& mat) const final;
    public:
        unsigned int ID;
        std::string m_name;

    private:
        void checkCompileErrors(GLuint shader, std::string type);
    };
}




#endif //SPARROW_OPENGLSHADER_H
