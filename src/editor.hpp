#ifndef EDITOR_HPP
#define EDITOR_HPP
#include "buffer.hpp"
#include "cursor.hpp"
#include "terminal.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <string>

class Editor {
private:
  std::string filename;
  Buffer buffer;

  Buffer copy_buffer;
  Terminal &terminal; // reference to allow virtual terminal
  Cursor cursor;

  Buffer read_file();
  void write_file();

  std::pair<int,int> get_yx_offset();
  void refresh_screen();
public:
  Editor(std::string _filename, Terminal &_terminal);

  int main_loop();
};

#endif
