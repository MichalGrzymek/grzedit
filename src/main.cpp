#include "editor.hpp"

std::string handle_cmd_args(int argc, char *argv[]) {
  if (argc == 2) {
    return argv[1];
  } else {
    std::cerr << "use: ./grzed filename" << std::endl;
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  std::string filename = handle_cmd_args(argc, argv);
  Terminal terminal;
  Editor editor(filename, terminal);
  return editor.main_loop();
}
