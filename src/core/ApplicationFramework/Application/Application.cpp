#include "Application.h"
#include <iostream>
#include <cstring>

Application::Application(const char *info) {
    _name = info;
    std::cout << info << std::endl;
}