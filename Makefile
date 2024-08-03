CXX = g++
CXXFLAGS = -std=c++17 `wx-config --cxxflags`
LDFLAGS = `wx-config --libs` -lcurl

SOURCES = main.cpp commands.cpp simpleshell.cpp tokens.cpp
HEADERS = commands.hpp simpleshell.hpp tokens.hpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = SimpleShellGUI

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)