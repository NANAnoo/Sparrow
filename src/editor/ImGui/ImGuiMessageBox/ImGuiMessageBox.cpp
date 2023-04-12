/*
 * @date   2023-04-11
 * @author dudu
 * @brief  ImGui Window Alert Class, used to deal with window message, and show it on the screen, including file dialog, window alert...
 */
#include "ImGuiMessageBox.h"
namespace SPW
{
	int ImGuiMessageBox::Exec()
	{
		int index = 0;

		if (ImGui::BeginPopupModal(m_Title, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (m_DontAskAgain && m_Selected != 0)
			{
				ImGui::CloseCurrentPopup();
				index = m_Selected;
			}
			else
			{
				if (m_Icon != nullptr)
				{
					const ImVec2 size = ImGui::CalcTextSize(m_Icon);
					ImVec2 pos = ImGui::GetCursorPos();
					const float save_y = pos.y;
					pos.y += size.y;

					ImGui::SetCursorPos(pos);
					ImGui::Text("%s", m_Icon);

					pos.x += size.x + pos.x;
					pos.y = save_y;

					ImGui::SetCursorPos(pos);
					ImGui::TextWrapped("%s", m_Text);
				}
				else
				{
					ImGui::TextWrapped("%s", m_Text);
				}

				ImGui::Separator();

				if (m_ShowCheckbox)
				{
					ImGui::Checkbox("Don't ask me again", &m_DontAskAgain);
				}

				ImVec2 size = ImVec2(50.0f, 0.0f);
				int count;

				for (count = 0; m_Captions[count] != nullptr; count++)
				{
					if (ImGui::Button(m_Captions[count], size))
					{
						index = count + 1;
						ImGui::CloseCurrentPopup();
						break;
					}

					ImGui::SameLine();
				}

				size = ImVec2((4 - count) * 50.0f, 1.0f);
				ImGui::Dummy(size);

				if (m_DontAskAgain)
				{
					m_Selected = index;
				}
			}

			ImGui::EndPopup();
		}

		return index;
	}


	void ImGuiMessageBox::AskAgain()
	{
		m_DontAskAgain = false;
		m_Selected = 0;
	}

}
