#ifndef COMPILER_WINDOW_H
#define COMPILER_WINDOW_H

#include "raygui.h"

typedef struct {
    Vector2 CompilerWindowAnchor;
    
    bool CompilerWindowActive;
    bool AdditionalIncludesTextboxEditMode;
    char AdditionalIncludesTextboxText[128];
    bool AdditionalLibrariesTextboxEditMode;
    char AdditionalLibrariesTextboxText[128];
    bool LinkLibrariesTextboxEditMode;
    char LinkLibrariesTextboxText[128];
    bool OutputPathTextboxEditMode;
    char OutputPathTextboxText[128];
    bool CompileButtonPressed;

    Rectangle layoutRecs[11];

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required

} GuiCompilerWindowState;

// Forward declarations for functions defined at compiler_window.c implementation file
GuiCompilerWindowState InitGuiCompilerWindow(void);
void GuiCompilerWindow(GuiCompilerWindowState *state);

#endif //!COMPILER_WINDOW_H