#ifndef BPE_H
#define BPE_H

#include <stdio.h>
#include <string.h>
#include "core.h"

typedef struct {
  int id, freq;
  int left, right;
  bool active;
} Token;

typedef struct {
  Token *items;
  size_t count;
  size_t capacity;
} Tokens;

void DebugToks(Tokens toks);
void TokDump(Tokens *toks, size_t id, StringBuf *sb);
size_t PushTokChar(Tokens *toks, unsigned char c);
size_t PushTokMerge(Tokens *toks, int l_id, int r_id);
void FillToksFromSb(Tokens *toks, StringBuf *sb);
void DumpAllToks(Tokens *toks);
bool Step(Tokens *toks);

#endif // BPE_H
