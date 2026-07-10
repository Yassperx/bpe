#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define VecInit(vec) do { \
    (vec)->items = NULL;  \
    (vec)->count = 0;     \
    (vec)->capacity = 0;  \
  } while(0)

#define VecEnsureCap(vec, cap) do {                                                   \
    if (cap >= (vec)->capacity) {                                                     \
      if (!(vec)->capacity) (vec)->capacity++, (vec)->count = 0;                      \
      while((vec)->capacity < cap) (vec)->capacity *= 2;                              \
      (vec)->items = realloc((vec)->items, sizeof(*(vec)->items) * (vec)->capacity);  \
    }                                                                                 \
  } while(0)

#define VecPush(vec, x) do {             \
    VecEnsureCap(vec, (vec)->count + 1); \
    (vec)->items[(vec)->count++] = x;    \
  } while(0)

#define VecPop(vec) (assert((vec)->count && "Empty vec"), (vec)->count--)

#define VecFree(vec) do { \
    free((vec)->items);   \
    (vec)->capacity = 0;  \
    (vec)->count = 0;     \
  } while(0)

#define VecBack(vec) (vec)->items[assert((vec)->count && "EmptyVec"), (vec)->count-1]
  
#define VecFront(vec) (vec)->items[assert((vec)->count && "EmptyVec"), 0]

#define Swap(Type, a, b) do { \
    Type tmp = *(a);          \
    *(a) = *(b);              \
    *(b) = tmp;               \
  } while(0)

#define Todo(msg) do {                                                          \
    fprintf(stderr, "%s:%d: not implemented yet: %s\n", __FILE__, __LINE__, msg); \
    abort();                                                                    \
  } while(0)

#define Unreachable(msg) do {                                                  \
    fprintf(stderr, "%s:%d: how did i get here? %s\n", __FILE__, __LINE__, msg); \
    abort();                                                                   \
  } while(0)

#define Here(msg) fprintf(stderr, "%s:%d: HERE: %s\n", __FILE__, __LINE__, msg)

typedef struct {
  char *items;
  size_t count;
  size_t capacity;
} StringBuf;

void StringBufNullT(StringBuf *sb);
void StringBufPushCstr(StringBuf *sb, const char* str);
void ReadEntireFile(const char* file, StringBuf *sb);

typedef struct {
  const char* view;
  size_t count;
} StringView;

#ifndef Sv_Fmt
#define Sv_Fmt "%.*s"
#endif

#ifndef Sv_Arg
#define Sv_Arg(sv) (int)(sv).count, (sv).view
#endif

StringView SvFromSb(StringBuf sb);

#endif // CORE_H
