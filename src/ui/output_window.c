#include "output_window.h"

void OutputWindowInit(OutputWindow *output, const char *outputWindowLayoutFile)
{
  if (output == NULL) return;

  output->layout = LoadLayoutFile(outputWindowLayoutFile);

  // Expose window and anchor
  RGLControl *window = GetControlByName(&output->layout, "OutputWindow");
  output->anchor = GetAnchorById(&output->layout, window->anchorID);
  output->window = window;

  // Expose window drag handle (status bar)
  output->dragHandle = GetControlRect(&output->layout, window);
  output->dragHandle.height = 24; // raygui window statusbar height 

  // Default state
  output->windowActive = true;
  
  output->outputText = NULL;
  output->exitCode = 0;
  output->commandState = 0;
}

void GuiOutputWindow(OutputWindow *output)
{
  int screenWidth = GetScreenWidth();
  int screenHeight = GetScreenHeight();

  // Move window down and expand to full width
  RGLControl *window = GetControlByName(&output->layout, "OutputWindow");
  RGLAnchor *anchor = GetAnchorById(&output->layout, window->anchorID);

  anchor->pos.y = screenHeight - window->rect.height;
  window->rect.width = screenWidth;

  // Update drag handle
  Rectangle windowRect = GetControlRect(&output->layout, window);
  output->dragHandle = windowRect;
  output->dragHandle.height = 24; // raygui window status bar

  if (output->windowActive)
  {
    const char *title = window->text;
    
    // CMD_PENDING
    if (output->commandState == 1)
    {
      const char *status = "Compilation in progress";
      title = TextFormat("%s - %s", window->text, status); 
    } // CMD_DONE
    else if (output->commandState == 2)
    {
      const char *status = output->exitCode == 0 ? 
        "Compilation Succesful" :
        "Compilation Failed";

      title = TextFormat("%s - %s (Exit Code: %d)", window->text, status, output->exitCode);
    } // CMD_FAILED 
    else if (output->commandState == 3)
    {
      const char *status = "Process Spawn Failed";
      title = TextFormat("%s - %s (Exit Code: %d)", window->text, status, output->exitCode);
    }

    output->windowActive = !GuiWindowBox(windowRect, title);

    // Update scroll panel to be inside window
    RGLControl *scrollPanel = GetControlByName(&output->layout, "OutputTextScroll");
    scrollPanel->rect.width = screenWidth;
    scrollPanel->rect.height = GetControlRect(&output->layout, window).height;

    output->scrollContent = GetControlRect(&output->layout, scrollPanel);
    output->scrollContent.height = 1000;

    GuiScrollPanel(GetControlRect(&output->layout, scrollPanel), NULL, output->scrollContent, &output->scrollOffset, &output->scrollView);

      BeginScissorMode(output->scrollView.x, output->scrollView.y, output->scrollView.width, output->scrollView.height);

      Rectangle text_rect = {output->scrollContent.x + output->scrollOffset.x, output->scrollContent.y + output->scrollOffset.y, output->scrollContent.width, output->scrollContent.height};

      int prevTextAlignment = GuiGetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL);
      int prevTextWrap = GuiGetStyle(DEFAULT, TEXT_WRAP_MODE);
      
      GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_TOP);
      GuiSetStyle(DEFAULT, TEXT_WRAP_MODE, TEXT_WRAP_WORD); // TEXT_WRAP_CHAR
      
      GuiLabel(text_rect, output->outputText);
      
      GuiSetStyle(DEFAULT, TEXT_WRAP_MODE, prevTextWrap);
      GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, prevTextAlignment);

      EndScissorMode();
  }
}

bool OutputWindowDrag(OutputWindow *outputW, const WindowDragInput *input, WindowDragOutput *output)
{
  if (!CheckCollisionPointRec(input->mousePos, outputW->dragHandle)) return false;

  output->dragStartAbsolute = (Vector2){0, outputW->window->rect.height};
  return true;
}

void OutputWindowMove(OutputWindow *outputW, const WindowMoveInput input)
{
  Vector2 dragAbsolute = Vector2Add(input.dragStart, Vector2Negate(input.mouseDelta));
  
  outputW->window->rect.height = Clamp(dragAbsolute.y, 
    input.clientArea.y, input.clientArea.height - input.clientArea.y);
}