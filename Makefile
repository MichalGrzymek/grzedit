TARGET = grzed
TESTTARGET = test
LDLIBS = -lncurses
CXX = g++
CXXFLAGS = -g -std=c++17 -Werror -Wall -Wextra -Wshadow -Wno-sign-compare -pedantic -fsanitize=address -fsanitize=undefined
RM = rm -f
FORMAT = clang-format -i 

.PHONY: clean distclean format test

TARGETS = $(TARGET) $(TESTTARGET)

HEADERS = $(wildcard src/*.hpp)
OBJECTS = $(patsubst %.cpp, %.o, $(wildcard src/*.cpp))

MAINS = src/main.o src/main.cpp

TESTHEADERS = $(wildcard tests/*.hpp) $(HEADERS)
ALLOBJECTS =  $(OBJECTS) $(patsubst %.cpp, %.o, $(wildcard tests/*.cpp))
TESTOBJECTS = $(filter-out $(MAINS),$(ALLOBJECTS))

SOURCEFILES = $(wildcard src/*.cpp tests/*.cpp) $(HEADERS) $(TESTHEADERS)


all: $(TARGET)

%.o: %.cpp $(HEADERS)
	$(CXX) -I src -c $< -o $@ $(CXXFLAGS)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDLIBS) -o $@ $(CXXFLAGS)

$(TESTTARGET): $(TESTOBJECTS)
	$(CXX) -I src $(TESTOBJECTS) $(LDLIBS) -lgtest_main -lgtest -o $@ $(CXXFLAGS)

format:
	$(FORMAT) $(SOURCEFILES)

clean:
	$(RM) $(wildcard src/*.o) $(wildcard tests/*.o)

distclean: clean
	$(RM) $(TARGETS)
