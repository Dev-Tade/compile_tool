#ifndef APP_MENU_H
#define APP_MENU_H

#include "raygui.h"

#define APP_MENU_WIDTH 800
#define APP_MENU_HEIGHT 450

typedef struct {
    Vector2 MenuAnchor;
    
    bool MenuWindowActive;
    bool FilesButtonPressed;
    bool CompilerButtonPressed;
    bool OutputButtonPressed;

    Rectangle layoutRecs[4];

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required

    const char *WindowTitle;

} GuiAppMenuState;

// Forward declarations for functions defined at app_menu.c implementation file
GuiAppMenuState InitGuiAppMenu(void);
void GuiAppMenu(GuiAppMenuState *state);

#endif //!APP_MENU_H