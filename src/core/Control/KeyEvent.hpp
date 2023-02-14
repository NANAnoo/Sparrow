//
// Created by ZhangHao on 2023/2/13.
//

#ifndef SPARROW_KEYEVENT_HPP
#define SPARROW_KEYEVENT_HPP

#include "Event/Event.h"

namespace SPW {
    class KeyEvent : public EventI {
    public:
        explicit KeyEvent(EventType type):_type(type){}
        EventCategory category() final {return EventCategory::KeyCategory;}
        EventType type() final {return _type;}
    private:
        EventType _type = UnknownType;
    };

    class KeyEventResponder : public EventResponderI {
    public:
        explicit KeyEventResponder(std::shared_ptr<EventResponderI> parent) :
                EventResponderI(parent) {
        }
        void solveEvent(const std::shared_ptr<EventI> &e) final {
            // TODO dispatch event to different type
            e->dispatch<KeyDownType, KeyEvent>([this](KeyEvent *e){
                return onKeyDown(e);
            });
        }
        EventCategory listeningCategory() final {
            return KeyCategory;
        }

        // interfaces for responding key events:

        // key down
        virtual bool onKeyDown(KeyEvent *e) { return false;};

        // TODO more event type
        // ...

    };
}
#endif //SPARROW_KEYEVENT_HPP
