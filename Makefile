#######################################
#   Makefile for Pure Data on Linux   #
#######################################

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
UNAME := $(shell uname -m)

ifeq ($(UNAME),x86_64)
	CXXFLAGS = -Wall -Wextra -std=c++17 -fPIC -Iinclude -MMD -MP -DUSE_DOUBLE_PRECISION
endif

ifeq ($(UNAME),armv7l)
    CXXFLAGS = -Wall -Wextra -std=c++17 -fPIC -Iinclude -MMD -MP -mfpu=neon -mfloat-abi=hard -march=armv7-a
endif

ifeq ($(UNAME),aarch64)
    CXXFLAGS = -Wall -Wextra -std=c++17 -fPIC -Iinclude -MMD -MP -DUSE_DOUBLE_PRECISION
endif

# === Directory layout ===
SRC_DIR = src
PD_SRC_DIR = $(SRC_DIR)/puredata
OBJ_DIR = obj
BIN_DIR = out

# === Pure Data sources ===
PD_SOURCES = \
	$(SRC_DIR)/DSP.cpp \
	$(SRC_DIR)/SlewLimiter.cpp \
	$(SRC_DIR)/ParamFader.cpp \
	$(SRC_DIR)/DSPBuffer.cpp \
	$(SRC_DIR)/DSPObject.cpp \
	$(SRC_DIR)/DCBlocker.cpp \
	$(SRC_DIR)/Voice.cpp \
	$(SRC_DIR)/Oscillator.cpp \
	$(SRC_DIR)/SineOscillator.cpp \
	$(SRC_DIR)/SawOscillator.cpp \
	$(SRC_DIR)/TriangleOscillator.cpp \
	$(SRC_DIR)/SquareOscillator.cpp \
	$(SRC_DIR)/NoiseGenerator.cpp \
	$(SRC_DIR)/WaveformGenerator.cpp \
	$(SRC_DIR)/WavetableOscillator.cpp \
	$(SRC_DIR)/SineWavetable.cpp \
	$(SRC_DIR)/SawWavetable.cpp \
	$(SRC_DIR)/SquareWavetable.cpp \
	$(SRC_DIR)/TriangleWavetable.cpp \
	$(SRC_DIR)/HarmonicClusterWavetable.cpp \
	$(SRC_DIR)/FibonacciWavetable.cpp \
	$(SRC_DIR)/MirrorWavetable.cpp \
	$(SRC_DIR)/ModuloWavetable.cpp \
	$(SRC_DIR)/BitWavetable.cpp \
	$(SRC_DIR)/KorgonFilter.cpp \
	$(PD_SRC_DIR)/jpvoice~.cpp 

PD_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(PD_SOURCES))
PD_TARGET = $(BIN_DIR)/jpvoice~.pd_linux

# === Dependency files ===
DEPS = $(PD_OBJECTS:.o=.d)

# === Default target ===
all: $(PD_TARGET)

# === Debug build ===
debug: CXXFLAGS += -O0 -g
debug: clean all

# === Release build ===
release: CXXFLAGS += -O3
release: clean all

# === Build rule for pd_linux external ===
$(PD_TARGET): $(PD_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) -shared -o $@ $^

# === Compile rule for .cpp to .o ===
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === Clean rule ===
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# === Include dependency files ===
-include $(DEPS)

.PHONY: all clean debug release
