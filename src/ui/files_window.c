#include "files_window.h"

void FilesWindowInit(FilesWindow *files, const char *filesWindowLayoutFile)
{
  if (files == NULL) return;

  files->layout = LoadLayoutFile(filesWindowLayoutFile);

  // Expose window and anchor
  RGLControl *window = GetControlByName(&files->layout, "FilesWindow");
  files->anchor = GetAnchorById(&files->layout, window->anchorID);
  files->window = window;

  // Expose window drag handle (status bar)
  files->dragHandle = GetControlRect(&files->layout, window);
  files->dragHandle.height = 24; // raygui window statusbar height

  // Empty list of files
  files->filesText = "";

  // Default state
  files->windowActive = true;
  files->untrackFile = false;
  files->selectedFileIndex = -1;
}

// Rendering/Logic function
void GuiFilesWindow(FilesWindow *files)
{
  // Update drag handle
  RGLControl *window = GetControlByName(&files->layout, "FilesWindow");
  
  Rectangle windowRect = GetControlRect(&files->layout, window);
  files->dragHandle = windowRect;
  files->dragHandle.height = 24; // raygui window statusbar height

  // Don't render filesText if there isn't any content
  const char *renderedListText = files->filesText;
  if (TextLength(files->filesText) <= 0) 
  {
    renderedListText = NULL;
    files->selectedFileIndex = -1;
  }

  if (files->windowActive)
  {
    files->windowActive = !GuiWindowBox(windowRect, window->text);

    RGLControl *label = GetControlByName(&files->layout, "FilesLabel");
    GuiLabel(GetControlRect(&files->layout, label), label->text);

    RGLControl *filesList = GetControlByName(&files->layout, "TrackedFilesList");
    GuiListView(GetControlRect(&files->layout, filesList), files->filesText, &files->listScrollIndex, &files->selectedFileIndex);

    // Disable untrack button if there isn't a file selected or if there isn't files tracked 
    if (!(files->selectedFileIndex >= 0 && TextLength(files->filesText) > 0))
      GuiDisable();

    RGLControl *untrackButton = GetControlByName(&files->layout, "UntrackFileButton");
    files->untrackFile = GuiButton(GetControlRect(&files->layout, untrackButton), untrackButton->text);

    // Enable further UI elements
    GuiEnable(); 
  }
}