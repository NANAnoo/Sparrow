/*
 * @date   2023-04-11
 * @author dudu
 * @brief  ImGui Window Alert Class, used to deal with window message, and show it on the screen, including file dialog, window alert...
 */
#pragma once

#include "ImGui/ImGuiDefinitions.h"

namespace SPW {

    class ImGuiMessageBox
    {
    public:
        ImGuiMessageBox(const char* title, const char* icon, const char* text, const char** captions, bool show_checkbox = false)
	        : m_Title(title)
    		, m_Icon()
    		, m_Text(text)
    		, m_Captions(captions)
    		, m_ShowCheckbox(show_checkbox)
    		, m_DontAskAgain(false)
    		, m_Selected(0)
        {   }

        // ~ImGuiMessageBox()

        void Trigger() const { ImGui::OpenPopup(m_Title); }

        int Exec();

    	void AskAgain();


    protected:
        const char* m_Title;
        const char* m_Icon;
        const char* m_Text;
        const char** m_Captions;
        bool m_ShowCheckbox;
        bool m_DontAskAgain;
        int m_Selected;
    };

}  // namespace SPW

