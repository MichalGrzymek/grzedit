#include "cursor.hpp"
#include "terminal.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <string>

std::vector<std::string> lines;
std::string filename;

void handle_cmd_args(int argc, char *argv[]) {
  if (argc == 2) {
    filename = argv[1];

    std::ifstream file(filename);
    if (file.is_open()) {
      std::string line;

      while (std::getline(file, line))
        lines.push_back(line);
    } else
      lines = {""};
  } else {
    std::cerr << "use: ./grzed filename" << std::endl;
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  handle_cmd_args(argc, argv);

  Terminal terminal;
  Cursor cursor(0, 0, terminal, lines);
  while (true) {
    {
      // offset in file at which the left upper corner of the terminal is
      int y_offset = cursor.get_y() - terminal.cursor_y(),
          x_offset = cursor.get_x() - terminal.cursor_x();
      if (y_offset < 0 || x_offset < 0)
        throw std::logic_error(
            "negative offset y_offset=" + std::to_string(y_offset) +
            " x_offset=" + std::to_string(x_offset));
      std::vector<std::string> temp;
      for (int i = y_offset; i < lines.size(); ++i)
        temp.push_back(std::string(
            min(lines[i].end(), lines[i].begin() + x_offset), lines[i].end()));
      terminal.display(temp);
    }
    terminal.deactivate_status(); // only display for one iteration

    int c = terminal.get_char();
    switch (c) {
    case KEY_BACKSPACE:
      if (cursor.get_x() > 0) {
        lines[cursor.get_y()].erase(cursor.get_x() - 1, 1);
        cursor.move_left();
      } else if (cursor.get_y() != 0) {
        cursor.move_up();
        cursor.move_maxright();
        lines[cursor.get_y()] += lines[cursor.get_y() + 1];
        lines.erase(lines.begin() + cursor.get_y() + 1);
      }
      break;
    case KEY_UP:
      cursor.move_up();
      break;
    case KEY_DOWN:
      cursor.move_down();
      break;
    case KEY_LEFT:
      cursor.move_left();
      break;
    case KEY_RIGHT:
      cursor.move_right();
      break;
    case '\n': {
      std::string new_line(lines[cursor.get_y()].begin() + cursor.get_x(),
                           lines[cursor.get_y()].end());
      lines[cursor.get_y()].erase(lines[cursor.get_y()].begin() + cursor.get_x(),
                            lines[cursor.get_y()].end());
      lines.insert(lines.begin() + cursor.get_y() + 1, new_line);
      cursor.move_maxleft();
      cursor.move_down();
    } break;
    case ctrl_plus('o'): {
      std::ofstream file(filename);
      if (!file.is_open())
        throw std::runtime_error("couldn't open file for writing!");
      for (std::string line : lines)
        file << line << '\n';
      terminal.display_status("Saved!");
    } break;
    case ctrl_plus('x'):
      return 0;
      break;
    case ctrl_plus('a'):
      cursor.move_maxleft();
      break;
    case ctrl_plus('e'):
      cursor.move_maxright();
      break;
    case ctrl_plus('p'):
      cursor.move_up();
      break;
    case ctrl_plus('n'):
      cursor.move_down();
      break;
    default:
      lines[cursor.get_y()].insert(lines[cursor.get_y()].begin() + cursor.get_x(), (char)c);
      cursor.move_right();
    }
  }
}
