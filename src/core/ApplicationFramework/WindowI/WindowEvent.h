//
// Created by ZhangHao on 2023/2/13.
//
#pragma once

#include "Event/Event.h"
#include "glm/glm/glm.hpp"

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
        void solveEvent(const std::shared_ptr<EventI> &e) final {
            // TODO dispatch event to different type
            e->dispatch<WindowResizeType, WindowEvent>(EVENT_RESPONDER(onWindowResize));
            e->dispatch<WindowFrameResizeType, WindowEvent>(EVENT_RESPONDER(onFrameResize));
            e->dispatch<WindowCloseType, WindowEvent>(EVENT_RESPONDER(onWindowClosed));
        }
        // dispatch event to different interface
        bool onWindowResize(WindowEvent *e) {
            return onWindowResize(e->width, e->height);
        }
        bool onFrameResize(WindowEvent *e) {
            return onFrameResize(e->width, e->height);
        }
        bool onWindowClosed(WindowEvent *e) {
            return onWindowClosed();
        }
    public:
        explicit WindowEventResponder(std::shared_ptr<EventResponderI> parent) :
            EventResponderI(parent){}

        virtual bool onWindowResize(int w, int h) {return false;}
        virtual bool onFrameResize(int w, int h) {return false;}
        virtual bool onWindowClosed() {return false;}
    };
}


