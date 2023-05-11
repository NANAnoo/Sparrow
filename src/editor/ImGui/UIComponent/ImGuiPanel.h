/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once

#include "../ImGuiDefinitions.h"

namespace SPW
{

	class ImGuiPanel
	{
	public:
		ImGuiPanel(std::string title, bool* open = nullptr, ImGuiWindowFlags flags = 0)
			: m_title(std::move(title)), m_open(open), m_windowFlags(flags)
		{ }

		virtual ~ImGuiPanel() = default;

		void Render()
		{
			Begin();
			Draw();
			End();
		}
	protected:
		virtual void Begin()
		{
			ImGui::Begin(m_title.c_str(), m_open , m_windowFlags);
		}

		virtual void Draw() = 0;

		virtual void End()
		{
			ImGui::End();
		}

	protected:
		std::string m_title;
		bool* m_open;
		ImGuiWindowFlags m_windowFlags;
	};

}
