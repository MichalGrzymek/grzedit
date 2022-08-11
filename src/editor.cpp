#include "editor.hpp"

Buffer Editor::read_file() {
  Buffer buf;
  std::ifstream file(filename);
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line))
      buf.push_back(Line(line.begin(), line.end()));
  } else
    buf = {{}};
  return buf;
}

void Editor::write_file() {
  std::ofstream file(filename);
  if (!file.is_open())
    throw std::runtime_error("couldn't open file for writing!");
  for (Line line : buffer) {
    for (Character character : line)
      file << character;
    file << '\n';
  }
}

Editor::Editor(std::string _filename, Terminal &_terminal)
    : filename(_filename), buffer(read_file()), terminal(_terminal),
      cursor(0, 0, _terminal, this->buffer) {
  // buffer=read_file(filename);
}

std::pair<int,int> Editor::get_yx_offset(){
    const int y_offset = cursor.get_y() - terminal.cursor_y(),
              x_offset = cursor.get_x() - terminal.cursor_x();
    if (y_offset < 0 || x_offset < 0)
      throw std::logic_error(
          "negative offset y_offset=" + std::to_string(y_offset) +
          " x_offset=" + std::to_string(x_offset));
    return {y_offset, x_offset};
}

void Editor::refresh_screen(){
    {
      // offset in file at which the left upper corner of the terminal is
      auto[y_offset, x_offset]=get_yx_offset();
      terminal.display([&](int y, int x) -> Terminal::Field {
        char c = ' ';
        // change from terminal coordinates to file coordinates
        y += y_offset;
        x += x_offset;
        Style s = cursor.in_selection(y) ? Style::selection : Style::normal;
        if (y < buffer.size() && x < buffer[y].size()) {
          c = buffer[y][x];
          int max_file_x_in_window = x_offset + terminal.max_x() - 1;
          if (x == max_file_x_in_window && x + 1 < buffer[y].size()) {
            s = Style::inline_info;
            c = '>';
          }
        }
        return {c, s};
      });
    }
}

int Editor::main_loop() {
  while (true) {
    refresh_screen();
    terminal.deactivate_status(); // only display for one iteration

    int c = terminal.get_char();
    switch (c) {
    case KEY_BACKSPACE: {
      Line &current_line = buffer[cursor.get_y()];
      if (cursor.get_x() > 0) {
        current_line.erase(current_line.begin() + cursor.get_x() - 1);
        cursor.move_left();
      } else if (cursor.get_y() != 0) {
        cursor.move_up();
        cursor.move_maxright();
        for (Character character : buffer[cursor.get_y() + 1])
          buffer[cursor.get_y()].push_back(character);
        buffer.erase(buffer.begin() + cursor.get_y() + 1);
      }
    } break;
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
      Line new_line(buffer[cursor.get_y()].begin() + cursor.get_x(),
                    buffer[cursor.get_y()].end());
      buffer[cursor.get_y()].erase(buffer[cursor.get_y()].begin() +
                                       cursor.get_x(),
                                   buffer[cursor.get_y()].end());
      buffer.insert(buffer.begin() + cursor.get_y() + 1, new_line);
      cursor.move_maxleft();
      cursor.move_down();
    } break;
    case ctrl_plus('o'): { // save
      write_file();
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
    case ctrl_plus('v'):
      cursor.toggle_selection();
      break;
    case ctrl_plus('k'): { // cut
      int begin_delete, end_delete;

      if (cursor.selection_active())
        std::tie(begin_delete, end_delete) = cursor.get_selection();
      else
        begin_delete = end_delete = cursor.get_y();

      copy_buffer.clear();
      copy_buffer.insert(copy_buffer.begin(), buffer.begin() + begin_delete,
                         buffer.begin() + end_delete + 1);

      buffer.erase(buffer.begin() + begin_delete,
                   buffer.begin() + end_delete +
                       1); // might put cursor in a bad state

      if (buffer.empty())
        buffer = {{}};

      cursor.move_to_row(std::min(static_cast<int>(buffer.size()) - 1,
                                  begin_delete)); // fix cursor
      cursor.disable_selection();
    } break;
    case ctrl_plus('u'): // paste
      buffer.insert(buffer.begin() + cursor.get_y() + 1, copy_buffer.begin(),
                    copy_buffer.end());
      cursor.move_down();
      break;
    default:
      buffer[cursor.get_y()].insert(
          buffer[cursor.get_y()].begin() + cursor.get_x(), c);
      cursor.move_right();
    }
  }
}
