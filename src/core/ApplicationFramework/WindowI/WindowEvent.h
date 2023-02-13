//
// Created by ZhangHao on 2023/2/13.
//
#pragma once

#include "Event/Event.hpp"
#include <glm/glm.hpp>

namespace SPW {
    class WindowEvent : public EventI {
    public:
        explicit WindowEvent(EventType t, int w, int h):_type(t), width(w), height(h){}
        int width;
        int height;
        EventCategory category() final {return EventCategory::WindowCategory;}
        EventType type() final {return _type;}
    private:
        EventType _type;
    };

    class WindowEventResponder : public EventResponderI {
        EventCategory listeningCategory() final {
            return WindowCategory;
        }
        void onEvent(EventI *e) final {
            // TODO dispatch event to different type
            e->dispatch<WindowResizeType, WindowEvent>(EVENT_RESPONDER(onWindowResize));
            if (!e->consumed) {
                EventResponderI::onEvent(e);
            } else {
                DEBUG_EXPRESSION(e->processChain.emplace_back(getName()))
            }
        }
        // dispatch event to different interface
        bool onWindowResize(WindowEvent *e) {
            return onWindowResize(e->width, e->height);
        }
    public:
        explicit WindowEventResponder(std::shared_ptr<EventResponderI> parent) :
            EventResponderI(parent){}

        virtual bool onWindowResize(int w, int h) = 0;
    };
}


