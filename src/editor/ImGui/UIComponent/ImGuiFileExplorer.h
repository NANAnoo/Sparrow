/*
 * @date   2023-04-09
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once
#include "ImGuiPanel.h"
#include "imgui_internal.h"
#include "ImGui/ImGuiIconManager.hpp"
#include "ImGui/UIComponent/ImGuiPanel.h"
#include "IO/FileSystem.h"

namespace SPW
{
	class ImGuiFileExplorer : public ImGuiPanel
    {
    public:
		ImGuiFileExplorer(ImGuiIconManager* iconManager)
			: ImGuiPanel("File Explorer Panel"), m_IconManager(iconManager)
		{
			// doc_textureID = std::make_unique<ImGuiIconManager>("./resources/icons/doc.png");
			// file_textureID = std::make_unique<ImGuiIconManager>("./resources/icons/file.png");
			// obj_textureID = std::make_unique<ImGuiIconManager>("./resources/icons/obj.png");
			// music_textureID = std::make_unique<ImGuiIconManager>("./resources/icons/music.png");
		}

    protected:
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

		void DisplaySelectedFolder(const std::string& folderPath)
		{
			// set row numbers and row width
			int numColumns = 4;
			ImGui::Columns(numColumns);

			for (const auto& entry : fs::directory_iterator(folderPath))
			{
				auto& entryPath = entry.path();
				std::string fileName = entryPath.filename().string();

				int64_t icon_id;

				if (fs::is_directory(entryPath))
				{
					icon_id = ImGuiIconManager::GenerateTextureID(m_IconManager->m_IconIDMap, "./resources/icons/file.png");
					if (ImGui::ImageButton(reinterpret_cast<void*>(icon_id), m_DefalutImageSize))
					{
						// selected_dir = entryPath.string();
						// std::cout << "CLiked" << std::endl;
					}
					if (ImGui::IsItemClicked())
					{
						// std::cout << "CLiked" << std::endl;
						selected_dir = entryPath.string();
					}
				}
				else
				{

					if (entryPath.extension() == ".obj")
						icon_id = ImGuiIconManager::GenerateTextureID(m_IconManager->m_IconIDMap, "./resources/icons/obj.png");
					else if (entryPath.extension() == ".wav")
						icon_id = ImGuiIconManager::GenerateTextureID(m_IconManager->m_IconIDMap, "./resources/icons/music.png");
					else if (entryPath.extension() == ".png" || entryPath.extension() == ".jpg" || entryPath.extension() == ".jpeg")
						icon_id = ImGuiIconManager::GenerateTextureID(m_IconManager->m_IconIDMap, entryPath.string());
					else 
						icon_id = ImGuiIconManager::GenerateTextureID(m_IconManager->m_IconIDMap, "./resources/icons/doc.png");

					if (ImGui::ImageButton(reinterpret_cast<void*>(icon_id), m_DefalutImageSize))
					{
						// selected_dir = entryPath.string();
					}
				}

				ImGui::Text("%s", fileName.c_str()); ImGui::Spacing();


				ImGui::NextColumn();
			}

			ImGui::Columns(1);
		}

        void Draw() override
        {
			ImGui::Begin("Selected Folder");
			if (!selected_dir.empty())
			{
				DisplaySelectedFolder(selected_dir);
			}
			ImGui::End();

			ImGui::Begin("Button Test");
			if(ImGui::Button(" Test"))
			{
				std::cout << "Test \n";
			}
			ImGui::End();

			DisplayImGuiFileSystem("resources/");
        }

    private:
		std::string selected_dir;

		ImGuiIconManager* m_IconManager;

		ImVec2 m_DefalutImageSize  = ImVec2(40, 40);
    };

}

