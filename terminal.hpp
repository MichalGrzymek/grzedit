#ifndef TERMINAL_HPP
#define TERMINAL_HPP
/*
controlling the terminal using calls to ncurses
*/
#include "constants.hpp"

#include <algorithm>
#include <ncurses.h>
#include <string>

// ncurses character code for CTRL + key x
constexpr int ctrl_plus(int x) { return x & 0x1f; }

class Terminal {
private:
  constexpr static int info_rows = 2;
  constexpr static int status_rows = 1;
  WINDOW *text_window;
  WINDOW *info_window;
  WINDOW *status_window;
  void reset_cursor();
  void write(int y, int x, char c, Style s);

public:
  struct Field {
    char character;
    Style style;
  };
  Terminal();
  ~Terminal();

  void move_cursor(int y, int x);
  void move_cursor_x(int x);
  void move_cursor_y(int y);
  void display(const std::function<Field(int, int)> &text);
  int get_char();
  std::pair<int, int> cursor_yx();
  int cursor_y();
  int cursor_x();
  std::pair<int, int> max_yx();
  int max_y();
  int max_x();
  void display_status(std::string s);
  void deactivate_status();
};

#endif
