#include "ImGui/ImGuiDefinitions.h"
#include <stb_image.h>

class ImGuiIcon
{
public:
    ImGuiIcon(const std::string& filename)
    {
        m_IconID = GenerateTextureID("./resources/texture/container.jpg");
    }

private:
    static uint32_t GenerateTextureID(const std::string& filename)
    {
        int width, height, numChannels;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &numChannels, 0);

        if (!data)
        {
            std::cerr << "Failed to load texture from file: " << filename << std::endl;
            return 0;
        }

        GLuint textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (numChannels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else if (numChannels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
        return textureId;
    }

public:
    uint32_t m_IconID;
    ImVec2 imageSize = ImVec2(20, 20);
};

