RAYLIB_PATH = C:/libs/raylib/
RAYLIB_INCLUDE = $(RAYLIB_PATH)/src
RAYLIB_LIBRARY = $(RAYLIB_PATH)/src

RAYGUI_INCLUDE = C:/libs/raygui/src

RAY = -I$(RAYLIB_INCLUDE) -I$(RAYGUI_INCLUDE) -L$(RAYLIB_LIBRARY)

BUILD_TYPE ?= RELEASE

OUT_DIR = ./out/

CC = gcc
CCFLAGS = $(RAY) -lraylib -lopengl32 -lgdi32 -lwinmm
LINK_FLAGS = 

PREPROC = 

ifeq ($(BUILD_TYPE),RELEASE)
	LINK_FLAGS += -Wl,--subsystem,windows
	PREPROC += -DRELEASE
endif

ifeq ($(BUILD_TYPE),DEBUG)
	LINK_FLAGS += -Wl,--subsystem,console
	PREPROC += -D_DEBUG
endif

SRC = ./src

all: check compile_tool

check:
	test -d $(OUT_DIR) || mkdir $(OUT_DIR)

compile_tool: $(SRC)/main.c $(SRC)/files_window.c $(SRC)/compiler_window.c $(SRC)/app_menu.c
	$(CC) -o $(OUT_DIR)/$@ $^ $(CCFLAGS) $(LINK_FLAGS) $(PREPROC)
	
run: compile_tool
	$(OUT_DIR)/compile_tool