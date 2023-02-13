//
// Created by ZhangHao on 2023/2/12.
//

#ifndef SPARROW_MOUSEEVENT_HPP
#define SPARROW_MOUSEEVENT_HPP

#include "Event/Event.hpp"

namespace SPW {
    class MouthEvent : public EventI {
    public:
        EventCategory category() final {return EventCategory::MouthCategory;}
        EventType type() final {return _type;}
    private:
        EventType _type = UnknownType;
    };

    class MouthEventResponder : public EventResponderI {
    public:
        explicit MouthEventResponder(std::shared_ptr<EventResponderI> parent) :
                EventResponderI(parent) {
        }
        void onEvent(EventI *e) final {
            e->consumed = onMouthDown((MouthEvent *)e);
            if (!e->consumed) {
                EventResponderI::onEvent(e);
            } else {
                DEBUG_EXPRESSION(e->processChain.emplace_back(getName()))
            }
        }
        EventCategory listeningCategory() final {
            return MouthCategory;
        }
        virtual bool onMouthDown(MouthEvent *e) = 0;
    };
}

#endif //SPARROW_MOUSEEVENT_HPP
