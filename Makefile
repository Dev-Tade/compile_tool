RAYLIB_PATH = C:/libs/raylib/
RAYLIB_INCLUDE = $(RAYLIB_PATH)/src
RAYLIB_LIBRARY = $(RAYLIB_PATH)/src

RAYGUI_INCLUDE = C:/libs/raygui/src

RAY = -I$(RAYLIB_INCLUDE) -I$(RAYGUI_INCLUDE) -L$(RAYLIB_LIBRARY)

BUILD_TYPE ?= RELEASE

RGLP_PATH = ./rglp/src

OUT_DIR = ./out/
INCLUDE = ./include/

CC = gcc
CCFLAGS = -I$(INCLUDE) -I$(RGLP_PATH) $(RAY) -lraylib
LINK_FLAGS = 

PREPROC = 

ifeq ($(OS),Windows_NT)
	CCFLAGS += -lopengl32 -lgdi32 -lwinmm
endif

ifeq ($(BUILD_TYPE),RELEASE)
	ifeq ($(OS),Windows_NT)
		LINK_FLAGS += -Wl,--subsystem,windows
	endif
	PREPROC += -DRELEASE
endif

ifeq ($(BUILD_TYPE),DEBUG)
	ifeq ($(OS),Windows_NT)
		LINK_FLAGS += -Wl,--subsystem,console
	endif
	PREPROC += -D_DEBUG
	CCFLAGS += -g
endif

SRC = ./src
SRC_FILES = $(SRC)/main.c $(SRC)/app_menu.c $(SRC)/files_window.c $(SRC)/compiler_window.c $(SRC)/output_window.c $(SRC)/platform_common.c

ifeq ($(OS),Windows_NT)
	SRC_FILES += $(SRC)/platform_win32.c
else
	SRC_FILES += $(SRC)/platform_posix.c
endif

all: check compile_tool

check:
	test -d $(OUT_DIR) || mkdir $(OUT_DIR)

compile_tool: $(OUT_DIR)/rglp.o $(SRC_FILES)
	$(CC) -o $(OUT_DIR)/$@ $^ $(CCFLAGS) $(LINK_FLAGS) $(PREPROC)

$(OUT_DIR)/rglp.o: $(SRC)/rglp.c
	$(CC) -o $@ -c $^ $(CCFLAGS)

run: compile_tool
	$(OUT_DIR)/compile_tool