# === Compiler settings ===
# Use g++ as the C++ compiler
CXX = g++

# Compiler flags:
# -Wall: enable all warnings
# -Wextra: enable extra warnings
# -std=c++17: use the C++17 standard
# -fPIC: generate position-independent code (useful for shared libraries)
# -Iinclude: add 'include' directory to the header search path
# -MMD -MP: generate dependency files for header tracking
CXXFLAGS_COMMON = -Wall -Wextra -std=c++17 -fPIC -Iinclude -MMD -MP

# === Directory layout ===
# Source files are located in ./src
# Header files are in ./include
# Object files will be placed in ./obj
# The final binary will be placed in ./bin
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# === Target binary ===
TARGET = $(BIN_DIR)/jpvoice

# === Explicit list of source files ===
SOURCES = \
    $(SRC_DIR)/main.cpp \
    $(SRC_DIR)/Voice.cpp \
    $(SRC_DIR)/Oscillator.cpp \
    $(SRC_DIR)/Supersaw.cpp

# === Derived object and dependency files ===
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))
DEPS    = $(OBJECTS:.o=.d)

# === Default target ===
all: release

# === Release build ===
release: CXXFLAGS = $(CXXFLAGS_COMMON) -O3
release: build

# === Debug build ===
debug: CXXFLAGS = $(CXXFLAGS_COMMON) -O0 -g
debug: build

# === Shared build rule ===
build: $(TARGET)

# Link all object files into the final binary
$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $(TARGET)

# Compile each source file to an object file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === Clean rule ===
# Removes all build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# === Include auto-generated dependency files ===
# Ensures that changes to headers trigger recompilation
-include $(DEPS)

# === Mark targets as phony (not actual files) ===
.PHONY: all release debug build clean
