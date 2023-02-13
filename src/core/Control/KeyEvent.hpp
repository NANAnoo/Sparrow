//
// Created by ZhangHao on 2023/2/13.
//

#ifndef SPARROW_KEYEVENT_HPP
#define SPARROW_KEYEVENT_HPP

#include "Event/Event.hpp"

namespace SPW {
    class KeyEvent : public EventI {
    public:
        EventCategory category() override {return EventCategory::KeyCategory;}
    };

    class KeyEventResponder : public EventResponderI {
    public:
        explicit KeyEventResponder(std::shared_ptr<EventResponderI> parent) :
                EventResponderI(parent) {
        }
        void onEvent(EventI *e) final {
            e->consumed = onKeyDown((KeyEvent *)e);
            if (!e->consumed) {
                EventResponderI::onEvent(e);
            } else {
                DEBUG_EXPRESSION(e->processChain.emplace_back(getName()))
            }
        }
        EventCategory listeningCategory() final {
            return KeyCategory;
        }

        virtual bool onKeyDown(KeyEvent *e) = 0;
    };
}
#endif //SPARROW_KEYEVENT_HPP
