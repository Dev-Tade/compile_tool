#include <string.h>
#include "app_menu.h"

void AppMenuInit(AppMenu *menu, const char *appMenuLayoutFile)
{
  if (menu == NULL) return;
  
  menu->layout = LoadLayoutFile(appMenuLayoutFile);

  // Initialize menu status bar related variables
  RGLControl *statusBar = GetControlByName(&menu->layout, "AppStatusBar");
  menu->windowTitle = statusBar->text;
  menu->dragHandle = GetControlRect(&menu->layout, statusBar);

  // Defailt state
  menu->shouldClose = false;
}

void GuiAppMenu(AppMenu *menu)
{
  int screenWidth = GetScreenWidth();

  // Move right anchor x position to the end of the window
  RGLAnchor *rightAnchor = GetAnchorById(&menu->layout, 2);
  rightAnchor->pos.x = screenWidth;

  // Expand status bar to occupy full window width
  RGLControl *statusBar = GetControlByName(&menu->layout, "AppStatusBar");
  statusBar->rect.width = screenWidth;

  // Update window drag handle
  Rectangle statusBarRect = GetControlRect(&menu->layout, statusBar);
  menu->dragHandle = statusBarRect;

  // Draw status bar
  GuiStatusBar(statusBarRect, menu->windowTitle);

  // Draw close button
  RGLControl *closeButton = GetControlByName(&menu->layout, "CloseButton");
  if (GuiButton(GetControlRect(&menu->layout, closeButton), closeButton->text))
  {
    menu->shouldClose = true;
  }
}