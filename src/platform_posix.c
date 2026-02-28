#include <platform.h>

#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <wordexp.h>

void *_PlatformRunnerFunction(void *param);

void _PlatformReleaseThread(CmdRunner *cmd)
{
  // Wait until thread has finished, it mostly wouldve when this is called
  // It releases on end
  pthread_join((pthread_t)cmd->threadHandle, NULL);
  cmd->threadHandle = NULL;
}

void _PlatformCommandRunnerAsync(CmdRunner *cmd)
{
  // Run command in thread
  if (pthread_create(((pthread_t*)&cmd->threadHandle), NULL, _PlatformRunnerFunction, cmd) != 0)
  {
    atomic_store(&cmd->state, CMD_FAILED);
    return;
  }
}

void *_PlatformRunnerFunction(void *param)
{
  CmdRunner *cmd = (CmdRunner *)param;
  
  // Split cmdString, execvp requires argv[]
  wordexp_t cmdStringSplit;
  // WRDE_NOCMD prevents command expansion: $(...)
  if (wordexp(cmd->cmdString, &cmdStringSplit, WRDE_NOCMD) != 0)
  {
    // End thread
    cmd->output = NULL;
    cmd->exitCode = -1;
    atomic_store(&cmd->state, CMD_FAILED);
    return NULL;
  }

  // Pipes: parent -> reads from pipefd[0], child writes to pipefd[1]
  int pipefd[2];

  // Create pipes
  if (pipe(pipefd) == -1)
  { // Failed to create pipes
    // Release resources
    wordfree(&cmdStringSplit);
    // End thread
    cmd->output = NULL;
    cmd->exitCode = -1;
    atomic_store(&cmd->state, CMD_FAILED);
    return NULL;
  }

  // Fork process
  pid_t pid = fork();

  // Failed to fork
  if (pid < 0)
  {
    // Release resources
    close(pipefd[0]);
    close(pipefd[1]);
    wordfree(&cmdStringSplit);
    // End thread
    cmd->output = NULL;
    cmd->exitCode = -1;
    atomic_store(&cmd->state, CMD_FAILED);
    return NULL;
  }

  // Redirect pipes to child process
  if (pid == 0)
  {
    // Redirect stdout to child write
    dup2(pipefd[1], STDOUT_FILENO);
    // Redirect sterr to child write
    dup2(pipefd[1], STDERR_FILENO);

    // Close child read and write pipes
    close(pipefd[0]);
    close(pipefd[1]);

    // Substitute child process program
    execvp(cmdStringSplit.we_wordv[0], cmdStringSplit.we_wordv);

    // Terminate child process if execvp failed
    wordfree(&cmdStringSplit);
    _exit(127);
  }

  // Parent deadlocks if lets write pipe opened
  close(pipefd[1]);

  // Buffer for reading from pipe
  char buffer[4096];
  ssize_t bytesRead = 0;

  // Heap allocated buffer for returned output
  char *output = NULL;
  size_t totalSize = 0;

  // Read into buffer 4096 bytes (sizeof(buffer)) from hRead pipe while process runs
  while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0)
  {
    // Expand output buffer to fit output
    char *newOutput = realloc(output, totalSize + bytesRead + 1);
    if (newOutput == NULL)
    {
      free(output);
      output = NULL;
      break;
    }

    output = newOutput;
    // Append bytes read to end of output buffer
    memcpy(output + totalSize, buffer, bytesRead);
    totalSize += bytesRead;
  }

  // Append NULL terminator
  if (output != NULL) output[totalSize] = '\0';

  // Wait until process (pid) finishes (0 means blocking)
  int exitCode = 0;
  int status;
  if (waitpid(pid, &status, 0) == -1)
  { // Wait failed (invalid process)
    // Release resources
    free(output);
    close(pipefd[0]);
    wordfree(&cmdStringSplit);
    // End thread
    cmd->output = NULL;
    cmd->exitCode = -1;
    atomic_store(&cmd->state, CMD_FAILED);
    return NULL;
  }
  else // Get process exit code
  {
    if (WIFEXITED(status))
      exitCode  = WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
      exitCode = 128 + WTERMSIG(status);
    else exitCode = -1;
  }

  // Release resources
  close(pipefd[0]);
  wordfree(&cmdStringSplit);
  
  // If no output read then give empty string
  if (output == NULL) output = calloc(1, sizeof(char));

  // Fill output structure
  cmd->output = output;
  cmd->exitCode = exitCode;
  atomic_store(&cmd->state, CMD_DONE);
  return NULL;
}