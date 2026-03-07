#ifndef OUTPUT_WINDOW_H
#define OUTPUT_WINDOW_H

#include "raygui.h"
#include "rglp.h"

#include "window_system.h"

typedef struct OutputWindow
{
  RGLayout layout;

  // Windows status bar
  Rectangle dragHandle;

  RGLControl *window;
  RGLAnchor *anchor;

  // Scroll panel state
  Rectangle scrollContent;
  Rectangle scrollView;
  Vector2 scrollOffset;


  // Window state
  bool windowActive;

  char *outputText;
  int exitCode;
  int commandState;

} OutputWindow;

void OutputWindowInit(OutputWindow *output, const char *outputWindowLayoutFile);
void GuiOutputWindow(OutputWindow *state);

bool OutputWindowDrag(OutputWindow *outputW, const WindowDragInput *input, WindowDragOutput *output);
void OutputWindowMove(OutputWindow *outputW, const WindowMoveInput input);

#endif //!OUTPUT_WINDOW_H