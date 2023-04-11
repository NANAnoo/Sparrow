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
			: ImGuiPanel("File Explorer Panel")
			, m_IconManager(iconManager)
		{ }

    protected:
		void Draw() override;
	private:
		void DisplayImGuiFileSystem(const std::string& path);
		void DisplaySelectedFolder(const std::string& folderPath);

    private:
		std::string selected_dir;
		ImGuiIconManager* m_IconManager;
		ImVec2 m_DefalutImageSize  = ImVec2(40, 40);
    };

}

