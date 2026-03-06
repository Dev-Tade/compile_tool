#include <stdlib.h>

#include "app_menu.h"
#include "files_window.h"
#include "compiler_window.h"
#include "output_window.h"

#include "window_sorting.h"

#include "platform.h"

#include <raylib.h>
#include <raymath.h>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#define TOOL_NAME "Compile Tool"
#define TOOL_VERSION "ver. 0.3"

#if defined(_DEBUG) && !defined(RELEASE)
#define TOOL_BUILD_TYPE "Debug"
#elif defined(RELEASE) && !defined(_DEBUG)
#define TOOL_BUILD_TYPE "Release"
#else
#error "Unkown build type"
#endif

FilePathList AllocFilePathList(size_t maxFilePathSize, size_t maxPathCount);
void FreeFilePathList(FilePathList *list, size_t allocatedPathCount);

bool IsFileAlreadyTracked(const FilePathList *trackedFiles, const char *filepath);
int TrackFile(FilePathList *trackedFiles, const char *filepath, const char *workingDir);
int UntrackFile(FilePathList *trackedFiles, const int index);

const int TRACKED_FILES_CAPACITY = 256;
const int TRACKED_FILE_PATH_SIZE = 256; 

// void BringWindowToFront(WindowEntry windows[], size_t windows_count, uint id)
// {
//   if (windows_count == 0) return;

//   uint index = windows_count;
//   // Find window with given ID
//   for (size_t i=0; i<windows_count; i++)
//   {
//     if (windows[i].id == id)
//     {
//       index = i; 
//       break;
//     }
//   }

//   if (index == windows_count) return;
//   if (index == 0) return;

//   WindowEntry copy = windows[index];
//   WindowEntry temp = windows[index];

//   // Desplazar hacia atrás
//   for (size_t i = index; i > 0; i--)
//   {
//     windows[i] = windows[i - 1];
//   }

//   windows[0] = temp;
// }

int main()
{
  const char *WINDOW_TITLE = TOOL_NAME" "TOOL_VERSION" - "TOOL_BUILD_TYPE;
  SetTraceLogLevel(LOG_ALL);
  SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST);
  InitWindow(800, 650, WINDOW_TITLE);
  SetTargetFPS(60);
  
  GuiLoadStyle("styles/style_genesis.rgs");
  GuiSetStyle(DEFAULT, TEXT_SIZE, 18);

  const char *appMenuLayoutFile = "./layouts/app_menu.rgl";
  const char *filesWindowLayoutFile = "./layouts/files_window.rgl";
  const char *compilerWindowLayoutFile = "./layouts/compiler_window.rgl";
  const char *outputWindowLayoutFile = "./layouts/output_window.rgl";

  AppMenu appMenu = {0};
  AppMenuInit(&appMenu, appMenuLayoutFile);
  appMenu.windowTitle = WINDOW_TITLE;
  
  FilesWindow filesWindow = {0};
  FilesWindowInit(&filesWindow, filesWindowLayoutFile);

  CompilerWindow compilerWindow = {0};
  CompilerWindowInit(&compilerWindow, compilerWindowLayoutFile);

  OutputWindow outputWindow = {0};
  OutputWindowInit(&outputWindow, outputWindowLayoutFile);

  // WindowEntry windows[] = 
  // {
  //   {1, &filesWindow, (void (*)(void *instance))GuiFilesWindow},
  //   {2, &compilerWindow, (void (*)(void *instance))GuiCompilerWindow},
  //   {3, &outputWindow, (void (*)(void *instance))GuiOutputWindow},
  // };

  // size_t windows_count = sizeof(windows)/sizeof(WindowEntry);

  WindowEntries windows = WindowEntriesInit(8);
  WindowEntriesPush(&windows, (WindowEntry){&filesWindow, (WindowDrawFn)GuiFilesWindow});
  WindowEntriesPush(&windows, (WindowEntry){&compilerWindow, (WindowDrawFn)GuiCompilerWindow});
  WindowEntriesPush(&windows, (WindowEntry){&outputWindow, (WindowDrawFn)GuiOutputWindow});

  bool debugDrags = false;

  bool draggingWindow = false;
  int draggingID = -1;
  Vector2 dragStartPos = Vector2Zero();
  Vector2 dragStartMouse = Vector2Zero();
  // Used for system window only
  Vector2 dragAccumPos = Vector2Zero();

  filesWindow.filesText = RL_CALLOC(4096, sizeof(char));

  FilePathList trackedFiles = AllocFilePathList(TRACKED_FILE_PATH_SIZE, TRACKED_FILES_CAPACITY);
  int newlyTrackedFiles = 0;

  char compile_command[2048] = {0};
  int compile_command_pos = 0;

  char *commandOutputCopy = NULL;
  
  CmdRunner compileCommand = {0};

  while (!appMenu.shouldClose && !WindowShouldClose())
  {
    Rectangle clientArea = (Rectangle)
    {
      .x = appMenu.dragHandle.x,
      .y = appMenu.dragHandle.height,
      .width = appMenu.dragHandle.width,
      .height = GetScreenHeight()
    };

    // Enable debug overlays
    if (IsKeyPressed(KEY_F3))
    {
      debugDrags = !debugDrags;
    }

    // Layout reloading
    if (IsKeyPressed(KEY_F5))
    {
      UnloadLayout(&appMenu.layout);
      appMenu.layout = LoadLayoutFile(appMenuLayoutFile);
      UnloadLayout(&filesWindow.layout);
      filesWindow.layout = LoadLayoutFile(filesWindowLayoutFile);
      UnloadLayout(&compilerWindow.layout);
      compilerWindow.layout = LoadLayoutFile(compilerWindowLayoutFile);
    }

    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !draggingWindow)
    {
      if (CheckCollisionPointRec(mousePos, appMenu.dragHandle))
      {
        draggingWindow = true;
        draggingID = 0;

        dragAccumPos = dragStartPos = GetWindowPosition();
        dragStartMouse = mousePos;
      } 
      else if (CheckCollisionPointRec(mousePos, filesWindow.dragHandle))
      {
        draggingWindow = true;
        draggingID = 1;

        dragStartPos = filesWindow.anchor->pos;
        dragStartMouse = mousePos;
      }
      else if (CheckCollisionPointRec(mousePos, compilerWindow.dragHandle))
      {
        draggingWindow = true;
        draggingID = 2;

        dragStartPos = compilerWindow.anchor->pos;
        dragStartMouse = mousePos;
      }
      else if (CheckCollisionPointRec(mousePos, outputWindow.dragHandle))
      {
        draggingWindow = true;
        draggingID = 3;

        dragStartPos = (Vector2){0, outputWindow.window->rect.height};
        dragStartMouse = mousePos;
      }
      
    }

    if (draggingWindow)
    { 
      Vector2 mouseDelta = Vector2Subtract(mousePos, dragStartMouse);
      Vector2 dragAbs = Vector2Add(dragStartPos, mouseDelta);

      dragAccumPos = Vector2Add(dragAccumPos, mouseDelta);

      switch (draggingID)
      {
        default: break; // Skip unhandled IDs
        case 0: // System window
        {
          SetWindowPosition((int)dragAccumPos.x, (int)dragAccumPos.y);
        } break;

        case 1: // Files window
        {
          WindowEntriesUpfront(&windows, &filesWindow);

          float windowWidth = filesWindow.window->rect.width;
          float windowHeight = filesWindow.window->rect.height;

          dragAbs.x = Clamp(dragAbs.x, clientArea.x, clientArea.width - windowWidth);
          dragAbs.y = Clamp(dragAbs.y, clientArea.y, clientArea.height - windowHeight);
          
          filesWindow.anchor->pos = dragAbs;
        } break;

        case 2: // Compiler window
        {
          WindowEntriesUpfront(&windows, &compilerWindow);
          float windowWidth = compilerWindow.window->rect.width;
          float windowHeight = compilerWindow.window->rect.height;

          dragAbs.x = Clamp(dragAbs.x, clientArea.x, clientArea.width - windowWidth);
          dragAbs.y = Clamp(dragAbs.y, clientArea.y, clientArea.height - windowHeight);
          
          compilerWindow.anchor->pos = dragAbs;
        } break;
        
        case 3: // Output window
        {
          WindowEntriesUpfront(&windows, &outputWindow);
          Vector2 dragAbs = Vector2Add(dragStartPos, Vector2Negate(mouseDelta));

          outputWindow.window->rect.height = Clamp(dragAbs.y, clientArea.y, clientArea.height - clientArea.y);
        } break;
      }
      
      // Reset dragging state on button release
      if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) 
      {
        draggingWindow = false;
        draggingID = -1;
      }  
    }

    if (IsFileDropped()) 
    {
      FilePathList droppedFiles = LoadDroppedFiles();
      const char *workingDir = GetWorkingDirectory();
      TraceLog(LOG_DEBUG, "Working dir: %s", workingDir);
      
      for (int i=0; i<droppedFiles.count; ++i)
      {
        char *file = droppedFiles.paths[i];
        
        // Recursively add directories
        if (DirectoryExists(file))
        {
          TraceLog(LOG_DEBUG, "Tracking Directory %s", file);
          FilePathList dir_files = LoadDirectoryFiles(file);

          for (int i = 0; i < dir_files.count; ++i)
          {
            const char *dir_file = dir_files.paths[i];
            newlyTrackedFiles += TrackFile(&trackedFiles, dir_file, workingDir);
          }

          UnloadDirectoryFiles(dir_files);
        }
        else // File
        {
          newlyTrackedFiles += TrackFile(&trackedFiles, file, workingDir);
        }
      }
      
      UnloadDroppedFiles(droppedFiles);
    }

    if (filesWindow.untrackFile)
    {
      int index = filesWindow.selectedFileIndex;
      newlyTrackedFiles -= UntrackFile(&trackedFiles, index);
      filesWindow.selectedFileIndex = index;
    }

    if (newlyTrackedFiles != 0)
    {
      TraceLog(LOG_DEBUG, "Updating tracked files!");
      if (newlyTrackedFiles < 0) TraceLog(LOG_DEBUG, "%d files untracked", abs(newlyTrackedFiles));
      else if (newlyTrackedFiles > 0) TraceLog(LOG_DEBUG, "%d new files tracked", newlyTrackedFiles);

      char *joinedFiles = TextJoin(trackedFiles.paths, trackedFiles.count, ";");
      TextCopy(filesWindow.filesText, joinedFiles);
      
      newlyTrackedFiles = 0;
    }

    if (compilerWindow.compile && CommandRunnerIs(&compileCommand, CMD_UNITIALIZED))
    {
      TraceLog(LOG_INFO, "Compile!");
      compile_command_pos = 0;

      // Default compiler gcc
      TextAppend(compile_command, "gcc ", &compile_command_pos);
      
      // -o if output path length != 0
      if (TextLength(compilerWindow.outputText) > 0)
      { 
        TextAppend(compile_command, "-o ", &compile_command_pos);
        TextAppend(compile_command, compilerWindow.outputText, &compile_command_pos);
        TextAppend(compile_command, " ", &compile_command_pos);
      }

      // all files tracked
      for (int i = 0; i < trackedFiles.count; ++i)
      {
        TextAppend(compile_command, trackedFiles.paths[i], &compile_command_pos);
        TextAppend(compile_command, " ", &compile_command_pos);
      }

      // Additional Include directories if any
      if (TextLength(compilerWindow.includeText) > 0)
      {
        // Append first -I
        TextAppend(compile_command, "-I", &compile_command_pos);
        
        // replace ; separator with -I and append all paths
        char *replaced = TextReplace(compilerWindow.includeText, ";", " -I");
        TextAppend(compile_command, replaced, &compile_command_pos);
        RL_FREE(replaced);

        // Trailing space for next part if any
        TextAppend(compile_command, " ", &compile_command_pos);
      }

      // Additional Library directories if any
      if (TextLength(compilerWindow.libraryText) > 0)
      {
        // Append first -L
        TextAppend(compile_command, "-L", &compile_command_pos);
        
        // replace ; separator with -L and append all paths
        char *replaced = TextReplace(compilerWindow.libraryText, ";", " -L");
        TextAppend(compile_command, replaced, &compile_command_pos);
        RL_FREE(replaced);

        // Trailing space for next part if any
        TextAppend(compile_command, " ", &compile_command_pos);
      }

      // Link Libraries if any
      if (TextLength(compilerWindow.linkText) > 0)
      {
        // Append first -l
        TextAppend(compile_command, "-l", &compile_command_pos);
        
        // replace ; separator with -l and append all library names
        char *replaced = TextReplace(compilerWindow.linkText, ";", " -l");
        TextAppend(compile_command, replaced, &compile_command_pos);
        RL_FREE(replaced);

        // Trailing space for next part if any
        TextAppend(compile_command, " ", &compile_command_pos);
      }

      TraceLog(LOG_DEBUG, "Compile command: \"%s\"", compile_command);
      
      CommandRunnerAsync(&compileCommand, compile_command);
    }

    // Command execution state
    // Command is still running
    if (CommandRunnerIs(&compileCommand, CMD_PENDING))
    {
      outputWindow.commandState = CMD_PENDING;
    } // Command finished execution
    else if (CommandRunnerIs(&compileCommand, CMD_DONE))
    {
      // Free previous output message
      if (commandOutputCopy != NULL) free(commandOutputCopy);

      // Copy output message
      commandOutputCopy = strdup(compileCommand.output);
      outputWindow.outputText = commandOutputCopy;

      // Copy exit code
      outputWindow.exitCode = compileCommand.exitCode;
      outputWindow.commandState = CMD_DONE;

      // Reset command runner state
      CommandRunnerReset(&compileCommand);
    } // Unsuccesfuly (Process Spawn Failed)
    else if (CommandRunnerIs(&compileCommand, CMD_FAILED))
    {
      outputWindow.commandState = CMD_FAILED;
      CommandRunnerReset(&compileCommand);
    }

    BeginDrawing();
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    GuiAppMenu(&appMenu);

    WindowEntriesDraw(&windows);
    
    // UI drags debug
    if (debugDrags)
    {
      const Color textColor = GetColor(0x282828FF);
      const Vector2 anchorSize = {12, 12};
      const Vector2 textOffset = {16, 3};
      const char *windowTextFmt = "x: %d y: %d w: %d h: %d";
      const float textSize = 20;
      const float textSpacing = 1;
      const char *windowText = 0;

      Vector2 textPos = {0, 0};

      // System Window
      DrawRectangleRec(appMenu.dragHandle, RED);
      
      // Files Window
      DrawRectangleRec(filesWindow.dragHandle, GREEN);
      DrawRectangleV(filesWindow.anchor->pos, anchorSize, DARKGREEN);
      windowText = TextFormat(windowTextFmt, 
        (int)filesWindow.anchor->pos.x, (int)filesWindow.anchor->pos.y,
        (int)filesWindow.window->rect.width, (int)filesWindow.window->rect.height
      );

      textPos = Vector2Add(filesWindow.anchor->pos, textOffset);
      DrawTextEx(GetFontDefault(), windowText, textPos, textSize, textSpacing, textColor);
      
      // Compiler Window
      DrawRectangleRec(compilerWindow.dragHandle, BLUE);
      DrawRectangleV(compilerWindow.anchor->pos, anchorSize, DARKBLUE);
      windowText = TextFormat(windowTextFmt, 
        (int)compilerWindow.anchor->pos.x, (int)compilerWindow.anchor->pos.y,
        (int)compilerWindow.window->rect.width, (int)compilerWindow.window->rect.height
      );

      textPos = Vector2Add(compilerWindow.anchor->pos, textOffset);
      DrawTextEx(GetFontDefault(), windowText, textPos, textSize, textSpacing, textColor);

      // Output Window
      DrawRectangleRec(outputWindow.dragHandle, BROWN);
      DrawRectangleV(outputWindow.anchor->pos, anchorSize, DARKBROWN);
      windowText = TextFormat(windowTextFmt, 
        (int)outputWindow.anchor->pos.x, (int)outputWindow.anchor->pos.y,
        (int)outputWindow.window->rect.width, (int)outputWindow.window->rect.height
      );

      textPos = Vector2Add(outputWindow.anchor->pos, textOffset);
      DrawTextEx(GetFontDefault(), windowText, textPos, textSize, textSpacing, textColor);
      
      // Client area
      DrawRectangleRec(clientArea, GetColor(0x80600020));
    }

    EndDrawing();
  }

  WindowEntriesFree(&windows);
  FreeFilePathList(&trackedFiles, TRACKED_FILES_CAPACITY);
  UnloadLayout(&outputWindow.layout);
  UnloadLayout(&compilerWindow.layout);
  UnloadLayout(&filesWindow.layout);
  UnloadLayout(&appMenu.layout);

  CloseWindow();
  return 0;
}

FilePathList AllocFilePathList(size_t maxFilePathSize, size_t maxPathCount)
{
  FilePathList list = {0};

  list.count = 0;
  list.paths = RL_MALLOC(maxPathCount * sizeof(char *));
  for (int i=0; i<maxPathCount; ++i)
  {
    list.paths[i] = RL_MALLOC(maxFilePathSize * sizeof(char));
  }

  return list;
}

void FreeFilePathList(FilePathList *list, size_t allocatedPathCount)
{
  for (int i=0; i<allocatedPathCount; ++i)
  {
    RL_FREE(list->paths[i]);
  }

  RL_FREE(list->paths);
}

bool IsFileAlreadyTracked(const FilePathList *trackedFiles, const char *filepath)
{
  if (trackedFiles == NULL || filepath == NULL) return false;

  for (int i = 0; i < trackedFiles->count; ++i)
    if (TextIsEqual(trackedFiles->paths[i], filepath)) 
      return true;

  return false;
}

int TrackFile(FilePathList *trackedFiles, const char *filepath, const char *workingDir)
{
  if (trackedFiles == NULL || filepath == NULL) return 0;
  
  const char *fileToTrack = filepath;

  // If file is from working directory trim it from path
  if (TextFindIndex(filepath, workingDir) > -1) 
  {
    // Trim working directory 
    fileToTrack = TextSubtext(
      filepath, 
      TextLength(workingDir) + 1, TextLength(filepath) - TextLength(workingDir)
    );
  }

  // File is already tracked
  if (IsFileAlreadyTracked(trackedFiles, fileToTrack)) return 0;
  
  // Track new file
  TraceLog(LOG_DEBUG, "Tracked file %s", fileToTrack);
  // Files are gathered from LoadDroppedFiles / LoadDirectoryFiles
  // The content of those strings is managed by Raylib so have to copy them
  TextCopy(trackedFiles->paths[trackedFiles->count++], fileToTrack);

  return 1;
}

int UntrackFile(FilePathList *trackedFiles, const int index)
{
  if (trackedFiles == NULL) return 0;
  if (index < 0 || index >= trackedFiles->count) return 0;

  const char *fileToUntrack = trackedFiles->paths[index];
  if (fileToUntrack == NULL || TextLength(fileToUntrack) <= 0) return 0;

  TraceLog(LOG_DEBUG, "Untracked file %s", fileToUntrack);

  // Save ar reference to removed item
  char *removedElement = trackedFiles->paths[index];

  for (int i = index; i < trackedFiles->count - 1; ++i)
  {
    trackedFiles->paths[i] = trackedFiles->paths[i + 1];
  }
  
  trackedFiles->count -= 1;

  // Move removed element to end of array
  trackedFiles->paths[trackedFiles->count] = removedElement;
  // Clean removed element to be reusable
  trackedFiles->paths[trackedFiles->count][0] = '\0';

  return 1;
}