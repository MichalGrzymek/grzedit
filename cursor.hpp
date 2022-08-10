#ifndef CURSOR_HPP
#define CURSOR_HPP
#include "terminal.hpp"

#include <optional>
#include <utility>
/*
controls the file cursor and allows
moving it together with the terminal cursor
*/

class Cursor {
private:
  Terminal &terminal;
  std::vector<std::string> &lines;
  int y, x;
  std::optional<int> selection_anchor;
  void set_x(int new_x);
  void set_y(int new_y);

public:
  Cursor(int _y, int _x, Terminal &_terminal, std::vector<std::string> &_lines);
  int get_x();
  int get_y();

  void move_left();
  void move_right();
  void move_up();
  void move_down();

  void move_maxright();
  void move_maxleft();

  void move_to_row(int row);

  void toggle_selection();
  void disable_selection();
  bool selection_active();
  bool in_selection(int row);
  std::pair<int, int> get_selection();
};

#endif
