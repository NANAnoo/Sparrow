//
// Created by sgsdi on 2023/2/17.
//

#include "OpenGLShader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

namespace SPW
{
    OpenGLShader::OpenGLShader( const ShaderHandle &handle):m_name(handle.name)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(handle.vertex_shader_path);
            fShaderFile.open(handle.frag_shader_path);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();

        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessery

        /*
        GLint binaryLength;
        glGetProgramiv(ID, GL_PROGRAM_BINARY_LENGTH, &binaryLength);
        binary.resize(binaryLength);
        GLenum binaryFormat;

        glGetProgramBinary(ID, binaryLength, NULL, &binaryFormat, binary.data());
        glProgramBinary(ID, binaryFormat, binary.data(), binaryLength);
        */
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void OpenGLShader::checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

    void OpenGLShader::setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    void OpenGLShader::setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void OpenGLShader::setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    void OpenGLShader::setVec2(const std::string& name, glm::vec2 value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void OpenGLShader::setVec3(const std::string& name, glm::vec3 value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void OpenGLShader::setVec4(const std::string& name, glm::vec4 value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void OpenGLShader::setMat2(const std::string& name, glm::mat2 mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void OpenGLShader::setMat3(const std::string& name, glm::mat3 mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void OpenGLShader::setMat4(const std::string& name, glm::mat4 mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void OpenGLShader::setDLight(unsigned int idx, const DLight &light) const
    {
        std::string name = "DLights["+std::to_string(idx)+"]";
        setVec3(name+".direction",light.direction);
        setVec3(name+".ambient",light.ambient);
        setVec3(name+".diffuse",light.diffuse);
        setVec3(name+".specular",light.specular);
    }

    void OpenGLShader::setPLight(unsigned int idx, const PLight &light) const
    {
        std::string name = "PLights["+std::to_string(idx)+"]";
        setVec3(name+".position",light.position);
        setVec3(name+".ambient",light.ambient);
        setVec3(name+".diffuse",light.diffuse);
        setVec3(name+".specular",light.specular);
        setFloat(name+".constant",light.constant);
        setFloat(name+".linear",light.linear);
        setFloat(name+".quadratic",light.quadratic);
    }

    void OpenGLShader::bindTex2D(unsigned int slot, unsigned int texid) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D,texid);
    }

    void OpenGLShader::bindTexArray(unsigned int slot, unsigned int texid) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D_ARRAY,texid);

    }

}
