/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once
#include "ImGui/ImGuiDefinitions.h"
#include <stb_image.h>

namespace SPW
{

	inline const std::unordered_map<std::string, std::string> k_IconLib
	{
		{"file", "./resources/icons/doc.png"},
		{"folder", "./resources/icons/file.png"},
		{"obj", "./resources/icons/obj.png"},
		{"music", "./resources/icons/music.png"}
	};

	class ImGuiIconManager
	{
	public:
		ImGuiIconManager()
		{
			for(const auto&[k, v] : k_IconLib)
			{
				GenerateTextureID(v);
			}
		}

		int64_t GetLibIcon(const char* name)
		{
			return GenerateTextureID(k_IconLib.find(name)->second);
		}

		int64_t GenerateTextureID(const std::string& filename)
	    {
	        if(m_IconIDMap.contains(filename)) { return m_IconIDMap[filename]; }

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

	        // glGenerateMipmap(GL_TEXTURE_2D);

	        stbi_image_free(data);

	        m_IconIDMap[filename] = textureId;

    		return textureId;
	    }


		std::unordered_map<std::string, int64_t> m_IconIDMap;
		ImVec2 imageSize = ImVec2(40, 40);
	};

}