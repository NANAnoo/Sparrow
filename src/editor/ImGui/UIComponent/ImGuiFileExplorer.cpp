/*
 * @date   2023-04-11
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#include "ImGuiFileExplorer.h"
#include "IO/FileSystem.h"
namespace SPW
{
	void SPW::ImGuiFileExplorer::Draw()
	{
		ImGui::Begin("Selected Folder");
		if (!selected_dir.empty())
		{
			DisplaySelectedFolder(selected_dir);
		}
		ImGui::End();

		DisplayImGuiFileExplorer(FileRoots::k_Root);
	}

	void SPW::ImGuiFileExplorer::DisplayImGuiFileExplorer(const std::string& path)
	{
		for (const auto& entry: fs::directory_iterator(FilePath(path)))
		{
			auto& entryPath = entry.path();
			if (fs::is_directory(entryPath))
			{
				std::string folderName = entryPath.filename().string();

				if (ImGui::TreeNode(folderName.c_str()))
				{
					DisplayImGuiFileExplorer(entryPath.string());
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

	void SPW::ImGuiFileExplorer::DisplaySelectedFolder(const std::string& folderPath)
	{
		// set row numbers and row width
		int numColumns = 4;
		ImGui::Columns(numColumns);

		for (const auto& entry: fs::directory_iterator(folderPath))
		{
			auto& entryPath = entry.path();
			std::string fileName = entryPath.filename().string();

			int64_t icon_id;

			if (fs::is_directory(entryPath))
			{
				icon_id = m_IconManager->GetLibIcon("folder");
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
					icon_id = m_IconManager->GetLibIcon("obj");
				else if (entryPath.extension() == ".wav")
					icon_id = m_IconManager->GetLibIcon("music");
				else if (entryPath.extension() == ".png" || entryPath.extension() == ".jpg"
					|| entryPath.extension() == ".jpeg" || entryPath.extension() == ".dds")
					icon_id = m_IconManager->GenerateIconID(entryPath.string());
				else
					icon_id = m_IconManager->GetLibIcon("file");

				if (ImGui::ImageButton(reinterpret_cast<void*>(icon_id), m_DefalutImageSize))
				{
					// selected_dir = entryPath.string();
				}

				if (ImGui::IsItemClicked() && (entryPath.extension() == ".json" || entryPath.extension() == ".lua" ))
				{
					std::string command = "code " + entryPath.string();

					if(std::system(command.c_str()))
						std::cerr << "Error: Failed to open the file in Visual Studio Code" << std::endl;
				}
			}

			ImGui::Text("%s", fileName.c_str());
			ImGui::Spacing();

			ImGui::NextColumn();
		}

		ImGui::Columns(1);
	}
}