//
// Created by NANAnoo on 2/11/2023.
//

#ifndef SPARROW_MESSAGEDEFINES_H
#define SPARROW_MESSAGEDEFINES_H


#include "MessageCenter.hpp"

namespace SPW {
#define POST_MSG(msg) MessageCenter::getInstance()->postMessage(msg);
#define REGISTER_MSG(msg, handler) MessageCenter::getInstance()->observeMessage(msg, handler);
#define OBSERVE_MSG_ONCE(msg, handler) MessageCenter::getInstance()->observeOnce(msg, handler);

    const Message kMsgApplicationCreated= "kMsgApplicationCreated";
    const Message kMsgApplicationInited= "kMsgApplicationInited";
    const Message kMsgApplicationStopped= "kMsgApplicationStopped";
    const Message kMsgBeforeAppUpdate= "kMsgBeforeAppUpdate";
    const Message kMsgAfterAppUpdate= "kMsgAfterAppUpdate";
}

#endif //SPARROW_MESSAGEDEFINES_H
