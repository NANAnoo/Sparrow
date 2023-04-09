#pragma once
#include "ImGuiPanel.h"
#include "ImGui/UIComponent/ImGuiPanel.h"
#include "IO/FileSystem.h"

namespace SPW {

    class ImGuiFileExplorer : public ImGuiPanel
    {
    public:
        ImGuiFileExplorer():ImGuiPanel("File Explorer Panel")
        {
			image_size = ImVec2(64, 64);

			doc_textureID = generateTextureID("./resources/icons/doc.png");
			file_textureID = generateTextureID("./resources/icons/file.png");
			obj_textureID = generateTextureID("./resources/icons/obj.png");
			music_textureID = generateTextureID("./resources/icons/music.png");

        }

    protected:
		inline uint32_t generateTextureID(const std::string& filename)
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

			//glGenerateMipmap(GL_TEXTURE_2D);

			stbi_image_free(data);
			return textureId;
		}

		void DisplayImGuiFileSystem(const std::string& path)
		{
			for (const auto& entry : fs::directory_iterator(FilePath(path)))
			{
				auto& entryPath = entry.path();
				if (fs::is_directory(entryPath))
				{
					std::string folderName = entryPath.filename().string();

					if (ImGui::TreeNode(folderName.c_str()))
					{
						DisplayImGuiFileSystem(entryPath.string());
						ImGui::TreePop();
					}

					if (ImGui::IsItemClicked())
					{
						selected_dir = entryPath.string();
					}
				}
				else
				{
					std::string fileName = entryPath.filename().string();
					ImGui::Text("%s", fileName.c_str());
				}
			}
		}

		inline void DisplaySelectedFolder(const std::string& folderPath)
		{
			// 设置列数和列宽度
			int numColumns = 4;
			ImGui::Columns(numColumns);

			for (const auto& entry : fs::directory_iterator(folderPath))
			{
				auto& entryPath = entry.path();
				std::string fileName = entryPath.filename().string();

				if (fs::is_directory(entryPath))
				{
					// glBindTexture(GL_TEXTURE_2D, dir_textureID);
					// ImGui::Image((ImTextureID)(intptr_t)dir_textureID, image_size); ImGui::SameLine();

					if (ImGui::ImageButton((ImTextureID)(intptr_t)file_textureID, image_size))
						// if(ImGui::Button(fileName.c_str()))
					{
						selected_dir = entryPath.string();
					}
					ImGui::Text("%s", fileName.c_str());

					ImGui::Spacing();
				}
				else
				{
					uint32_t icon_id;

					if (entryPath.extension() == ".obj") icon_id = obj_textureID;
					else if (entryPath.extension() == ".wav") icon_id = music_textureID;
					else if (entryPath.extension() == ".png" || entryPath.extension() == ".jpg")
					{
						//TODO:材质的压缩或提前加载
						icon_id = generateTextureID(entryPath.string());
					}
					else icon_id = doc_textureID;

					if (ImGui::ImageButton((ImTextureID)(intptr_t)icon_id, image_size))
					{
						// selected_dir = entryPath.string();
					}

					ImGui::Text("%s", fileName.c_str());

					ImGui::Spacing();
				}

				// 添加下一列
				ImGui::NextColumn();
			}

			// 重置列
			ImGui::Columns(1);

		}

        void Draw() override
        {
            DisplayImGuiFileSystem("resources/");

			ImGui::Begin("Selected Folder");
			if (!selected_dir.empty())
			{
				DisplaySelectedFolder(selected_dir);
			}
			ImGui::End();

        }

    private:
    	std::string selected_dir;

		uint32_t doc_textureID;
		uint32_t file_textureID;
		uint32_t obj_textureID;
		uint32_t music_textureID;

    	ImVec2 image_size ;

    };

}

