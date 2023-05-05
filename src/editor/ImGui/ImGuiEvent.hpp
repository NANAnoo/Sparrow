/*
* @date   2023-05-05
* @author dudu
* @brief  ${FILE_DESCRIPTION}
*/
#pragma once

#include "Event/Event.h"
#include "Event/EventDefines.h"
#include "Control/MouseEvent.hpp"
#include "Control/KeyEvent.hpp"

#include "ImGuiManager.hpp"

namespace SPW {
    class ImGuiMouseEventResponder : public MouseEventResponder {
    public:
        explicit ImGuiMouseEventResponder(const std::shared_ptr<EventResponderI> &p)
                : MouseEventResponder(p) {

        }

        virtual bool onMouseDown(MouseEvent *e) {
            ImGuiIO &io = ImGui::GetIO();

            bool ret = false;
        	if (e->type() == MouseDownType)
        	{
                // std::cout << "Mouse Down" << std::endl;
                io.MouseDown[(int)MouseCode::ButtonLeft] = true;
        	}

            if (e->type() == MouseReleasedType)
            {
                // std::cout << "Mouse Released" << std::endl;
                io.MouseDown[(int)MouseCode::ButtonLeft] = false;
            }

            return true;
        }

        virtual bool onMouseReleased(MouseEvent* e)
        {
            ImGuiIO& io = ImGui::GetIO();

        	if (e->type() == MouseReleasedType)
            {
                // std::cout << "Mouse Released" << std::endl;
                io.MouseDown[(int)MouseCode::ButtonLeft] = false;
            }

            return true;
        }

        virtual bool onMouseScroll(MouseEvent* e)
        {
            ImGuiIO& io = ImGui::GetIO();

            if (e->category() == MouseCategory && e->type() == MouseScrollType)
            {
                io.MouseWheel += (float)(e->scroll_offset);
            }

        	return true;
        }


    };

    class ImGuiKeyEventResponder : public KeyEventResponder {
    public:
        explicit ImGuiKeyEventResponder(const std::shared_ptr<EventResponderI>& p)
            : KeyEventResponder(p) {

        }

        // interfaces for responding key events:
        // key down
        virtual bool onKeyDown(KeyEvent* e)
        {
            ImGuiIO& io = ImGui::GetIO();

        	if (e->category() == KeyCategory && e->type() == KeyDownType)
            {
                io.AddInputCharacter((int)e->keycode);
            }

            return true;
        }

    	//key released
        virtual bool onKeyReleased(KeyEvent* e) { return false; };
        //key held
        virtual bool onKeyHeld(KeyEvent* e) { return false; };


    };
}