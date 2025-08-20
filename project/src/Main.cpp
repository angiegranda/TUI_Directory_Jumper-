#include "Controller.h"
#include <iostream>


int main() {
    fs::path path = "/Users/angiegranda";
    Controller controller(path);
    controller.run();
    return 0;
}
