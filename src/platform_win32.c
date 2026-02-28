#include <platform.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

DWORD WINAPI _PlatformRunnerFunction(void *param);

void _PlatformReleaseThread(CmdRunner *cmd)
{
  // Wait until thread has finished, it mostly wouldve when this is called
  WaitForSingleObject(cmd->threadHandle, INFINITE);
  // Release handle
  CloseHandle(cmd->threadHandle);
  cmd->threadHandle = NULL;
}

void _PlatformCommandRunnerAsync(CmdRunner *cmd)
{
  // Run command in thread
  cmd->threadHandle = CreateThread(NULL, 0, _PlatformRunnerFunction, cmd, 0, NULL);
  if (cmd->threadHandle == NULL)
  {
    atomic_store(&cmd->state, CMD_FAILED);
    return;
  }
}

DWORD WINAPI _PlatformRunnerFunction(void *param)
{
  CmdRunner *cmd = (CmdRunner *)param;
  
  // Copy cmdString, CreateProcess requires it to be modifiable
  char *cmdStringCopy = strdup(cmd->cmdString);
  if (cmdStringCopy == NULL)
  {
    // End thread
    cmd->output = NULL;
    cmd->exitCode = -1;
    atomic_store(&cmd->state, CMD_FAILED);
    return -1;
  }

  // Pipe handles: parent -> reads from hRead, child writes to hWrite
  HANDLE hWrite = NULL;
  HANDLE hRead = NULL;

  // Pipe security
  SECURITY_ATTRIBUTES sa = {0};
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = TRUE; // Child can inherit parent handles
  sa.lpSecurityDescriptor = NULL;

  // Create pipes
  if (!CreatePipe(&hRead, &hWrite, &sa, 0))
  { // Failed to create pipes
    // Release resources
    free(cmdStringCopy);
    // End thread
    cmd->output = NULL;
    cmd->exitCode = -1;
    atomic_store(&cmd->state, CMD_FAILED);
    return -1;
  }

  // Invalidate read handle for child
  SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);

  // Process startup info
  STARTUPINFOA si = {0};
  si.cb = sizeof(STARTUPINFOA);
  si.dwFlags = STARTF_USESTDHANDLES;

  // Redirect output streams
  si.hStdOutput = hWrite;
  si.hStdError  = hWrite;
  
  // Default stdin
  si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

  // Process information
  PROCESS_INFORMATION pi = {0};

  if (!CreateProcessA(
    NULL, cmdStringCopy,    // Application name, Command line string 
    NULL, NULL,             // Process security, Thread security
    TRUE, 0,                // Inherit parent handles, Creation flags
    NULL, NULL,             // Environment variables, Startup Directory
    &si, &pi))              // Startup info, Process info 
  { // Failed to create process
    // Release resources
    CloseHandle(hWrite);
    CloseHandle(hRead);
    free(cmdStringCopy);
    // End thread
    cmd->output = NULL;
    cmd->exitCode = -1;
    atomic_store(&cmd->state, CMD_FAILED);
    return -1;
  }

  // Parent deadlocks if lets write pipe opened
  CloseHandle(hWrite);

  // Buffer for reading from pipe
  char buffer[4096];
  DWORD bytesRead = 0;

  // Heap allocated buffer for returned output
  char *output = NULL;
  size_t totalSize = 0;

  // Read into buffer 4096 bytes (sizeof(buffer)) from hRead pipe while process runs
  while (ReadFile(hRead, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0)
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

  // Wait until process finishes
  WaitForSingleObject(pi.hProcess, INFINITE);

  // Get process exit code
  DWORD exitCode = 0;
  GetExitCodeProcess(pi.hProcess, &exitCode);

  // Release resources
  CloseHandle(hRead);
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  free(cmdStringCopy);

  // If no output read then give empty string
  if (output == NULL) output = calloc(1, sizeof(char));

  // Fill output structure
  cmd->output = output;
  cmd->exitCode = exitCode;
  atomic_store(&cmd->state, CMD_DONE);
  return 0;
}