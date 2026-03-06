#ifndef WINDOW_SORTING_H
#define WINDOW_SORTING_H

#include <stdint.h>

#include <raylib.h>

typedef void (*WindowDrawFn)(void *instance);

typedef struct WindowEntry
{
  void *instance;
  WindowDrawFn draw;
} WindowEntry;

#define MakeWindowEntry(instance, draw, drag, move) \
  (WindowEntry) {                                   \
    (void *)((instance)),                           \
    (WindowDrawFn)((draw)),                         \
  }                                                 \

typedef struct WindowEntries
{
  WindowEntry *entries;
  size_t count;
  size_t capacity;
} WindowEntries;

WindowEntries WindowEntriesInit(uint32_t count);
void WindowEntriesFree(WindowEntries *entries);

void WindowEntriesPush(WindowEntries *entries, WindowEntry entry);

void WindowEntriesUpfront(WindowEntries *entries, void *instance);

void WindowEntriesDraw(WindowEntries *entries);

#endif //!WINDOW_SORTING_H