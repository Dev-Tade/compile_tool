#include <stdlib.h>

#include "app_menu.h"
#include "files_window.h"
#include "compiler_window.h"

#include <raylib.h>
#include <raymath.h>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#define TOOL_NAME "Compile Tool"
#define TOOL_VERSION "ver. 0.1"

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

int main()
{
  const char *WINDOW_TITLE = TOOL_NAME" "TOOL_VERSION" - "TOOL_BUILD_TYPE;
  SetTraceLogLevel(LOG_ALL);
  SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST);
  InitWindow(800, 650, WINDOW_TITLE);
  SetTargetFPS(60);
  
  GuiLoadStyle("styles/style_dark.rgs");
  GuiSetStyle(DEFAULT, TEXT_SIZE, 18);

  GuiAppMenuState app_menu = InitGuiAppMenu();
  app_menu.WindowTitle = WINDOW_TITLE;
  GuiFilesWindowState files_window = InitGuiFilesWindow();
  GuiCompilerWindowState compiler_window = InitGuiCompilerWindow();

  bool output_window = true;

  Rectangle drag_handle = app_menu.layoutRecs[0];
  drag_handle.height = 24; // RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT
  bool dragging_window = false;
  Vector2 window_pos = Vector2Zero();
  Vector2 pan_offset = Vector2Zero();

  files_window.files_text = RL_CALLOC(4096, sizeof(char));
  files_window.files_text_pos = 0;

  const int TRACKED_FILES_CAPACITY = 256;
  const int TRACKED_FILE_PATH_SIZE = 256; 
  FilePathList trackedFiles = AllocFilePathList(TRACKED_FILE_PATH_SIZE, TRACKED_FILES_CAPACITY);
  int newlyTrackedFiles = 0;

  char compile_command[2048] = {0};
  int compile_command_pos = 0;

  char command_output[4096] = {0};

  Rectangle output_window_rect = {0, 512, 800, 250};
  Rectangle output_panel_rect = output_window_rect;
  output_panel_rect.y += 24; // RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT
  Rectangle output_panel_content_rect = output_panel_rect;
  output_panel_content_rect.height = 400;
  Vector2 output_panel_scroll = {1, 0};
  Rectangle output_panel_view = {0};


  while (app_menu.MenuWindowActive && !WindowShouldClose())
  {
    Vector2 mouse_pos = GetMousePosition();
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !dragging_window)
    {
      if (CheckCollisionPointRec(mouse_pos, drag_handle))
      {
        window_pos = GetWindowPosition();
        dragging_window = true;
        pan_offset = mouse_pos;
      }
    }

    if (dragging_window)
    { 
      window_pos = Vector2Add(window_pos, Vector2Subtract(mouse_pos, pan_offset));
      SetWindowPosition((int)window_pos.x, (int)window_pos.y);
      if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) dragging_window = false;
    }

    if (app_menu.FilesButtonPressed) files_window.FilesWindowActive = 1;
    if (app_menu.CompilerButtonPressed) compiler_window.CompilerWindowActive = 1;
    if (app_menu.OutputButtonPressed) output_window = true;

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

    if (files_window.UntrackFileButtonPressed)
    {
      int index = files_window.FilesListActive;
      newlyTrackedFiles -= UntrackFile(&trackedFiles, index);
      files_window.FilesListActive = index;
    }

    if (newlyTrackedFiles != 0)
    {
      TraceLog(LOG_DEBUG, "Updating tracked files!");
      if (newlyTrackedFiles < 0) TraceLog(LOG_DEBUG, "%d files untracked", abs(newlyTrackedFiles));
      else if (newlyTrackedFiles > 0) TraceLog(LOG_DEBUG, "%d new files tracked", newlyTrackedFiles);

      char *joinedFiles = TextJoin(trackedFiles.paths, trackedFiles.count, ";");
      TextCopy(files_window.files_text, joinedFiles);
      
      newlyTrackedFiles = 0;
    }

    if (compiler_window.CompileButtonPressed)
    {
      TraceLog(LOG_INFO, "Compile!");
      compile_command_pos = 0;

      // Default compiler gcc
      TextAppend(compile_command, "gcc ", &compile_command_pos);
      
      // -o if output path length != 0
      if (TextLength(compiler_window.OutputPathTextboxText) > 0)
      { 
        TextAppend(compile_command, "-o ", &compile_command_pos);
        TextAppend(compile_command, compiler_window.OutputPathTextboxText, &compile_command_pos);
        TextAppend(compile_command, " ", &compile_command_pos);
      }

      // all files tracked
      for (int i = 0; i < trackedFiles.count; ++i)
      {
        TextAppend(compile_command, trackedFiles.paths[i], &compile_command_pos);
        TextAppend(compile_command, " ", &compile_command_pos);
      }

      // Additional Include directories if any
      if (TextLength(compiler_window.AdditionalIncludesTextboxText) > 0)
      {
        // Append first -I
        TextAppend(compile_command, "-I", &compile_command_pos);
        
        // replace ; separator with -I and append all paths
        char *replaced = TextReplace(compiler_window.AdditionalIncludesTextboxText, ";", " -I");
        TextAppend(compile_command, replaced, &compile_command_pos);
        RL_FREE(replaced);

        // Trailing space for next part if any
        TextAppend(compile_command, " ", &compile_command_pos);
      }

      // Additional Library directories if any
      if (TextLength(compiler_window.AdditionalLibrariesTextboxText) > 0)
      {
        // Append first -L
        TextAppend(compile_command, "-L", &compile_command_pos);
        
        // replace ; separator with -L and append all paths
        char *replaced = TextReplace(compiler_window.AdditionalLibrariesTextboxText, ";", " -L");
        TextAppend(compile_command, replaced, &compile_command_pos);
        RL_FREE(replaced);

        // Trailing space for next part if any
        TextAppend(compile_command, " ", &compile_command_pos);
      }

      // Link Libraries if any
      if (TextLength(compiler_window.LinkLibrariesTextboxText) > 0)
      {
        // Append first -l
        TextAppend(compile_command, "-l", &compile_command_pos);
        
        // replace ; separator with -l and append all library names
        char *replaced = TextReplace(compiler_window.LinkLibrariesTextboxText, ";", " -l");
        TextAppend(compile_command, replaced, &compile_command_pos);
        RL_FREE(replaced);

        // Trailing space for next part if any
        TextAppend(compile_command, " ", &compile_command_pos);
      }

      TraceLog(LOG_DEBUG, "Compile command: \"%s\"", compile_command);
      
      // Redirect stderr to stdout for the process
      TextAppend(compile_command, " 2>&1", &compile_command_pos);
      FILE *pipe = popen(compile_command, "r");
      if (pipe == NULL) 
      {
        TraceLog(LOG_DEBUG, "FAILED TO EXECUTE COMMAND");
        TextCopy(command_output, "FAILED TO EXECUTE COMMAND!");
        continue;
      }

      fread(command_output, 1, 4096, pipe);
      pclose(pipe);
    }

    BeginDrawing();
    ClearBackground(BLACK);

    GuiAppMenu(&app_menu);
    GuiFilesWindow(&files_window);
    GuiCompilerWindow(&compiler_window);

    if (output_window)
    {
      output_window = !GuiWindowBox(output_window_rect, "Output");
    
      GuiScrollPanel(output_panel_rect, NULL, output_panel_content_rect, &output_panel_scroll, &output_panel_view);

      BeginScissorMode(output_panel_view.x, output_panel_view.y, output_panel_view.width, output_panel_view.height);

      Rectangle text_rect = {output_panel_rect.x + output_panel_scroll.x, output_panel_rect.y + output_panel_scroll.y, output_panel_content_rect.width, output_panel_content_rect.height};

      int before = GuiGetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL);
      int before2 = GuiGetStyle(DEFAULT, TEXT_WRAP_MODE);
      GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_TOP);
      GuiSetStyle(DEFAULT, TEXT_WRAP_MODE, TEXT_WRAP_CHAR);
      GuiDrawText(command_output, text_rect, TEXT_ALIGN_LEFT, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));
      GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, before);
      GuiSetStyle(DEFAULT, TEXT_WRAP_MODE, before2);

      EndScissorMode();
    }

    EndDrawing();
  }

  FreeFilePathList(&trackedFiles, TRACKED_FILES_CAPACITY);

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
  if (index >= trackedFiles->count) return 0;

  const char *fileToUntrack = trackedFiles->paths[index];
  if (fileToUntrack == NULL || TextLength(fileToUntrack) <= 0) return 0;

  TraceLog(LOG_DEBUG, "Untracked file %s", fileToUntrack);

  for (int i = index; i < trackedFiles->count - index; ++i)
  {
    trackedFiles->paths[i] = trackedFiles->paths[i + 1];
  }

  trackedFiles->count -= 1;
  return 1;
}