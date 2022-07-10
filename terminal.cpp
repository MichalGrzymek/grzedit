#include "terminal.hpp"

#include <ncurses.h>
#include <signal.h>
#include <stdexcept>

/* ncurses doesn't handle sigsegv,
because of noecho this messes up the terminal*/
void sigsegv_handler(int signo) {
  (void)signo;
  endwin();
  signal(SIGSEGV, nullptr); // avoids infinite loop
}

void Terminal::move_cursor(int y, int x) { wmove(text_window, y, x); }

void Terminal::move_cursor_x(int x) { move_cursor(cursor_y(), x); }

void Terminal::move_cursor_y(int y) { move_cursor(y, cursor_x()); }

void Terminal::write(int y, int x, char c) { mvwaddch(text_window, y, x, c); }

std::pair<int, int> Terminal::cursor_yx() {
  int y, x;
  getyx(text_window, y, x);
  return {y, x};
}

int Terminal::cursor_y() {
  auto [y, x] = cursor_yx();
  return y;
}

int Terminal::cursor_x() {
  auto [y, x] = cursor_yx();
  return x;
}

std::pair<int, int> Terminal::max_yx() {
  int y, x;
  getmaxyx(text_window, y, x);
  return {y, x};
}

int Terminal::max_y() {
  auto [y, x] = max_yx();
  return y;
}

int Terminal::max_x() {
  auto [y, x] = max_yx();
  return x;
}

int Terminal::get_char() { return wgetch(text_window); }

void Terminal::display(const std::vector<std::string> &lines) {
  int cursor_y, cursor_x;
  getyx(text_window, cursor_y, cursor_x);

  int max_x;
  int max_y;

  getmaxyx(text_window, max_y, max_x);

  wclear(text_window);

  int height = std::min((int)lines.size(), max_y);
  for (int i = 0; i < height; ++i) {
    int width = std::min(max_x, (int)lines[i].size());
    for (int j = 0; j < width; ++j)
      mvwaddch(text_window, i, j, lines[i][j]);
    if ((int)lines[i].size() > max_x) {
      wattron(text_window, COLOR_PAIR(1));
      mvwaddch(text_window, i, max_x - 1, '>');
      wattroff(text_window, COLOR_PAIR(1));
    }
  }

  wmove(text_window, cursor_y, cursor_x);
  wrefresh(text_window);
  wrefresh(status_window);
}

Terminal::Terminal() {
  setlocale(LC_ALL, "");

  signal(SIGSEGV, sigsegv_handler);
  initscr(); // start ncurses

  // terminal options
  cbreak();
  noecho();

  // ncurses options
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_YELLOW);

  int nrows, ncolumns;
  getmaxyx(stdscr, nrows, ncolumns);

  if (nrows < info_rows + status_rows + 1)
    throw std::runtime_error("terimal is too small!");

  text_window = newwin(nrows - info_rows - status_rows, ncolumns, 0, 0);
  keypad(text_window, TRUE);

  status_window =
      newwin(status_rows, ncolumns, nrows - status_rows - info_rows, 0);
  display_status("Welcome to grzedit v1.0");

  info_window = newwin(info_rows, ncolumns, nrows - info_rows, 0);
  waddstr(info_window, "^X exit \n^O write");
  wrefresh(info_window);
}

Terminal::~Terminal() { endwin(); }

void Terminal::display_status(std::string s) {
  wclear(status_window);
  mvwaddstr(status_window, 0, max_x() / 2 - s.size() / 2, s.c_str());
  wrefresh(status_window);
  reset_cursor();
}

void Terminal::deactivate_status() {
  wclear(status_window);
  reset_cursor();
}

void Terminal::reset_cursor() { move_cursor(cursor_y(), cursor_x()); }
