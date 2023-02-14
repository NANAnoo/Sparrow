//
// Created by NANAnoo on 2/11/2023.
// define event structure , producer and consumer interface
//

#ifndef SPARROW_EVENT_H
#define SPARROW_EVENT_H

#include "Utils/MacroUtils.h"
#include "EventDefines.h"
#include "Utils/SingletonBase.h"

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>

namespace SPW {
    class Application;
    class EventResponderI;
#define EVENT_RESPONDER(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

    class EventI {
    public:
        EventI() = default;
        EventI(const EventI& other) {
            consumed = other.consumed;
        }
        using EventHanlder = std::function<void(const std::shared_ptr<EventI> &e)>;
        virtual EventType type() {return UnknownType;}
        virtual EventCategory category() {return UnknownCategory;}
        bool isIn(EventCategory c) {
            return (category() & c) > 0;
        }
        const char *name() {
            return toString(this->type());
        }

        template<typename T>
        using EventFunc = std::function<bool(T *)>;
        // dispatcher
        template<EventType type, typename T>
        void dispatch(EventFunc<T> func) {
            if (!this->consumed && this->type() == type) {
                T *te = (T *)this;
                this->consumed = func(te);
            }
        }
    private:
        bool consumed = false;
        DEBUG_PROPERTY(std::vector<std::vector<std::string>> processChain = {})
        friend EventResponderI;
        friend Application;
        friend std::ostream &operator<<(std::ostream &os, EventI *e);
    };

    class EventResponderI {
    public:
        EventResponderI() = default;
        explicit EventResponderI(std::shared_ptr<EventResponderI> &p) {
            if (p) {
                p->subResponders.insert(this);
                parent = p;
            }
        }
        ~EventResponderI() {
            if (!parent.expired()) {
                auto strong_p = parent.lock();
                strong_p->subResponders.erase(this);
            }
        }
        virtual const char* getName() {return "Unknown";}
        virtual void solveEvent(const std::shared_ptr<EventI> &e) {}

        virtual EventCategory listeningCategory() {return AllCategory;}
    private:
        std::unordered_set<EventResponderI*> subResponders;
        std::weak_ptr<EventResponderI> parent;

        // private logic
        void onEvent(const std::shared_ptr<EventI> &e) {
            // use for debugging
            if (e->consumed) return;
            for (auto sub : subResponders) {
                // pass through event
                if (!e->consumed) {
                    sub->onEvent(e);
                }
            }
            if (subResponders.empty() && e->isIn(listeningCategory())) {
                // dispatch event from leaf
                DEBUG_EXPRESSION(e->processChain.emplace_back();)
                dispatchEvent(e);
            }
        }
        void dispatchEvent(const std::shared_ptr<EventI> &e) {
            DEBUG_EXPRESSION(e->processChain.back().emplace_back(getName());)
            solveEvent(e);
            if (!e->consumed && !parent.expired()) {
                // using parent to dispatch event
                parent.lock()->dispatchEvent(e);
            } else {
                DEBUG_EXPRESSION(if (e->consumed)std::cout << e << std::endl;)

            }
        }
    friend Application;
    };
}

#endif //SPARROW_EVENT_H
