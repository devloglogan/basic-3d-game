CXX := clang++

CXXFLAGS := -g

# SDL3
CXXFLAGS += -I/opt/homebrew/include
LDFLAGS := -L/opt/homebrew/lib -Wl,-rpath,/opt/homebrew/lib -lSDL3

# GLEW
CXXFLAGS += -I/opt/homebrew/Cellar/glew/2.2.0_1/include
LDFLAGS += -L/opt/homebrew/Cellar/glew/2.2.0_1/lib -lGLEW

# GLM
CXXFLAGS += -I/opt/homebrew/Cellar/glm/1.0.1/include

# tinygltf
CXXFLAGS += -Ithirdparty/tinygltf

LDFLAGS += -framework OpenGL

SRC_DIR := src
BIN_DIR := bin

SRC += $(shell find $(SRC_DIR) -name '*.cpp')
OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%.o,$(SRC))
EXE := $(BIN_DIR)/main

all: $(EXE)

$(EXE): $(OBJ)
	@mkdir -p $(dir $@)
	$(CXX) --output $@ $^ $(LDFLAGS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) --compile $< --output $@ $(CXXFLAGS)

.PHONY: run
run: $(EXE)
	cd $(BIN_DIR) && ./$(notdir $(EXE))

.PHONY: clean
clean:
	rm -rf $(BIN_DIR)
