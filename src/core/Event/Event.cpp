//
// Created by ZhangHao on 2023/2/13.
//

#include "Event.h"

namespace SPW {
    std::ostream &operator<<(std::ostream &os, EventI *e) {
        std::cout << "-----------------------"<< std::endl;
        std::cout << "[" << e->name() << "]:" << std::endl;
        DEBUG_EXPRESSION(
            for (int i = 0; i < e->processChain.size();i++) {
                std::cout << "    [Chain " << i + 1 << "]:";
                for (auto &name:e->processChain[i]) {
                    std::cout << name << "->";
                }
                if (i < e->processChain.size() - 1)
                    std::cout << "[not consumed]" << std::endl;
            }
        )
        std::cout << (e->consumed ? "[consumed]" : "[not consumed]");
        return os;
    }
}
