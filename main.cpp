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
    if (!file.is_open())
      throw std::runtime_error("couldn't open file for reading!");
    std::string line;

    while (std::getline(file, line))
      lines.push_back(line);
  } else {
    std::cerr << "you must pass a filename to edit" << std::endl;
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  handle_cmd_args(argc, argv);

  Terminal terminal;

  // position of cursor in file
  int file_y = 0;
  int file_x = 0;
  while (1) {
    {
      // offset in file at which the left upper corner of the terminal is
      int y_offset = file_y - terminal.cursor_y(),
          x_offset = file_x - terminal.cursor_x();
      if (y_offset < 0 || x_offset < 0)
        throw std::runtime_error(
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

    auto move_file_x = [&](int x) {
      if (lines[file_y].size() < x)
        throw std::runtime_error("x too big");
      if (x < 0)
        throw std::runtime_error("x too small");
      int delta_x = x - file_x;
      terminal.move_cursor_x(
          std::clamp(terminal.cursor_x() + delta_x, 0, terminal.max_x() - 1));
      file_x = x;
    };

    auto move_file_y = [&](int y) {
      if (y >= lines.size())
        throw std::runtime_error("y too big");
      if (y < 0)
        throw std::runtime_error("y too small");
      int delta_y = y - file_y;
      terminal.move_cursor_y(
          std::clamp(terminal.cursor_y() + delta_y, 0, terminal.max_y() - 1));
      file_y = y;

      if (lines[file_y].size() < file_x)
        throw std::runtime_error("x too big for new line");
    };

    auto cursor_left = [&]() {
      if (file_x > 0) {
        move_file_x(file_x - 1);
      }
    };
    auto cursor_right = [&]() {
      if (file_x < lines[file_y].size()) { // no -1, get after last char
        move_file_x(file_x + 1);
      }
    };
    auto cursor_up = [&]() {
      if (file_y > 0) {
        move_file_x(std::min(file_x, (int)lines[file_y - 1].size()));
        move_file_y(file_y - 1);
      }
    };
    auto cursor_down = [&]() {
      if (file_y < lines.size() - 1) {
        move_file_x(std::min(file_x, (int)lines[file_y + 1].size()));
        move_file_y(file_y + 1);
      }
    };
    auto cursor_maxright = [&]() { move_file_x(lines[file_y].size()); };
    auto cursor_maxleft = [&]() { move_file_x(0); };

    switch (c) {
    case KEY_BACKSPACE:
      if (file_x > 0) {
        lines[file_y].erase(file_x - 1, 1);
        cursor_left();
      } else if (file_y != 0) {
        cursor_up(); // changes file_y!
        cursor_maxright();
        lines[file_y] += lines[file_y + 1];
        lines.erase(lines.begin() + file_y + 1);
      }
      break;
    case KEY_UP:
      cursor_up();
      break;
    case KEY_DOWN:
      cursor_down();
      break;
    case KEY_LEFT:
      cursor_left();
      break;
    case KEY_RIGHT:
      cursor_right();
      break;
    case '\n': {
      std::string new_line(lines[file_y].begin() + file_x, lines[file_y].end());
      lines[file_y].erase(lines[file_y].begin() + file_x, lines[file_y].end());
      lines.insert(lines.begin() + file_y + 1, new_line);
      cursor_maxleft();
      cursor_down();
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
      cursor_maxleft();
      break;
    case ctrl_plus('e'):
      cursor_maxright();
      break;
    case ctrl_plus('p'):
      cursor_up();
      break;
    case ctrl_plus('n'):
      cursor_down();
      break;
    default:
      lines[file_y].insert(lines[file_y].begin() + file_x, (char)c);
      cursor_right();
    }
  }
}
