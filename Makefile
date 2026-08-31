CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = piano

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.wav

.PHONY: all run clean
