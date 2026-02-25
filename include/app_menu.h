#ifndef APP_MENU_H
#define APP_MENU_H

#include "raygui.h"
#include "rglp.h"

typedef struct AppMenu
{
    RGLayout layout;
    Rectangle dragHandle;

    const char *windowTitle;
    bool shouldClose;
} AppMenu;

void AppMenuInit(AppMenu *menu, const char *appMenuLayoutFile);
void GuiAppMenu(AppMenu *state);

#endif //!APP_MENU_H