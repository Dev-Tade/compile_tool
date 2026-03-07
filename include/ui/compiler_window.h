#ifndef COMPILER_WINDOW_H
#define COMPILER_WINDOW_H

#include "raygui.h"
#include "rglp.h"

#include "window_system.h"

typedef struct CompilerWindow
{
    RGLayout layout;

    // Windows status bar
    Rectangle dragHandle;

    RGLControl *window;
    RGLAnchor *anchor;

    bool windowActive;
    bool compile;
    
    char *includeText;
    char *libraryText;
    char *linkText;
    char *outputText;

    bool includeTextEdit;
    bool libraryTextEdit;
    bool linkTextEdit;
    bool outputTextEdit;

} CompilerWindow;

void CompilerWindowInit(CompilerWindow *compiler, const char *compilerWindowLayoutFile);
void GuiCompilerWindow(CompilerWindow *state);

bool CompilerWindowDrag(CompilerWindow *compiler, const WindowDragInput *input, WindowDragOutput *output);
void CompilerWindowMove(CompilerWindow *compiler, const WindowMoveInput input);

#endif //!COMPILER_WINDOW_H