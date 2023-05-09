/*
* @date   2023-03-27
* @author dudu
* @brief  ${FILE_DESCRIPTION}
*/
#pragma once
#include "ImGuiDefinitions.h"
#include "ImGuiEvent.hpp"
#include "UIComponent/ImGuiDockSpace.h"
#include "UIComponent/ImGuiPanel.h"
#include "UIComponent/ImGuiMenuBar.h"
#include "UIComponent/ImGuiEntityPanel.h"
#include "UIComponent/ImGuiFileDialogPanel.h"
#include "UIComponent/ImGuiInspectorPanel.h"
#include "UIComponent/ImGuiTreeNodePanel.h"
#include "UIComponent/ImGuiImagePanel.h"
#include "UIComponent/ImGuiFileExplorer.h"
#include "UIComponent/ImGuiProfilingPanel.h"
#include "ImGui/ImGuiMessageBox/ImGuiMessageBox.h"
#include "Asset/ResourceManager/ResourceManager.h"
#include "Asset/Serializer/EntitySerializer.h"
#include "ImGui/IconsFontAwesome6.h"
#include "UIComponent/ImGuiLog.h"

namespace SPW
{
	class ImGuiImagePanel;

	class ImGuiManager : public EventResponderI
	{
	public:
		ImGuiManager(GLFWwindow* window, std::shared_ptr<Scene> scene, const std::shared_ptr<EventResponderI>& eventResponder)
					: windowHandle(window)
			        , m_Scene (scene)
					, EventResponderI(eventResponder)
		{
			Init(windowHandle);
		}

		~ImGuiManager()
		{
			//delete m_KeyResponder;
			//delete m_MouseResponder;

		}

		void Init(GLFWwindow* window);
		void Begin();
		void End();
		void CleanUp();
		void ShowDemoWindow(bool show_demo_window);
		void EnableViewport() const;
		GLFWwindow* GetWindowHandle() const {return windowHandle;}

		void RenderAllPanels() 
		{
			m_DockspacePanel->Render();
			DisplayDialog();
		}
		void CreateImagePanel(uint64_t renderID);
		std::shared_ptr<ImGuiEntityPanel>     GetEntityPanel() { return m_EntityPanel; }
		std::shared_ptr<ImGuiInspectorPanel>  GetInspectorPanel() { return m_InspectorPanel; }

		void FileDialogCallBack_1();
		void FileDialogCallBack_2();
		void FileDialogCallBack_3();
		void FileDialogCallBack_4();


		void DisplayDialog() const;
		void loadDefaultLayout() const;

		void Render()
		{
			Begin();
			//----------------------------------------------------------------------------------------
			CreateImagePanel(m_Scene->m_renderSystem.lock()->getTextureID());

			RenderAllPanels();
			//----------------------------------------------------------------------------------------
			m_EntityPanel->SetActiveScene(m_Scene);
			m_InspectorPanel->SetActiveScene(m_Scene);

			m_Scene->forEachEntity<IDComponent>([this](const Entity& e)
			{
				const auto component_name = e.component<NameComponent>()->getName();
				const auto component_id = e.component<IDComponent>()->getID().toString();
				m_EntityPanel->AddMenuItem(component_id, component_name, [&, e]()
				{
					m_InspectorPanel->SetSelectedGameObject(e);
					// if(ImGui::Button("xxxx"))
					// {
					// 	m_InspectorPanel->SetNoneSelectedGameObject();
					// }
				}
				);
			});

			//----------------------------------------------------------------------------------------
			End();
			EnableViewport();
		}

		void solveEvent(const std::shared_ptr<EventI>& e)
		{

			ImGuiIO& io = ImGui::GetIO();

			e->dispatch<MouseScrollType, MouseEvent>([&io](const MouseEvent* e)
			{

				io.MouseWheel += (float)(e->scroll_offset);

				return false;
			});

			e->dispatch<MouseDownType, MouseEvent>([&io](const MouseEvent* e)
			{
				io.MouseDown[(int)MouseCode::ButtonLeft] = true;

				return false;
			});

			e->dispatch<MouseReleasedType, MouseEvent>([&io](const MouseEvent* e)
			{
				io.MouseDown[(int)MouseCode::ButtonLeft] = false;

				return false;
			});

			e->dispatch<KeyInputType, KeyEvent>([&io](const KeyEvent* e)
			{
				io.AddInputCharacter((int)e->keycode);

				return false;
			});
		}

	private:
		void InitLayout();
		void InitIconManager();
		void InitMenuBar();
		void InitEntityPanel();
		void InitInspectorPanel();
		void InitFileExplorer();
		void InitLogPanel();


	private:
		std::shared_ptr<ImGuiDockSpace>			m_DockspacePanel;
		std::shared_ptr<ImGuiEntityPanel>		m_EntityPanel;
		std::shared_ptr<ImGuiMenuBar>			m_MainMenuBar;
		std::shared_ptr<ImGuiInspectorPanel>	m_InspectorPanel;
		std::shared_ptr<ImGuiImagePanel>	    m_ImagePanel;
		std::shared_ptr<ImGuiFileExplorer>      m_FileExplorer;
		std::shared_ptr<ImGuiFileDialogPanel>   m_FileDialogPanel;
		std::shared_ptr<ImGuiProfilingPanel>    m_ProfilingPanel;
		//to do:delete
		std::shared_ptr<ImGuiLog>				m_LogPanel;

		std::shared_ptr<Scene>				    m_Scene;

		//file dialog panel
		SharedPtr<ImGuiFileDialog> file_dialog;

		std::unique_ptr<ImGuiMessageBox> importModel_MessageBox;
		std::unique_ptr<ImGuiMessageBox> textureCompression_MessageBox;

		bool show_demo_window = false;
		GLFWwindow* windowHandle;
		std::unique_ptr<ImGuiIconManager>		m_ImguiIconManager;
		std::uint32_t m_FileDialogID ;
		ImFont* font = nullptr;
    };

}
