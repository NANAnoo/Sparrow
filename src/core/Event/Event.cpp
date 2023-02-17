//
// Created by ZhangHao on 2023/2/13.
//

#include "Event.h"

namespace SPW {
    std::ostream &operator<<(std::ostream &os, EventI *e) {
        std::cout << "[" << e->name() << "]:" << std::endl;
        DEBUG_EXPRESSION(
            for (const auto & name : e->processChain) {
                std::cout << name << "->";
            }
        )
        std::cout << (e->consumed ? "[consumed]" : "[not consumed]") << std::endl;
        return os;
    }
}
