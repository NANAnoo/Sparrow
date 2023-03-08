//
// Created by ZhangHao on 2023/2/13.
//

#ifndef SPARROW_KEYEVENT_HPP
#define SPARROW_KEYEVENT_HPP

#include "Event/Event.h"
#include "KeyCodes.h"

namespace SPW {
    class KeyEvent : public EventI {
    public:
        KeyCode keycode;
        explicit KeyEvent(EventType type, KeyCode code):_type(type), keycode(code) {}
        EventCategory category() final {return EventCategory::KeyCategory;}
        EventType type() final {return _type;}
    private:
        EventType _type = UnknownType;
    };

    class KeyEventResponder : public EventResponderI {
    public:

        explicit KeyEventResponder(const std::shared_ptr<EventResponderI> &parent) :
                EventResponderI(parent) {
        }

        void solveEvent(const std::shared_ptr<EventI> &e) final {
            e->dispatch<KeyDownType, KeyEvent>(EVENT_RESPONDER(onKeyDown));
            e->dispatch<KeyHeldType, KeyEvent>(EVENT_RESPONDER(onKeyHeld));
            e->dispatch<KeyReleasedType, KeyEvent>(EVENT_RESPONDER(onKeyReleased));
        }
        EventCategory listeningCategory() final {
            return KeyCategory;
        }

        // interfaces for responding key events:
        // key down
        virtual bool onKeyDown(KeyEvent *e) { return false;};
        //key released
        virtual bool onKeyReleased(KeyEvent *e) { return false;};
        //key held
        virtual bool onKeyHeld(KeyEvent *e) { return false;};
    };
}
#endif //SPARROW_KEYEVENT_HPP
