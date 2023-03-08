//
// Created by 惠普 on 2023/3/2.
//

#ifndef SPARROW_AUDIOLISTENER_H
#define SPARROW_AUDIOLISTENER_H

#include "../ComponentI.h"
#include "fmod.hpp"
#include "string"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace SPW
{
    class AudioListener : public ComponentI{

    public:
        AudioListener() = default;

        int Listener_id;

    };



};
#endif //SPARROW_AUDIOLISTENER_H
