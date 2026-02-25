#ifndef COMPILER_WINDOW_H
#define COMPILER_WINDOW_H

#include "raygui.h"
#include "rglp.h"

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

#endif //!COMPILER_WINDOW_H