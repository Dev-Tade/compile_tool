#include <string.h>
#include "app_menu.h"

// Initialization function
GuiAppMenuState InitGuiAppMenu(void)
{
    GuiAppMenuState state = { 0 };

    state.MenuAnchor = (Vector2){ 0, 0 };
    
    state.MenuWindowActive = true;
    state.FilesButtonPressed = false;
    state.CompilerButtonPressed = false;
    state.OutputButtonPressed = false;

    state.layoutRecs[0] = (Rectangle){ 0, 0, 800, 72 };
    state.layoutRecs[1] = (Rectangle){ state.MenuAnchor.x + 8, state.MenuAnchor.y + 32, 152, 32 };
    state.layoutRecs[2] = (Rectangle){ state.MenuAnchor.x + 168, state.MenuAnchor.y + 32, 152, 32 };
    state.layoutRecs[3] = (Rectangle){ state.MenuAnchor.x + 328, state.MenuAnchor.y + 32, 152, 32 };

    // Custom variables initialization

    return state;
}

// Rendering/Logic function
void GuiAppMenu(GuiAppMenuState *state)
{
    const char *MenuWindowText = state->WindowTitle;
    const char *FilesButtonText = "Files";
    const char *CompilerButtonText = "Compiler";
    const char *OutputButtonText = "Output";
    
    if (state->MenuWindowActive)
    {
        state->MenuWindowActive = !GuiWindowBox(state->layoutRecs[0], MenuWindowText);
    }
    state->FilesButtonPressed = GuiButton(state->layoutRecs[1], FilesButtonText); 
    state->CompilerButtonPressed = GuiButton(state->layoutRecs[2], CompilerButtonText); 
    state->OutputButtonPressed = GuiButton(state->layoutRecs[3], OutputButtonText); 
}