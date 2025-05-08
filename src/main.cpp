#include "../include/tui_configurator.h"
#include <iostream>

int main(int argc, char* argv[]) {
    tui::Configurator::run();
    std::cout << "If you are currently using cjsh, please restart it to apply the changes." << std::endl;
    return 0;
}
