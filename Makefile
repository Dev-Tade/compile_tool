RAYLIB_PATH = C:/libs/raylib/
RAYLIB_INCLUDE = $(RAYLIB_PATH)/src
RAYLIB_LIBRARY = $(RAYLIB_PATH)/src

RAYGUI_INCLUDE = C:/libs/raygui/src

RAY = -I$(RAYLIB_INCLUDE) -I$(RAYGUI_INCLUDE) -L$(RAYLIB_LIBRARY)

BUILD_TYPE ?= RELEASE

RGLP_PATH = ./rglp/src

OUT_DIR = ./out
INCLUDE = ./include
UI_INCLUDE = $(INCLUDE)/ui

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
UI_SRC = $(SRC)/ui

SRC_FILES = $(SRC)/main.c $(SRC)/platform_common.c $(SRC)/window_system.c
UI_SRC_FILES = $(wildcard $(UI_SRC)/*.c)

UI_OUT_FILES = $(patsubst %.c,$(OUT_DIR)/%.o,$(notdir $(UI_SRC_FILES)))

ifeq ($(OS),Windows_NT)
	SRC_FILES += $(SRC)/platform_win32.c
else
	SRC_FILES += $(SRC)/platform_posix.c
endif

all: check compile_tool

check:
	test -d $(OUT_DIR) || mkdir $(OUT_DIR)

compile_tool: $(SRC_FILES) $(OUT_DIR)/rglp.o $(UI_OUT_FILES)
	$(CC) -o $(OUT_DIR)/$@ $^ $(CCFLAGS) $(LINK_FLAGS) $(PREPROC)

$(OUT_DIR)/rglp.o: $(SRC)/rglp.c
	$(CC) -o $@ -c $^ $(CCFLAGS)

$(OUT_DIR)/%.o: $(UI_SRC)/%.c
	$(CC) -o $@ -c $^ $(CCFLAGS) $(PREPROC) -I$(UI_INCLUDE)

run: compile_tool
	$(OUT_DIR)/compile_tool