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
#include <stb_image.h>

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
		void DisplayImGuiFileExplorer(const std::string& path);
		void DisplaySelectedFolder(const std::string& folderPath);
		int64_t LoadImage(const std::string& file_path);
		void DisplayImagePanel();

    private:
		std::string selected_dir;
		ImGuiIconManager* m_IconManager;
		ImVec2 m_DefalutImageSize  = ImVec2(40, 40);
		GLuint m_OpenGLTexture = 0;
		bool m_ShowImagePanel = false;
    };

}

