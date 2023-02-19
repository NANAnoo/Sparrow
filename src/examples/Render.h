//
// Created by sgsdi on 2023/2/19.
//

#ifndef SPARROW_RENDER_H
#define SPARROW_RENDER_H
#include "IO/AttriVertex.h"
#include "Render/shader.h"
#include "Render/VertexBuffer.h"
#include "glm/glm.hpp"
#include "vector"
#include "Render/RenderAPII.h"
#include "Platforms/OPENGL/OpenGLAPI.h"
#include "Render/IndexBuffer.h"
#include "Platforms/OPENGL/OpenGLIndexBuffer.h"
std::vector<SPW::AttribVertex> vertices;
std::vector<unsigned int> indices{0,1,3,1,2,3};

class Render
{
public:
    Render()
    {
        Ibuffer = std::make_shared<SPW::OpenGLIndexBuffer>(indices);
        Ibuffer->Bind();

        shader = SPW::Shader::Create("sb","./resources/shaders/simpleVs.vert","./resources/shaders/simplefrag.frag");
        VBuffer = SPW::VertexBufferI::CreateVertexBuffer();


        SPW::AttribVertex A1(glm::vec3(0.5,0.5,0),glm::vec4(0),glm::vec2(0),glm::vec3(0),glm::vec3(0));
        SPW::AttribVertex A2(glm::vec3(0.5,-0.5,0),glm::vec4(0),glm::vec2(0),glm::vec3(0),glm::vec3(0));
        SPW::AttribVertex A3(glm::vec3(-0.5,-0.5,0),glm::vec4(0),glm::vec2(0),glm::vec3(0),glm::vec3(0));
        SPW::AttribVertex A4(glm::vec3(-0.5,0.5,0),glm::vec4(0),glm::vec2(0),glm::vec3(0),glm::vec3(0));



        vertices.push_back(A1);
        vertices.push_back(A2);
        vertices.push_back(A3);
        vertices.push_back(A4);

        VBuffer->GenVertexArray();
        VBuffer->GenVertexBuffer();
        VBuffer->VertexBufferData(vertices);

        VBuffer->Bind();
        shader->Bind();
    }

    void render() const
    {
        SPW::RenderAPII* APII = new SPW::OpenGLAPI();
        APII->SetClearColor(glm::vec4(0.7,0.7,0.7,1.0f));
        APII->Clear();

        VBuffer->Bind();
        shader->Bind();
        APII->DrawElement(VBuffer,Ibuffer);
    }

private:
    std::shared_ptr<SPW::Shader> shader;
    std::shared_ptr<SPW::VertexBufferI> VBuffer;
    std::shared_ptr<SPW::IndexBuffer> Ibuffer;
};

#endif //SPARROW_RENDER_H
