//
// Created by sgsdi on 2023/2/17.
//

#include "OpenGLVertexBuffer.h"

void SPW::OpenGLVertexBuffer::GenVertexBuffer()
{
    glGenBuffers(1,&VBO);
}

void SPW::OpenGLVertexBuffer::GenVertexArray() {
    glGenVertexArrays(1,&VAO);
}

void SPW::OpenGLVertexBuffer::VertexBufferData(std::vector<AttribVertex> vertices)
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindVertexArray(VAO);
    auto x = sizeof(AttribVertex);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(AttribVertex), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AttribVertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AttribVertex), (void*)offsetof(AttribVertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(AttribVertex), (void*)offsetof(AttribVertex, texCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(AttribVertex), (void*)offsetof(AttribVertex, tangent));
    // vertex bitangent
    //glEnableVertexAttribArray(4);
    //glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(AttribVertex), (void*)offsetof(AttribVertex, Bitangent));
}
