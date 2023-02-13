//
// Created by NANAnoo on 2/11/2023.
//

#ifndef SPARROW_MESSAGECENTER_HPP
#define SPARROW_MESSAGECENTER_HPP

#include "SingletonBase.h"
#include <unordered_map>
#include <string>
#include <functional>

namespace SPW {
    using Message = const char *;
    using MessageHandler = std::function<void(Message)>;
    class MessageCenter : public SingletonBase<MessageCenter>{
    public:
        // notify all handler when a msg is posted
        void postMessage(Message msg) {
            auto handlers = this->handlerMap[msg];
            for (auto &handler : handlers) {
                handler.second(msg);
            }
        }

        // observe a message once, handler will be deleted after being triggered
        void observeOnce(Message msg, const MessageHandler& handler) {
            std::pair<int, MessageHandler> p = getHandlerPair(handler);
            int id = p.first;
            p.second = [&handler, this, id](Message msg){
                handler(msg);
                handlerMap[msg].erase(id);
            };
            handlerMap[msg].insert(p);
        }

        // observe a message forever
        void observeMessage(Message msg, const MessageHandler& handler) {
            handlerMap[msg].insert(getHandlerPair(handler));
        }
    private:
        // generate a pair from a self-increased id
        static std::pair<int, MessageHandler> getHandlerPair(const MessageHandler& handler) {
            static int id = 0;
            return {id++, handler};
        }
    protected:
        // store all callbacks that register to a message
        // using another map as value for delete a handler
        std::unordered_map<std::string , std::unordered_map<int, MessageHandler>> handlerMap;
    };


}

#endif //SPARROW_MESSAGECENTER_HPP
