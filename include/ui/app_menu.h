#ifndef APP_MENU_H
#define APP_MENU_H

#include "raygui.h"
#include "rglp.h"

#include "window_system.h"

typedef struct AppMenu
{
    RGLayout layout;
    Rectangle dragHandle;

    const char *windowTitle;
    bool shouldClose;
} AppMenu;

void AppMenuInit(AppMenu *menu, const char *appMenuLayoutFile);
void GuiAppMenu(AppMenu *state);

bool AppMenuDrag(AppMenu *menu, const WindowDragInput *input, WindowDragOutput *output);
void AppMenuMove(AppMenu *menu, const WindowMoveInput input);

#endif //!APP_MENU_H