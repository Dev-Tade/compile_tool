#ifndef FILES_WINDOW_H
#define FILES_WINDOW_H

#include "raygui.h"
#include "rglp.h"

typedef struct FilesWindow
{
    RGLayout layout;

    // Window status bar
    Rectangle dragHandle;

    RGLControl *window;
    RGLAnchor *anchor;

    bool windowActive;
    bool untrackFile;
    int selectedFileIndex;

    // Scroll bar state
    int listScrollIndex;

    // ';' separated list of tracked files
    char *filesText;
} FilesWindow;

void FilesWindowInit(FilesWindow *window, const char *filesWindowLayoutFile);
void GuiFilesWindow(FilesWindow *state);

#endif //!FILES_WINDOW_H