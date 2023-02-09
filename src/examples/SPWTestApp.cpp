#include <iostream>
#include "ApplicationFramework/Application/Application.h"

int main() {
    Application app("hello");
    std::printf("Hello World, %s", app._name);
    return 0;
}