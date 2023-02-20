//
// Created by ZhangHao on 2023/2/19.
//

#ifndef SPARROW_RENDERCOMMAND_HPP
#define SPARROW_RENDERCOMMAND_HPP

#include "RenderBackEndI.h"
#include <memory>
#include <functional>

namespace SPW {
    class RenderCommand {
    public:
        RenderCommand() = delete;

        // APIName from RenderBackEndI
        template<typename ...Args>
        explicit RenderCommand(void(RenderBackEndI::*MenFn)(const Args& ...), const Args& ...args) {
            apiCaller = [=](const RenderBackEndI &api){
                (api.*MenFn)(std::forward<Args>(args)...);
            };
        }


        template<typename ...Args>
        explicit RenderCommand(void(RenderBackEndI::*MenFn)(Args ...), Args ...args) {
            apiCaller = [=](RenderBackEndI &api){
                (api.*MenFn)(args ...);
            };
        }

        // move
        template<typename ...Args>
        explicit RenderCommand(const RenderCommand &&other) {
            apiCaller = other.apiCaller;
        }

        // copy
        template<typename ...Args>
        explicit RenderCommand(const RenderCommand &other) {
            apiCaller = other.apiCaller;
        }

        // execute with an api
        void execute(RenderBackEndI &api) {
            apiCaller(api);
        }

    private:
        std::function<void(RenderBackEndI &api)> apiCaller;
    };
}

#endif //SPARROW_RENDERCOMMAND_HPP
