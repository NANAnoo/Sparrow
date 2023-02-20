//
// Created by sgsdi on 2023/2/14.
//

#ifndef SPARROW_SHADER_H
#define SPARROW_SHADER_H
#include "string"
#include "glm/glm.hpp"
#include <type_traits>
#include <glad/glad.h>
#include <memory>

namespace SPW
{

    class Shader
    {
    public:
        virtual ~Shader() = default;
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        template<class T>
        void SetUniformValue(const std::string& str,T& value);

    private:
        virtual void setBool(const std::string& name, bool value) const = 0;
        virtual void setInt(const std::string& name, int value) const = 0;
        virtual void setFloat(const std::string& name, float value) const = 0;
        virtual void setVec2(const std::string& name, const glm::vec2& value) const = 0;
        virtual void setVec3(const std::string& name, const glm::vec3& value) const = 0;
        virtual void setVec4(const std::string& name, const glm::vec4& value) const = 0;
        virtual void setMat2(const std::string& name, const glm::mat2& mat) const = 0;
        virtual void setMat3(const std::string& name, const glm::mat3& mat) const = 0;
        virtual void setMat4(const std::string& name, const glm::mat4& mat) const = 0;
    };


}


#endif //SPARROW_SHADER_H
