#ifndef WINDOW_SYSTEM_H
#define WINDOW_SYSTEM_H

#include <stdint.h>

#include <raylib.h>
#include <raymath.h>

typedef uint32_t uint;              // Window ID maybe a better name

// WindowFnDrag Input State
typedef struct WindowDragInput
{
  Vector2 mousePos;                 // Current mouse position
  Rectangle clientArea;             // Space where windows are allowed to move, must clamp if desired behaivour is not going out of it
} WindowDragInput;

typedef struct WindowDragOutput
{
  Vector2 dragStartAbsolute;        // Drag start position absolute
  Vector2 dragStartAccumulative;    // Frame acummulated drag start position
} WindowDragOutput;

typedef struct WindowMoveInput
{
  Rectangle clientArea;             //
  Vector2 mouseDelta;               // = mousePos (from WindowFnDrag) - currentMousePos
  Vector2 dragAccumulated;          // = dragAccumulated + mouseDelta
  Vector2 dragAbsolute;             // = dragStartAbsolute + mouseDelta
} WindowMoveInput;

typedef struct _WindowSystem WindowSystem;

// Window Member Function used for checking dragging, should return true if dragging
typedef bool (*WindowFnDrag)(void *instance, const WindowDragInput *input, WindowDragOutput *output);

// Window Member Function used for processing drag movement
typedef void (*WindowFnMove)(void *instance, const WindowMoveInput input);

// Window Member Function used for drawing actual window
typedef void (*WindowFnDraw)(void *instance);

typedef struct WindowEntry
{
  uint id;            // Window ID for querying
  void *instance;     // Pointer to window state
  WindowFnDraw draw;  // Function for drawing the window 
  WindowFnDrag drag;  // Function for determining window drag
  WindowFnMove move;  // Function for processing window movement when dragging
} WindowEntry;

WindowSystem *WindowSystemInit(uint32_t initialWindowCount);
void WindowSystemFree(WindowSystem *state);

// Process drawing of windows
void WindowSystemDraw(WindowSystem *state);

// Process dragging of windows
void WindowSystemDrag(WindowSystem *state, WindowDragInput input);

// Process drag movement of windows
void WindowSystemMove(WindowSystem *state);

// Register a window
void WindowSystemRegister(WindowSystem *state, WindowEntry entry);

// Get a window entry by its ID
WindowEntry WindowSystemGetEntry(WindowSystem *state, uint windowID);

// Move window to the front of rendering queue
void WindowSystemMoveToFront(WindowSystem *state, uint windowID);

// Helper to make a WindowEntry
WindowEntry MakeWindowEntry(
  uint id, void *instance, 
  WindowFnDraw draw, 
  WindowFnDrag drag, 
  WindowFnMove move
);

#endif //!WINDOW_SYSTEM_H