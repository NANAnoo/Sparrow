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

#include "ImGui/IconsFontAwesome6.h"

namespace SPW
{
	class ImGuiFileExplorer : public ImGuiPanel
    {
    public:
		ImGuiFileExplorer(ImGuiIconManager* iconManager)
			: ImGuiPanel(ICON_FA_DATABASE"  File Explorer Panel",nullptr,ImGuiWindowFlags_NoMove)
			, m_IconManager(iconManager)
		{ }

    protected:
		void Draw() override;
	private:
		void DisplayImGuiFileExplorer(const char* icon, const std::string& path);
		void DisplaySelectedFolder(const std::string& folderPath);
		int64_t LoadImage(const std::string& file_path);
		void DisplayImagePanel();

	protected:
		void SetLuaPathCallback(const std::filesystem::path& path);
		void DisplayLuaSettingMenu();
		std::string luaPath;

    private:
		std::string selected_dir;
		ImGuiIconManager* m_IconManager;
		ImVec2 m_DefalutImageSize  = ImVec2(40, 40);
		GLuint m_OpenGLTexture = 0;
		bool m_ShowImagePanel = false;
    };

}

