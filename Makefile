TARGET = grzed
LDLIBS = -lncurses
CXX = g++
CXXFLAGS = -g -std=c++17 -Werror -Wall -Wextra -Wshadow -Wno-sign-compare -pedantic -fsanitize=address -fsanitize=undefined
RM = rm -f
FORMAT = clang-format -i 

.PHONY: clean distclean format

HEADERS = $(wildcard *.hpp)
OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))


all: $(TARGET)

%.o: %.cpp $(HEADERS)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDLIBS) -o $@ $(CXXFLAGS)

format:
	$(FORMAT) *.cpp *.hpp

clean:
	$(RM) *.o

distclean: clean
	$(RM) $(TARGET)
