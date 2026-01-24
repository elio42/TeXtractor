CXX = g++
CXXFLAGS = -g -Isrc/include -Wall $(shell pkg-config --cflags tesseract lept)
LDFLAGS = $(shell pkg-config --libs tesseract lept)
SOURCES = $(shell find src/ -name '*.cpp')
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = TeXtractor

# Compile each .cpp file into a .o file
# -c means "compile only, don't link"
# $< is the source file (e.g., src/core/main.cpp)
# $@ is the target object file (e.g., src/core/main.o)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target to remove the executable
clean:
	rm -f $(TARGET) $(OBJECTS)
