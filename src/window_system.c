#include "window_system.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

struct _WindowSystem
{
  // Data for the system
  WindowEntry *windows;             // WindowEntry dynamic array
  size_t count;                     // Amount of windows registered
  size_t capacity;                  // Max capacity of windows array

  // Dragging internals
  Vector2 startMousePos;            // Starting position of mouse
  Vector2 currentMousePos;            // Current position of mouse
  uint draggingWindowID;            // ID of window that is being dragged
  bool draggingWindow;              // Can current frame process movement?
  
  // Input and Outputs for entries member functions
  WindowDragInput dragInput;        // WindowFnDrag Input
  WindowDragOutput dragOutput;      // WindowFnDrag Output
  WindowMoveInput moveInput;        // WindowFnMove Input
};

WindowSystem *WindowSystemInit(uint32_t initialWindowCount)
{
  // Some default capacity
  if (initialWindowCount == 0) initialWindowCount = 8;

  WindowSystem *state = RL_CALLOC(1, sizeof(struct _WindowSystem));

  assert(state != NULL && "WindowSystem allocation failed");
  
  state->capacity = initialWindowCount;
  state->windows = RL_CALLOC(state->capacity, sizeof(WindowEntry));
  assert(state->windows != NULL && "WindowSystem allocation failed");

  return state;
}

void WindowSystemFree(WindowSystem *state)
{
  if (state == NULL) return;
  RL_FREE(state->windows);
  RL_FREE(state);
  memset(state, 0, sizeof(struct _WindowSystem));
}

void WindowSystemDraw(WindowSystem *state)
{
  // Draw from back to front
  for (int32_t i = state->count - 1; i > -1; --i)
  {
    WindowEntry *entry = &state->windows[i];

    if (entry->draw != NULL) entry->draw(entry->instance);
    else  // MAYBE: Hide this else under a ifdef guard?
    {
      TraceLog(LOG_WARNING, "Window with ID: %u has no draw function bound", entry->id);
      continue;
    }
  }
}

void WindowSystemDrag(WindowSystem *state, WindowDragInput input)
{
  // Update client area and mouse position
  state->dragInput = input;
  state->currentMousePos = input.mousePos;

  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !state->draggingWindow)
  {
    // Process first drag from front to back
    for (size_t i=0; i<state->count; ++i)
    {
      WindowEntry *entry = &state->windows[i];
      if (entry->drag != NULL)
      {
        if (entry->drag(entry->instance, &state->dragInput, &state->dragOutput))
        {
          state->draggingWindow = true;
          state->draggingWindowID = entry->id;
          state->startMousePos = state->dragInput.mousePos;
          
          // Write accumulative in here because later is dragOutput accumulative will not used
          state->moveInput.dragAccumulated = state->dragOutput.dragStartAccumulative;
          break; // Stop, already dragging
        } 
      }
      else  // MAYBE: Hide this else under a ifdef guard?
      {
        TraceLog(LOG_WARNING, "Window with ID: %u has no drag function bound", entry->id);
        continue;;
      }
    }
  }
}

void WindowSystemMove(WindowSystem *state)
{ 
  // Return early if not dragging any window
  if (state->draggingWindow != true) return;

  // Compute move input variables
  state->moveInput.clientArea = state->dragInput.clientArea;
  state->moveInput.dragStart = state->dragOutput.dragStartAbsolute;

  state->moveInput.mouseDelta = Vector2Subtract(
    state->currentMousePos, state->startMousePos);

  state->moveInput.dragAbsolute = Vector2Add(
    state->dragOutput.dragStartAbsolute, state->moveInput.mouseDelta);

  state->moveInput.dragAccumulated = Vector2Add(
    state->moveInput.dragAccumulated, state->moveInput.mouseDelta);
  
  // Bring window to front of rendering queue
  WindowSystemMoveToFront(state, state->draggingWindowID);

  // Process movement of given window ID
  WindowEntry entry = WindowSystemGetEntry(state, state->draggingWindowID);

  if (entry.move != NULL) entry.move(entry.instance, state->moveInput);
  else  // MAYBE: Hide this else under a ifdef guard?
  {
    TraceLog(LOG_WARNING, "Window with ID: %u has no move function bound", entry.id);
  }

  // Reset dragging state on button release
  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) 
  {
    state->draggingWindow = false;
  }  
}
void WindowSystemRegister(WindowSystem *state, WindowEntry entry)
{
  assert(state != NULL && "NULL WindowSystem");

  // Grow dynamic array if space isn't enough
  if ((state->count + 1) > state->capacity)
  {
    state->capacity *= 1.5f;
    state->windows = RL_REALLOC(state->windows, sizeof(WindowEntry) * state->capacity);
    assert(state->windows != NULL && "WindowSystem allocation failed");
  }

  state->windows[state->count++] = entry;
}

WindowEntry WindowSystemGetEntry(WindowSystem *state, uint windowID)
{
  assert(state != NULL && "NULL WindowSystem");

  // Default index above last (which is an invalid ID)
  uint32_t index = state->count;

  for (size_t i = 0; i < state->count; ++i)
  {
    if (state->windows[i].id == windowID)
    {
      index = i;
      break;
    }
  }

  // Index is not valid (index is still above last entry)
  assert(index != state->count && "No entry bound to given ID");

  return state->windows[index];
}

void WindowSystemMoveToFront(WindowSystem *state, uint windowID)
{
  assert(state != NULL && "NULL WindowSystem");
  
  // Find window with given index
  // Default index above last (which is an invalid ID)
  uint32_t index = state->count;
  for (size_t i = 0; i < state->count; ++i)
  {
    if (state->windows[i].id == windowID)
    {
      index = i;
      break;
    }
  }

  // Index is not valid (index is still above last entry)
  assert(index != state->count && "No entry bound to given ID");

  // Window is already at the front
  if (index == 0) return;

  // Move entry to the front
  WindowEntry desired = state->windows[index];  // Temporary copy
  
  // Move back other entries
  for (size_t i = index; i > 0; --i)
  {
    state->windows[i] = state->windows[i - 1];
  }

  // Write copy to the front
  state->windows[0] = desired;
}

WindowEntry MakeWindowEntry(
  uint id, void *instance, 
  WindowFnDraw draw, 
  WindowFnDrag drag, 
  WindowFnMove move)
{
  WindowEntry entry = {0};
  entry.id = id;
  entry.instance = instance;
  entry.draw = draw;
  entry.drag = drag;
  entry.move = move;
  
  return entry;
}