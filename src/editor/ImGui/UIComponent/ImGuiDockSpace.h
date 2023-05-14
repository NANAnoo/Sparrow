/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once
#include "../ImGuiDefinitions.h"
#include "ImGuiPanel.h"
#include "ApplicationFramework/WindowI/WindowEvent.h"
#include <ostream>
#include <iostream>

namespace SPW
{
	class ImGuiDockSpace final
	{
	public:
		ImGuiDockSpace()
		{
			docspace_Open = true;
			opt_fullscreen = true;
			opt_padding = false;
			dockspace_flags = ImGuiDockNodeFlags_None;
		}

		void Begin()
		{
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar /*|ImGuiWindowFlags_MenuBar*/;
			if (opt_fullscreen)
			{
				const ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(viewport->WorkPos);
				ImGui::SetNextWindowSize(viewport->WorkSize);
				ImGui::SetNextWindowViewport(viewport->ID);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			}
			else
			{
				dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
			}

			// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
			// and handle the pass-thru hole, so we ask Begin() to not render a background.
			if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
				window_flags |= ImGuiWindowFlags_NoBackground;

			// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
			// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
			// all active windows docked into it will lose their parent and become undocked.
			// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
			// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
			if (!opt_padding)
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

			ImGui::Begin("DockSpace Demo", &docspace_Open, window_flags);
		}

		void Render()
		{
			Begin();

			// Submit the DockSpace
			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			}


			for (auto const& p : m_Panels)
			{
				p->Render();
			}

			End();
		}

		void End()
		{

			if (!opt_padding)
				ImGui::PopStyleVar();

			if (opt_fullscreen)
				ImGui::PopStyleVar(2);

			ImGui::End();
		}

		void AddPanel(const std::shared_ptr<ImGuiPanel>& panel)
		{
			m_Panels.emplace_back(panel);
		}

		std::vector<std::shared_ptr<ImGuiPanel>> GetPanels()
		{
			return m_Panels;
		}

	private:
		std::vector<std::shared_ptr<ImGuiPanel>> m_Panels;
		bool docspace_Open;
		bool opt_fullscreen;
		bool opt_padding;
		ImGuiDockNodeFlags dockspace_flags;
	};
}
