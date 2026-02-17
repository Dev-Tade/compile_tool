#include <string.h>
#include "files_window.h"

// Initialization function
GuiFilesWindowState InitGuiFilesWindow(void)
{
    GuiFilesWindowState state = { 0 };

    state.FilesWindowAnchor = (Vector2){ 24, 96 };
    
    state.FilesWindowActive = true;
    state.FilesListScrollIndex = 0;
    state.FilesListActive = 0;
    state.UntrackFileButtonPressed = false;

    state.layoutRecs[0] = (Rectangle){ state.FilesWindowAnchor.x + 0, state.FilesWindowAnchor.y + 0, 280, 392 };
    state.layoutRecs[1] = (Rectangle){ state.FilesWindowAnchor.x + 8, state.FilesWindowAnchor.y + 32, 264, 32 };
    state.layoutRecs[2] = (Rectangle){ state.FilesWindowAnchor.x + 8, state.FilesWindowAnchor.y + 72, 264, 264 };
    state.layoutRecs[3] = (Rectangle){ state.FilesWindowAnchor.x + 8, state.FilesWindowAnchor.y + 344, 264, 40 };

    // Custom variables initialization

    return state;
}

// Rendering/Logic function
void GuiFilesWindow(GuiFilesWindowState *state)
{
    const char *FilesWindowText = "Files";
    const char *TracekdFilesLabelText = "Tracked Files (Drop into window to add)";
    const char *FilesListText = state->files_text;
    const char *UntrackFileButtonText = "Untrack selected file";
    
    // Hack so it doesn't render any text and you can't select
    // anything under the list view
    if (TextLength(FilesListText) == 0) 
    {
        FilesListText = NULL;
        state->FilesListActive = -1;
    }

    if (state->FilesWindowActive)
    {
        state->FilesWindowActive = !GuiWindowBox(state->layoutRecs[0], FilesWindowText);
        GuiLabel(state->layoutRecs[1], TracekdFilesLabelText);
        GuiListView(state->layoutRecs[2], FilesListText, &state->FilesListScrollIndex, &state->FilesListActive);

        if (!(state->FilesListActive > -1 && TextLength(FilesWindowText) > 0)) GuiDisable();
        state->UntrackFileButtonPressed = GuiButton(state->layoutRecs[3], UntrackFileButtonText);
        GuiEnable(); 
    }
}