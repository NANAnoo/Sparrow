//
// Created by sgsdi on 2023/2/17.
//

#include "OpenGLBackEnd.h"
#include "OpenGLVertexBuffer.h"
#include "OpenGLIndexBuffer.h"
#include "OpenGLTextureManager.h"
#include "OpenGLTexture2D.h"
#include "OpenGLFrameBuffer.h"
#include "Render/Material.h"
#include "IO/FileSystem.h"
#include <fstream>
#include <vector>
#include <unordered_set>
float quadVertices[] =
{
                // positions   // texCoords
                -1.0f,  1.0f,  0.0f, 1.0f,
                -1.0f, -1.0f,  0.0f, 0.0f,
                1.0f, -1.0f,  1.0f, 0.0f,

                -1.0f,  1.0f,  0.0f, 1.0f,
                1.0f, -1.0f,  1.0f, 0.0f,
                1.0f,  1.0f,  1.0f, 1.0f
 };

namespace SPW
{

    void OpenGLBackEnd::Init()
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }

    void OpenGLBackEnd::loadShaderLib(std::string libPath) {
        std::vector<FilePath> files = FileSystem::GetFiles(libPath);
        for (auto &path : files) {
            if (path.has_filename() && path.filename().has_extension()) {
                auto ext = path.extension().string();
                if (ext == ".glsl") {
                    auto shader_lib = path.string();
                    std::stringstream ss;
                    std::ifstream fs(shader_lib);
                    ss << fs.rdbuf();
                    auto code = ss.str();
                    shader_lib = "/" + path.filename().string();
                    glNamedStringARB(GL_SHADER_INCLUDE_ARB, shader_lib.size(), shader_lib.c_str(),
                                    code.size(), code.c_str());
                    fs.close();
                }
            }

        }
    }

    void OpenGLBackEnd::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glViewport(x, y, width, height);
    }

    void OpenGLBackEnd::SetClearColor(const glm::vec4 color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void OpenGLBackEnd::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void OpenGLBackEnd::DrawElement(std::shared_ptr<VertexBufferI> &vertexBuffer,
                                    std::shared_ptr<IndexBuffer> &indexBuffer)
    {
        vertexBuffer->Bind();
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indexBuffer->size), GL_UNSIGNED_INT, 0);
        vertexBuffer->UnBind();
    }


    void OpenGLBackEnd::DepthTest(bool enable)
    {
        if(enable) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);
    }

    void OpenGLBackEnd::DepthMask(bool maskFlag)
    {
        if (maskFlag) glDepthMask(GL_TRUE);
        else glDepthMask(GL_FALSE);
    }

    void OpenGLBackEnd::DepthFunc(DepthComp comp)
    {

    }

    void OpenGLBackEnd::Cull(int32_t Bit)
    {
        if (Bit) glEnable(GL_CULL_FACE);
        else glDisable(GL_CULL_FACE);
    }

    void OpenGLBackEnd::CullFrontOrBack(bool bFront)
    {
        if (bFront) glCullFace(GL_FRONT);
        else glCullFace(GL_BACK);
    }

    void OpenGLBackEnd::BindTexture(std::shared_ptr<Shader> shader, std::shared_ptr<Material>material)
    {
        shader->Bind();
        // bind all textures
        std::vector<TextureType> types = {
            TextureType::Albedo, 
            TextureType::Normal, 
            TextureType::Metalness, 
            TextureType::Roughness, 
            TextureType::AmbientOcclusion
            };
        std::vector<std::string> names = {
            "albedoMap",
            "normalMap",
            "metallicMap",
            "roughnessMap",
            "AoMap"
            };
        for (int i = 0; i < types.size(); i ++) {
            TextureType type = types[i];
            std::string name = names[i];
            if (material->TextureMap.find(type) != material->TextureMap.end()) {
                std::string path = material->TextureMap[type];
                std::shared_ptr<OpenGLtexture2D> texture =
                OpenGLTextureManager::getInstance()->getOpenGLtexture2D(path);
                shader->SetUniformValue<int>(name,i);
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, texture->ID);
            }
        }
        // TODO @ Zhou, read other material in resources manager
        shader->SetUniformValue<float>("diffusion", 0.4);
        shader->SetUniformValue<float>("shininess", 3);
        shader->SetUniformValue<float>("lambertin", 0.3);
        shader->SetUniformValue<float>("specularPower", 64);

    }
    std::shared_ptr<FrameBuffer> OpenGLBackEnd::creatSenceFrameBuffer()
    {
        scenceFrameBuffer = std::make_shared<OpenGLFrameBuffer>();
        return scenceFrameBuffer;
    }

    void OpenGLBackEnd::drawInTexture(SPW::PostProcessingEffects effect)
    {
        SPW::ShaderHandle handle;
        if(effect==SPW::PostProcessingEffects::None)
        {
            handle = SPW::ShaderHandle({
                                               "drawIntexture",
                                               "./resources/shaders/screen.vert",
                                               "./resources/shaders/screen.frag"
                                       });
        }
        else if(effect==SPW::PostProcessingEffects::Gauss)
        {
            handle = SPW::ShaderHandle({
                                               "drawIntexture",
                                               "./resources/shaders/screen.vert",
                                               "./resources/shaders/Gauss.frag"
                                       });
        }
        else if(effect==SPW::PostProcessingEffects::FXAA)
        {
            handle = SPW::ShaderHandle({
                                               "drawIntexture",
                                               "./resources/shaders/screen.vert",
                                               "./resources/shaders/FXAA.frag"
                                       });
        }
        std::shared_ptr<Shader> screenShader = this->getShader(handle);
        screenShader->Bind();
        glActiveTexture(GL_TEXTURE0);
        screenShader->SetUniformValue<int>("screenTexture",0);
        glBindVertexArray(quadVAO);
        scenceFrameBuffer->drawinTexture();
        glBindVertexArray(0);
    }

    void OpenGLBackEnd::InitSkyBox()
    {
        float skyboxVertices[] = {
                // positions
                -1.0f,  1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                -1.0f,  1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f,  1.0f
        };
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        auto Handle = ShaderHandle({
                                            "skyBox",
                                            "./resources/shaders/cubeMap.vert",
                                            "./resources/shaders/cubeMap.frag"
                                    });
        skyShader = this->getShader(Handle);
        skyShader->Bind();
        skyShader->SetUniformValue<int>("skybox",0);
    }

    void OpenGLBackEnd::SetSkyBox(std::vector<std::string> &faces)
    {
        cubemapTexture = OpenGLTextureManager::getInstance()->getOpenGLCubeMap(faces);
    }

    void OpenGLBackEnd::drawSkyBox(glm::mat4 &V, glm::mat4 &P)
    {
        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyShader->Bind();// remove translation from the view matrix
        skyShader->SetUniformValue<int>("skybox",0);
        skyShader->setMat4("V", V);
        skyShader->setMat4("P", P);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture->ID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
    }
}
