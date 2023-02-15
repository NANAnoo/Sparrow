//
// Created by ZhangHao on 2023/2/15.
//

#ifndef SPARROW_IDCOMPONENT_H
#define SPARROW_IDCOMPONENT_H

#include "../ComponentI.h"

#include "Utils/UUID.hpp"

namespace SPW {
    // give id to an entity
    class IDComponent : public ComponentI {
    public:
        IDComponent() = delete;
        const UUID &getID() {
            return id;
        }
    private:
        UUID id;
    };
}


#endif //SPARROW_IDCOMPONENT_H
