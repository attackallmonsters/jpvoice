#######################################
#   Makefile for Pure Data on Linux   #
#######################################

CXX = g++
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

SRC_DIR = src
PD_SRC_DIR = $(SRC_DIR)/puredata
OBJ_DIR = obj
BIN_DIR = out

# === Module names ===
PD_MODULES = jpvoice~ adsr~
PD_TARGETS = $(addprefix $(BIN_DIR)/, $(addsuffix .pd_linux, $(PD_MODULES)))

# === Common C++ sources ===
COMMON_SOURCES = \
	$(SRC_DIR)/pdbase.cpp \
	$(SRC_DIR)/DSP.cpp \
	$(SRC_DIR)/SlewLimiter.cpp \
	$(SRC_DIR)/ParamFader.cpp \
	$(SRC_DIR)/DSPBuffer.cpp \
	$(SRC_DIR)/DSPObject.cpp \
	$(SRC_DIR)/DCBlocker.cpp \
	$(SRC_DIR)/ADSR.cpp \
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
	$(SRC_DIR)/KorgonFilter.cpp

# === Pure Data wrapper sources ===
JPVOICE_SRC = $(PD_SRC_DIR)/jpvoice~.cpp
ADSR_SRC    = $(PD_SRC_DIR)/adsr~.cpp

# === Object files ===
COMMON_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(COMMON_SOURCES))
JPVOICE_OBJECTS = $(patsubst $(PD_SRC_DIR)/%.cpp, $(OBJ_DIR)/puredata/%.o, $(JPVOICE_SRC))
ADSR_OBJECTS    = $(patsubst $(PD_SRC_DIR)/%.cpp, $(OBJ_DIR)/puredata/%.o, $(ADSR_SRC))

# === Targets ===
all: $(PD_TARGETS)

$(BIN_DIR)/jpvoice~.pd_linux: $(COMMON_OBJECTS) $(JPVOICE_OBJECTS)
	@mkdir -p $(BIN_DIR)
	@echo "Linking $@"
	$(CXX) -shared -o $@ $^

$(BIN_DIR)/adsr~.pd_linux: $(COMMON_OBJECTS) $(ADSR_OBJECTS)
	@mkdir -p $(BIN_DIR)
	@echo "Linking $@"
	$(CXX) -shared -o $@ $^

# === Compile .cpp to .o ===
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/puredata/%.o: $(PD_SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === Debug build ===
debug: CXXFLAGS += -O0 -g
debug: clean all

# === Release build ===
release: CXXFLAGS += -O3
release: clean all

# === Clean ===
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# === Dependencies ===
DEPS = $(JPVOICE_OBJECTS) $(ADSR_OBJECTS) $(COMMON_OBJECTS)
-include $(DEPS:.o=.d)

.PHONY: all clean debug release
