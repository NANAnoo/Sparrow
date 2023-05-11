//
// Created by 29134 on 2023/4/7.
//
#pragma once
#include "Event/Event.h"
#include "Control/MouseEvent.hpp"

namespace SPW{
    class UIResponder : public EventResponderI{
    public:
        explicit UIResponder(const std::shared_ptr<EventResponderI> &p) : EventResponderI(p){}

        const char* getName() override{
            return "SPWUIEvent";
        }

        bool canRespondTo(const std::shared_ptr<EventI> &e) override{
            auto mouse_event = std::dynamic_pointer_cast<MouseEvent>(e);

            if(!mouse_event)
                return false;

            if((mouse_event->cursor_xpos > this->pos_x && mouse_event->cursor_xpos < this->pos_x + this->width) && (mouse_event->window_height - mouse_event->cursor_ypos > this->pos_y && mouse_event->window_height - mouse_event->cursor_ypos < this->pos_y + this->height))
                return true;

            return false;
        }

        EventCategory listeningCategory() override{
            return MouseCategory;
        }

        int rootDepth = 0;

    protected:
        int pos_x;
        int pos_y;
        int width;
        int height;
        int depth = 1;
        int bias = 4;

        friend Scene;
    };
}