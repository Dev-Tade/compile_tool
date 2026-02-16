#ifndef FILES_WINDOW_H
#define FILES_WINDOW_H

#include "raygui.h"

typedef struct {
    Vector2 FilesWindowAnchor;
    
    bool FilesWindowActive;
    int FilesListScrollIndex;
    int FilesListActive;
    bool UntrackFileButtonPressed;

    Rectangle layoutRecs[4];

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required
    char *files_text;
    int files_text_pos;
} GuiFilesWindowState;

// Forward declarations for functions defined at files_window.c implementation file
GuiFilesWindowState InitGuiFilesWindow(void);
void GuiFilesWindow(GuiFilesWindowState *state);

#endif //!FILES_WINDOW_H