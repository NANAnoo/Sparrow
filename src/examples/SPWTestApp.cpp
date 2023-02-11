#include <iostream>
#include "SparrowCore.h"

int main() {
    SPW::Application app("hello");
    std::printf("Hello World, %s", app._name);
    return 0;
}