# Makefile for Quantum Simulator
# This is a wrapper around CMake to simplify common build tasks

# Configuration variables - modify as needed
BUILD_DIR := build
CMAKE := cmake
CMAKE_GENERATOR :=
VCPKG_TOOLCHAIN := $(CURDIR)/vcpkg/scripts/buildsystems/vcpkg.cmake
BUILD_TYPE := Release

# Detect macOS
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  # Check for Apple Silicon
  UNAME_M := $(shell uname -m)
  ifeq ($(UNAME_M),arm64)
    VCPKG_TRIPLET := arm64-osx
  else
    VCPKG_TRIPLET := x64-osx
  endif
  # macOS requires vcpkg and libomp
  CMAKE_ARGS := -DCMAKE_TOOLCHAIN_FILE=$(VCPKG_TOOLCHAIN) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
else ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
  # Windows with MinGW
  VCPKG_TRIPLET := x64-windows
  CMAKE_ARGS := -DCMAKE_TOOLCHAIN_FILE=$(VCPKG_TOOLCHAIN) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
else ifeq ($(findstring MSYS,$(UNAME_S)),MSYS)
  # Windows with MSYS
  VCPKG_TRIPLET := x64-windows
  CMAKE_ARGS := -DCMAKE_TOOLCHAIN_FILE=$(VCPKG_TOOLCHAIN) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
else
  # Linux or other Unix-like OS
  CMAKE_ARGS := -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
endif

# Default target (will be executed when just typing 'make')
.PHONY: all
all: build

# Help target
.PHONY: help
help:
	@echo "Quantum Simulator Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  make              - Build the project (default)"
	@echo "  make run          - Build and run the simulator"
	@echo "  make test         - Build and run all tests"
	@echo "  make clean        - Remove build directory"
	@echo "  make rebuild      - Clean and rebuild from scratch"
	@echo "  make deps         - Install dependencies (macOS only)"
	@echo "  make help         - Show this help message"
	@echo ""
	@echo "Configuration options:"
	@echo "  BUILD_TYPE = $(BUILD_TYPE) (can be Debug or Release)"
	@echo "  VCPKG_TRIPLET = $(VCPKG_TRIPLET) (auto-detected)"
	@echo ""
	@echo "Examples:"
	@echo "  make BUILD_TYPE=Debug"
	@echo "  make run BUILD_TYPE=Release"

# Configure the build
.PHONY: configure
configure:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) $(CMAKE_ARGS) ..

# Build the project
.PHONY: build
build: configure
	@echo "Building Quantum Simulator ($(BUILD_TYPE) mode)..."
	@cd $(BUILD_DIR) && $(CMAKE) --build .
	@echo "Build complete. Executable is at $(BUILD_DIR)/src/quantum_simulator"

# Run the simulator
.PHONY: run
run: build
	@echo "Running Quantum Simulator..."
	@cd $(BUILD_DIR) && src/quantum_simulator

# Run all tests
.PHONY: test
test: build
	@echo "Running tests..."
	@cd $(BUILD_DIR) && ctest --output-on-failure

# Clean build artifacts
.PHONY: clean
clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete"

# Clean and rebuild
.PHONY: rebuild
rebuild: clean build

# Install dependencies (macOS only)
.PHONY: deps
deps:
ifeq ($(UNAME_S),Darwin)
	@echo "Installing dependencies for macOS..."
	@echo "Setting up vcpkg..."
	@if [ ! -f vcpkg/vcpkg ]; then ./vcpkg/bootstrap-vcpkg.sh; fi
	@echo "Installing packages for $(VCPKG_TRIPLET)..."
	@./vcpkg/vcpkg install fftw3:$(VCPKG_TRIPLET) \
		glfw3:$(VCPKG_TRIPLET) \
		glad:$(VCPKG_TRIPLET) \
		nlohmann-json:$(VCPKG_TRIPLET) \
		hdf5:$(VCPKG_TRIPLET) \
		"imgui[glfw-binding,opengl3-binding]:$(VCPKG_TRIPLET)" \
		--recurse
	@echo "Installing OpenMP via Homebrew..."
	@brew install libomp
	@echo "All dependencies installed successfully"
else
	@echo "Dependency installation via make is only supported on macOS."
	@echo "Please refer to the README.md for installation instructions for your platform."
endif