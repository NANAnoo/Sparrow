//
// Created by ZhangHao on 2023/2/12.
//

#ifndef SPARROW_MOUSEEVENT_HPP
#define SPARROW_MOUSEEVENT_HPP

#include "Event/Event.h"
#include "MouseCodes.h"

namespace SPW {
    class MouseEvent : public EventI {
    public:
        MouseCode button_code;
        double scroll_offset;
        double cursor_xpos_bias;
        double cursor_ypos_bias;
        double cursor_xpos;
        double cursor_ypos;
        int window_width;
        int window_height;
        explicit MouseEvent(EventType type, MouseCode code, double x_pos, double y_pos):
                _type(type), button_code(code), cursor_xpos(x_pos), cursor_ypos(y_pos){}
        explicit MouseEvent(EventType type, double x_pos, double y_pos, double x_bias, double y_bias, int win_width, int win_height):
                _type(type), cursor_xpos(x_pos), cursor_ypos(y_pos), cursor_xpos_bias(x_bias), cursor_ypos_bias(y_bias), window_width(win_width), window_height(win_height){}
        explicit MouseEvent(EventType type, MouseCode code, double x_pos, double y_pos, double x_bias, double y_bias, int win_width, int win_height):
                 _type(type), button_code(code), cursor_xpos(x_pos), cursor_ypos(y_pos), cursor_xpos_bias(x_bias), cursor_ypos_bias(y_bias), window_width(win_width), window_height(win_height){}
        EventCategory category() final {return EventCategory::MouseCategory;}
        EventType type() final {return _type;}
    private:
        EventType _type = UnknownType;
    };

    class MouseEventResponder : public EventResponderI {
    public:
        void solveEvent(const std::shared_ptr<EventI> &e) final {
            e->dispatch<MouseDownType, MouseEvent>(EVENT_RESPONDER(onMouseDown));
            e->dispatch<MouseHeldType, MouseEvent>(EVENT_RESPONDER(onMouseHeld));
            e->dispatch<MouseReleasedType, MouseEvent>(EVENT_RESPONDER(onMouseReleased));
            e->dispatch<MouseScrollType, MouseEvent>(EVENT_RESPONDER(onMouseScroll));
            e->dispatch<CursorMovementType, MouseEvent>(EVENT_RESPONDER(cursorMovement));
        }
        EventCategory listeningCategory() final {
            return MouseCategory;
        }

        const char *getName() override {
            return "SPWMouseEvent";
        }

        explicit MouseEventResponder(const std::shared_ptr<EventResponderI> &parent) :
                EventResponderI(parent) {
        }

        virtual bool onMouseDown(MouseEvent *e) {return false;};
        virtual bool onMouseHeld(MouseEvent *e) {return false;};
        virtual bool onMouseReleased(MouseEvent *e) {return false;};
        virtual bool onMouseScroll(MouseEvent *e) {return false;};
        virtual bool cursorMovement(MouseEvent *e) {return false;};
    };
}

#endif //SPARROW_MOUSEEVENT_HPP
