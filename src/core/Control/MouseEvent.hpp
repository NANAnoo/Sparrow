//
// Created by ZhangHao on 2023/2/12.
//

#ifndef SPARROW_MOUSEEVENT_HPP
#define SPARROW_MOUSEEVENT_HPP

#include "Event/Event.h"

namespace SPW {
    class MouseEvent : public EventI {
    public:
        explicit MouseEvent(EventType type): _type(type){}
        EventCategory category() final {return EventCategory::MouseCategory;}
        EventType type() final {return _type;}
    private:
        EventType _type = UnknownType;
    };

    class MouseEventResponder : public EventResponderI {
    public:
        explicit MouseEventResponder(const std::shared_ptr<EventResponderI> &parent) :
                EventResponderI(parent) {
        }
        void solveEvent(const std::shared_ptr<EventI> &e) final {
            e->dispatch<MouseDownType, MouseEvent>(EVENT_RESPONDER(onMouseDown));
        }
        EventCategory listeningCategory() final {
            return MouseCategory;
        }
        virtual bool onMouseDown(MouseEvent *e) {return false;};
    };
}

#endif //SPARROW_MOUSEEVENT_HPP
