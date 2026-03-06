#include "window_sorting.h"

#include <assert.h>
#include <stdlib.h>

void WindowEntriesGrow(WindowEntries *entries);

WindowEntries WindowEntriesInit(uint32_t count)
{
  WindowEntries entries = {0};
  entries.capacity = count;
  entries.entries = RL_CALLOC(entries.capacity, sizeof(WindowEntry));
  assert(entries.entries != NULL && "Init failed");

  return entries;
}

void WindowEntriesFree(WindowEntries *entries)
{
  RL_FREE(entries->entries);
  entries->capacity = 0;
  entries->count = 0;
}

void WindowEntriesPush(WindowEntries *entries, WindowEntry entry)
{
  if ((entries->count + 1) > entries->capacity)
  {
    WindowEntriesGrow(entries);
  }

  entries->entries[entries->count++] = entry;
}

void WindowEntriesUpfront(WindowEntries *entries, void *instance)
{
  if (instance == NULL) return;

  // Find window with given instance
  uint32_t index = entries->count;
  for (size_t i=0; i<entries->count; ++i)
  {
    if (entries->entries[i].instance == instance)
    {
      index = i;
      break;
    }
  }

  // Index is not valid
  // It's already the front window
  if (index == 0) return;
  // No instance bound
  assert(index != entries->count && "No entry bound to instance");

  // Move desired entry to front
  // Copy desired entry
  WindowEntry temp = entries->entries[index];
  // Move back other entries
  for (size_t i = index; i > 0; i--)
  {
    entries->entries[i] = entries->entries[i - 1];
  }
  // Write copy back
  entries->entries[0] = temp;
}

void WindowEntriesGrow(WindowEntries *entries)
{
  entries->capacity *= 1.5f;
  void *ptr = RL_REALLOC(entries->entries, sizeof(WindowEntries) * entries->capacity);
  assert(ptr != NULL && "GROW FAILED");
  entries->entries = ptr;
}

void WindowEntriesDraw(WindowEntries *entries)
{
  for (int32_t i = entries->count - 1; i > -1; --i)
  {
    WindowEntry *entry = &entries->entries[i];

    if (entry->draw == NULL)
    {
      TraceLog(LOG_WARNING, "Window %p has no draw function bound", entry);
      continue;
    } else entry->draw(entry->instance);
  }
}