#include "cursor.hpp"
#include "buffer.hpp"

#include <gtest/gtest.h> 

TEST(CursorTest, HandlesEmptyFile){
	Buffer buffer{{}};
	Terminal terminal;
	Cursor cursor(0, 0, terminal, buffer);

	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 0);
	cursor.move_left();
	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 0);
	cursor.move_up();
	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 0);
	cursor.move_right();
	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 0);
	cursor.move_down();
	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 0);
	cursor.move_maxleft();
	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 0);
	cursor.move_maxright();
	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 0);
	cursor.move_to_row(0);
	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 0);
}

TEST(CursorTest, MoveRightToEnd){
	Buffer buffer{{'a'}};
	Terminal terminal;
	Cursor cursor(0, 0, terminal, buffer);
	cursor.move_right();
	EXPECT_EQ(cursor.get_x(), 1);
	EXPECT_EQ(cursor.get_y(), 0);
}

TEST(CursorTest, MoveMaxrightOnOneCharacterFile){
	Buffer buffer{{'a'}};
	Terminal terminal;
	Cursor cursor(0, 0, terminal, buffer);
	cursor.move_maxright();
	EXPECT_EQ(cursor.get_x(), 1);
	EXPECT_EQ(cursor.get_y(), 0);
}

TEST(CursorTest, MoveDownToShorterLine){
	Buffer buffer{{'a'}, {}};
	Terminal terminal;
	Cursor cursor(0, 0, terminal, buffer);
	cursor.move_maxright();
	cursor.move_down();
	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 1);
}
