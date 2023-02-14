//
// Created by ZhangHao on 2023/2/12.
//

#ifndef SPARROW_MOUSEEVENT_HPP
#define SPARROW_MOUSEEVENT_HPP

#include "Event/Event.h"

namespace SPW {
    class MouthEvent : public EventI {
    public:
        explicit MouthEvent(EventType type):_type(type){}
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
        void solveEvent(const std::shared_ptr<EventI> &e) final {
            e->dispatch<MouthDownType, MouthEvent>([this](MouthEvent *e){
                return onMouthDown(e);
            });
        }
        EventCategory listeningCategory() final {
            return MouthCategory;
        }
        virtual bool onMouthDown(MouthEvent *e) {return false;};
    };
}

#endif //SPARROW_MOUSEEVENT_HPP
