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
//#include "GameWrapper.hpp"
// #include ""

namespace SPW
{
	enum class FeatureType
	{
		None,

		ImportModel,
		ImportAudio,
		ImageCompression,

		SetupScriptEntry,

		SaveAsset,
		LoadAsset,

		SaveScene,
		LoadScene,

		Max,
	};

	class ImGuiImagePanel;

	class ImGuiManager : public EventResponderI
	{
	public:
		ImGuiManager(GLFWwindow* window, const std::shared_ptr<Scene>& scene, const std::shared_ptr<EventResponderI>& eventResponder)
					: m_Window(window)
			        , m_Scene (scene)
					, EventResponderI(eventResponder)
		{
			Init(m_Window);
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
		GLFWwindow* GetWindowHandle() const {return m_Window;}

		void RenderAllPanels() 
		{
			m_Dockspace->Render();
			DisplayDialog();
		}

		void CreateImagePanel(uint64_t renderID);
		std::shared_ptr<ImGuiEntityPanel>     GetEntityPanel() { return m_EntityPanel; }
		std::shared_ptr<ImGuiInspectorPanel>  GetInspectorPanel() { return m_InspectorPanel; }

		void SaveSceneCallback();
		void LoadSceneCallback();

		void SetupScriptEntryCallback();
		void ImportModelCallback();
		void LoadAssetCallback();
		void ImageCompressedCallback();
		void ImportAudioCallback();
		void SaveEditorLayoutCallback();
		void OpenConfigCallback();

		void DisplayDialog() ;

		void SaveEditorLayout() const;
		void LoadDefaultLayout() const;

		void Render();

		void solveEvent(const std::shared_ptr<EventI>& e) override
		{

			ImGuiIO& io = ImGui::GetIO();

			e->dispatch<MouseScrollType, MouseEvent>([&io](const MouseEvent* e)
			{
				io.MouseWheel += (float)(e->scroll_offset);

				return false;
			});

			e->dispatch<MouseDownType, MouseEvent>([&io](const MouseEvent* e)
			{
				io.MouseDown[(int)e->button_code] = true;
				
				return false;
			});

			e->dispatch<MouseReleasedType, MouseEvent>([&io](const MouseEvent* e)
			{
				io.MouseDown[(int)e->button_code] = false;

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
		void InitImagePanel();
		void InitIconManager();
		void InitMenuBar();
		void InitEntityPanel();
		void InitInspectorPanel();
		void InitFileExplorer();
		void InitProfilingPanel();

	private:
		std::shared_ptr<ImGuiDockSpace>			m_Dockspace;
		// file dialog panel
		std::shared_ptr<ImGuiFileDialog>		m_FileDialog;

		FeatureType m_Feature = FeatureType::None;

		std::shared_ptr<ImGuiEntityPanel>		m_EntityPanel;
		std::shared_ptr<ImGuiMenuBar>			m_MainMenuBar;
		std::shared_ptr<ImGuiInspectorPanel>	m_InspectorPanel;
		std::shared_ptr<ImGuiImagePanel>	    m_ImagePanel;
		std::shared_ptr<ImGuiFileExplorer>      m_FileExplorer;
		std::shared_ptr<ImGuiFileDialogPanel>   m_FileDialogPanel;
		std::shared_ptr<ImGuiProfilingPanel>    m_ProfilingPanel;

		const std::shared_ptr<Scene>			m_Scene;
		GLFWwindow* m_Window;
		bool showInputBox = false;
		std::string selectedFile;


		std::unique_ptr<ImGuiMessageBox> m_ImportModelMessageBox;
		std::unique_ptr<ImGuiMessageBox> m_TextureCompressionMessageBox;
		std::unique_ptr<ImGuiIconManager> m_ImguiIconManager;
    };

}
