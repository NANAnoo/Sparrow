//
// Created by ZhangHao on 2023/2/19.
//

#ifndef SPARROW_RENDERCOMMAND_HPP
#define SPARROW_RENDERCOMMAND_HPP

#include "RenderAPII.h"
#include <memory>
#include <functional>

namespace SPW {
    class RenderCommand {
    public:
        RenderCommand() = delete;

        // APIName from RenderAPII
        template<typename ...Args>
        explicit RenderCommand(void(RenderAPII::*MenFn)(const Args& ...), const Args& ...args) {
            apiCaller = [=](const RenderAPII &api){
                (api.*MenFn)(std::forward<Args>(args)...);
            };
        }


        template<typename ...Args>
        explicit RenderCommand(void(RenderAPII::*MenFn)(Args ...), Args ...args) {
            apiCaller = [=](RenderAPII &api){
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
        void execute(RenderAPII &api) {
            apiCaller(api);
        }

    private:
        std::function<void(RenderAPII &api)> apiCaller;
    };
}

#endif //SPARROW_RENDERCOMMAND_HPP
