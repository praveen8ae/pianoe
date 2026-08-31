CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = piano
PYTHON = python3

.PHONY: all run clean gui-run gui-server help

help:
	@echo "🎹 Piano Makefile Targets:"
	@echo ""
	@echo "  make gui-run      - Start the GUI piano (requires browser)"
	@echo "  make gui-server   - Start GUI server on localhost:8000"
	@echo "  make run          - Run console piano demo"
	@echo "  make all          - Build console piano binary"
	@echo "  make clean        - Remove built files"
	@echo ""

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp

run: $(TARGET)
	./$(TARGET)

gui-run:
	@echo "🎹 Opening Interactive Piano..."
	@$(PYTHON) run_piano.py

gui-server:
	@echo "🎹 Starting Piano GUI Server at http://localhost:8000"
	@cd $(dir $(abspath $(firstword $(MAKEFILE_LIST)))) && $(PYTHON) -m http.server 8000

clean:
	rm -f $(TARGET) *.wav

.PHONY: all run clean gui-run gui-server help
