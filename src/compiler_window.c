#include <string.h>
#include "compiler_window.h"

// Initialization function
GuiCompilerWindowState InitGuiCompilerWindow(void)
{
    GuiCompilerWindowState state = { 0 };

    state.CompilerWindowAnchor = (Vector2){ 324, 96 };
    
    state.CompilerWindowActive = true;
    state.AdditionalIncludesTextboxEditMode = false;
    strcpy(state.AdditionalIncludesTextboxText, "");
    state.AdditionalLibrariesTextboxEditMode = false;
    strcpy(state.AdditionalLibrariesTextboxText, "");
    state.LinkLibrariesTextboxEditMode = false;
    strcpy(state.LinkLibrariesTextboxText, "");
    state.OutputPathTextboxEditMode = false;
    strcpy(state.OutputPathTextboxText, "");
    state.CompileButtonPressed = false;

    state.layoutRecs[0] = (Rectangle){ state.CompilerWindowAnchor.x + 0, state.CompilerWindowAnchor.y + 0, 280, 392 };
    state.layoutRecs[1] = (Rectangle){ state.CompilerWindowAnchor.x + 8, state.CompilerWindowAnchor.y + 32, 264, 32 };
    state.layoutRecs[2] = (Rectangle){ state.CompilerWindowAnchor.x + 8, state.CompilerWindowAnchor.y + 104, 264, 32 };
    state.layoutRecs[3] = (Rectangle){ state.CompilerWindowAnchor.x + 8, state.CompilerWindowAnchor.y + 176, 264, 32 };
    state.layoutRecs[4] = (Rectangle){ state.CompilerWindowAnchor.x + 8, state.CompilerWindowAnchor.y + 248, 264, 32 };
    state.layoutRecs[5] = (Rectangle){ state.CompilerWindowAnchor.x + 8, state.CompilerWindowAnchor.y + 56, 264, 40 };
    state.layoutRecs[6] = (Rectangle){ state.CompilerWindowAnchor.x + 8, state.CompilerWindowAnchor.y + 128, 264, 40 };
    state.layoutRecs[7] = (Rectangle){ state.CompilerWindowAnchor.x + 8, state.CompilerWindowAnchor.y + 200, 264, 40 };
    state.layoutRecs[8] = (Rectangle){ state.CompilerWindowAnchor.x + 8, state.CompilerWindowAnchor.y + 272, 264, 40 };
    state.layoutRecs[9] = (Rectangle){ state.CompilerWindowAnchor.x + 8, state.CompilerWindowAnchor.y + 336, 264, 40 };
    state.layoutRecs[10] = (Rectangle){ state.CompilerWindowAnchor.x + 8, state.CompilerWindowAnchor.y + 320, 264, 16 };

    // Custom variables initialization

    return state;
}

// Rendering/Logic function
void GuiCompilerWindow(GuiCompilerWindowState *state)
{
    const char *CompilerWindowText = "Compiler";
    const char *IncludePathLabelText = "Additional include path(s) (; separated)";
    const char *LibraryPathLabelText = "Additional library path(s) (; separated)";
    const char *LinkLibrariesLabelText = "Link libraries (; separated)";
    const char *OutputPathLabelText = "Output name/path";
    const char *CompileButtonText = "Compile";
    
    if (state->CompilerWindowActive)
    {
        state->CompilerWindowActive = !GuiWindowBox(state->layoutRecs[0], CompilerWindowText);
        GuiLabel(state->layoutRecs[1], IncludePathLabelText);
        GuiLabel(state->layoutRecs[2], LibraryPathLabelText);
        GuiLabel(state->layoutRecs[3], LinkLibrariesLabelText);
        GuiLabel(state->layoutRecs[4], OutputPathLabelText);
        if (GuiTextBox(state->layoutRecs[5], state->AdditionalIncludesTextboxText, 128, state->AdditionalIncludesTextboxEditMode)) state->AdditionalIncludesTextboxEditMode = !state->AdditionalIncludesTextboxEditMode;
        if (GuiTextBox(state->layoutRecs[6], state->AdditionalLibrariesTextboxText, 128, state->AdditionalLibrariesTextboxEditMode)) state->AdditionalLibrariesTextboxEditMode = !state->AdditionalLibrariesTextboxEditMode;
        if (GuiTextBox(state->layoutRecs[7], state->LinkLibrariesTextboxText, 128, state->LinkLibrariesTextboxEditMode)) state->LinkLibrariesTextboxEditMode = !state->LinkLibrariesTextboxEditMode;
        if (GuiTextBox(state->layoutRecs[8], state->OutputPathTextboxText, 128, state->OutputPathTextboxEditMode)) state->OutputPathTextboxEditMode = !state->OutputPathTextboxEditMode;
        state->CompileButtonPressed = GuiButton(state->layoutRecs[9], CompileButtonText); 
        GuiLine(state->layoutRecs[10], NULL);
    }
}