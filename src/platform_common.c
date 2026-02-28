#include <platform.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

CmdRunner *CommandRunnerCreate(void)
{
  return (CmdRunner *)calloc(1, sizeof(CmdRunner));
}

void CommandRunnerAsync(CmdRunner *cmd, const char *cmdString)
{
  // Invalid pointer
  assert(cmd != NULL && "NULL pointer to CmdRunner struct");

  // Invalid state, command must not be initialized
  if (!CommandRunnerIs(cmd, CMD_UNITIALIZED)) return;

  cmd->output = NULL;
  cmd->exitCode = 0;
  atomic_store(&cmd->state, CMD_PENDING);

  // Duplicate cmdString for thread lifetime
  cmd->cmdString = strdup(cmdString);

  // Run in separate thread
  _PlatformCommandRunnerAsync(cmd);
}

void CommandRunnerReset(CmdRunner *cmd)
{
  // Invalid pointer
  assert(cmd != NULL && "NULL pointer to CmdRunner struct");

  // Release when command execution is done or failed
  if (CommandRunnerIs(cmd, CMD_DONE) || CommandRunnerIs(cmd, CMD_FAILED))
  {
    _PlatformReleaseThread(cmd);
    cmd->threadHandle = NULL;
    free(cmd->cmdString);
    cmd->cmdString = NULL;
    free(cmd->output);
    cmd->output = NULL;
    cmd->exitCode = 0;
    atomic_store(&cmd->state, CMD_UNITIALIZED);
  }
}

bool CommandRunnerIs(CmdRunner *cmd, CmdState state)
{
  // Invalid pointer
  assert(cmd != NULL && "NULL pointer to CmdRunner struct");

  return (atomic_load(&cmd->state) == state);
}