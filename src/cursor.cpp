#include "cursor.hpp"

#include <stdexcept>

Cursor::Cursor(int _y, int _x, Terminal &_terminal, Buffer &_buffer)
    : terminal(_terminal), buffer(_buffer), y(_y), x(_x),
      selection_anchor(std::nullopt) {}

void Cursor::set_x(int new_x) {
  if (buffer[y].size() < new_x)
    throw std::logic_error("cursor x too big");
  if (new_x < 0)
    throw std::logic_error("cursor x too small");
  int delta_x = new_x - x;
  terminal.move_cursor_x(
      std::clamp(terminal.cursor_x() + delta_x, 0, terminal.max_x() - 1));
  x = new_x;
};

void Cursor::set_y(int new_y) {
  if (new_y >= buffer.size())
    throw std::logic_error("cursor y too big");
  if (new_y < 0)
    throw std::logic_error("cursor y too small");
  int delta_y = new_y - y;
  terminal.move_cursor_y(
      std::clamp(terminal.cursor_y() + delta_y, 0, terminal.max_y() - 1));
  y = new_y;

  if (buffer[y].size() < x)
    throw std::logic_error("cursor x too big for new line");
};

int Cursor::get_y() { return y; }

int Cursor::get_x() { return x; }

void Cursor::move_left() {
  if (x > 0) {
    set_x(x - 1);
  }
};
void Cursor::move_right() {
  if (x < buffer[y].size()) { // no -1, get after last char
    set_x(x + 1);
  }
};
void Cursor::move_up() {
  if (y > 0) {
    set_x(std::min(x, (int)buffer[y - 1].size()));
    set_y(y - 1);
  }
};
void Cursor::move_down() {
  if (y + 1 < buffer.size())
    move_to_row(y + 1);
};

void Cursor::move_maxright() { set_x(buffer[y].size()); };
void Cursor::move_maxleft() { set_x(0); };

void Cursor::move_to_row(int row) {
  if (row < 0 || row >= buffer.size())
    throw std::runtime_error("incorrect row number!");
  set_x(std::min(x, (int)buffer[row].size()));
  set_y(row);
}

void Cursor::toggle_selection() {
  if (selection_anchor.has_value())
    selection_anchor.reset();
  else
    selection_anchor = y;
}

void Cursor::disable_selection() { selection_anchor.reset(); }

bool Cursor::in_selection(int row) {
  if (!selection_anchor.has_value())
    return false;
  auto [begin, end] = get_selection();
  return begin <= row && row <= end;
}

bool Cursor::selection_active() { return selection_anchor.has_value(); }

std::pair<int, int> Cursor::get_selection() {
  if (!selection_active())
    throw std::logic_error("selection not active!");
  if (selection_anchor.value() <= y)
    return {selection_anchor.value(), y};
  else
    return {y, selection_anchor.value()};
}
