.PHONY: all build run clean help install

# Default build type
BUILD_TYPE ?= Release

help:
	@echo "🎹 Piano Application - Makefile Targets"
	@echo ""
	@echo "  make build        - Build the piano application"
	@echo "  make run          - Build and run"
	@echo "  make install      - Install to system (requires sudo for /usr/bin)"
	@echo "  make clean        - Remove build directory"
	@echo "  make help         - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make                  # Build with Release optimization"
	@echo "  make BUILD_TYPE=Debug # Build with debug symbols"
	@echo "  make run              # Build and immediately run"
	@echo ""

all: build

build:
	@mkdir -p build
	@cd build && cmake -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) .. && make
	@echo "✓ Build complete: ./build/piano"

run: build
	@./build/piano

install: build
	@cd build && make install
	@echo "✓ Installed to system"

clean:
	@rm -rf build
	@echo "✓ Cleaned"

.PHONY: all build run clean help install
