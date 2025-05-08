#include <iostream>

#include "../include/cjsh_filesystem.h"
#include "../include/tui_configurator.h"

int main(int argc, char* argv[]) {
  initialize_cjsh_directories();
  tui::Configurator::run();
  std::cout << "If you are currently using cjsh, please restart it to apply "
               "the changes."
            << std::endl;
  return 0;
}
