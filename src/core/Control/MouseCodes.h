//
// Created by 29134 on 2023/2/21.
//

#ifndef SPARROW_MOUSECODES_H
#define SPARROW_MOUSECODES_H

#include <cstdint>

namespace SPW {
    typedef enum class MouseCode : uint16_t {
        // From glfw3.h
        Button0 = 0,
        Button1 = 1,
        Button2 = 2,
        Button3 = 3,
        Button4 = 4,
        Button5 = 5,
        Button6 = 6,
        Button7 = 7,

        ButtonLast = Button7,
        ButtonLeft = Button0,
        ButtonRight = Button1,
        ButtonMiddle = Button2
    } Mouse;
}

#endif //SPARROW_MOUSECODES_H
