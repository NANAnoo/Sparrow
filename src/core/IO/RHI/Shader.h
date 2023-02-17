/*
 * Created by elecfrog@du
 * 2023/02/16
 * Stay Empty for API specification
 */

#pragma once

#include "Utils/MacroUtils.h"



namespace SPW
{
    class Shader
    {
    public:
        Shader() = default;
        //TODO: Mutiple Sources or Single Source?
        explicit Shader(const FilePath& _filePath);
        Shader(const char* _nativeShaderString);
    private:
        // Need C++ 17 Support
        // TODO: Using Native String? Self-String Ulties?
        FilePath  m_filePath;

        UnorderMap<std::string, FilePath> m_ShaderPaths;

        void ShaderComplie();
        void ShaderLink();
    };
}