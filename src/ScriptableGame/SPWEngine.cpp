//
// Created by NANAnoo on 3/20/2023.
//

#include "GameWrapper.hpp"

// main entrance
int main(int argc, char **argv) {
    // app test
    auto appProxy = SPW::Application::create<SPW::GameWrapper>();
    return appProxy->app->run(argc, argv);
}