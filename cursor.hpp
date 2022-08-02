#ifndef CURSOR_HPP
#define CURSOR_HPP
#include "terminal.hpp"
/*
controls the position of the file cursor and allows
moving it together with the terminal cursor
*/

class Cursor{
private:
  Terminal &terminal;
  std::vector<std::string> &lines;

public:
  int y,x;
  Cursor(int _y, int _x, Terminal &_terminal, std::vector<std::string> &_lines);
  void set_x(int new_x);
  void set_y(int new_y);

  void move_left();
  void move_right();
  void move_up();
  void move_down();
  
  void move_maxright();
  void move_maxleft();
};


#endif
