#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdbool.h>
#include <stdatomic.h>

typedef enum CmdState 
{
  CMD_UNITIALIZED = 0,
  CMD_PENDING,
  CMD_DONE,
  CMD_FAILED,
} CmdState;

typedef struct CmdRunner
{
  atomic_int state;
  int exitCode;
  char *output;

  // Platform related
  void *threadHandle; // Handle for running process thread
  char *cmdString; // Copy of command string for thread lifetime 
} CmdRunner;

// Create a CmdRunner
CmdRunner *CommandRunnerCreate(void);

// Run cmdString asynchronusly
void CommandRunnerAsync(CmdRunner *cmd, const char *cmdString);

// Reset CmdRunner structure for resource free (struct is reusable)
void CommandRunnerReset(CmdRunner *cmd);

// Check if CmdRunner state matches the expected
bool CommandRunnerIs(CmdRunner *cmd, CmdState state);

// Run cmdString asynchronusly, each platform must implement this
void _PlatformCommandRunnerAsync(CmdRunner *cmd);

// Release thread resource, each platform must implement this
void _PlatformReleaseThread(CmdRunner *cmd);

#endif //!PLATFORM_H