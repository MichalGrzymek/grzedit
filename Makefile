TARGET = grzed
LDLIBS = -lncurses
CXX = g++
CXXFLAGS = -g -std=c++17 -Werror -Wall -Wextra -Wshadow -Wno-sign-compare -pedantic -fsanitize=address -fsanitize=undefined
RM = rm -f
FORMAT = clang-format -i 

.PHONY: clean distclean format

HEADERS = $(wildcard src/*.hpp)
OBJECTS = $(patsubst %.cpp, %.o, $(wildcard src/*.cpp))

all: $(TARGET)

%.o: %.cpp $(HEADERS)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDLIBS) -o $@ $(CXXFLAGS)

format:
	$(FORMAT) $(HEADERS) $(wildcard src/*.cpp)

clean:
	$(RM) $(wildcard src/*.o)

distclean: clean
	$(RM) $(TARGET)
