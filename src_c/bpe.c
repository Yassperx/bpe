#include "bpe.h"

void ToksPop(Tokens *toks){
  toks->items[VecBack(toks).left].active = true;
  toks->items[VecBack(toks).right].active = true;
  VecPop(toks);
}

void DebugToks(Tokens toks){
  fprintf(stderr, "tok.count = %zu, tok.capacity = %zu\n", toks.count, toks.capacity);
  for (size_t i = 0; i < toks.count; ++i){
    fprintf(stderr, " - { id = %d, left = %d, right = %d, freq = %d, active = %s }\n",
      toks.items[i].id, toks.items[i].left, toks.items[i].right, toks.items[i].freq,
      toks.items[i].active ? "true" : "false"
    );
  }
}

void TokDump(Tokens *toks, size_t id, StringBuf *sb) {
  if (toks->items[id].right == -1) {
    VecPush(sb, (char)toks->items[id].left);
  } else {
    TokDump(toks, toks->items[id].left, sb);
    TokDump(toks, toks->items[id].right, sb);
  }
}

size_t PushTokChar(Tokens *toks, unsigned char c) { 
  size_t id = toks->count;
  Token tok = { .id = id, .left = c, .right = -1, .freq = 0, .active = true };
  VecPush(toks, tok);
  return id;
}

size_t PushTokMerge(Tokens *toks, int l_id, int r_id) {
  size_t id = toks->count;
  toks->items[l_id].active = false;
  toks->items[r_id].active = false;
  Token tok = { .id = id, .left = l_id, .right = r_id, .freq = 0, .active = true };
  VecPush(toks, tok);
  return id;
}

void FillToksFromSb(Tokens *toks, StringBuf *sb) {
  for (size_t ch = 0; ch < sb->count; ++ch) {
    PushTokChar(toks, sb->items[ch]);
  }
}

void DumpAllToks(Tokens *toks) {
  for (size_t i = 0; i < toks->count; ++i) {
    if (!toks->items[i].active) continue;
    StringBuf sb = {0};
    TokDump(toks, i, &sb);
    printf("%s", sb.items);
    VecFree(&sb);
  }
  printf("\n");
}

typedef struct {
  StringView sv; 
  size_t id, freq;
} Pair;

int PairComp(const void* x, const void* y){
  const Pair *a = x;
  const Pair *b = y;
  return strcmp(a->sv.view, b->sv.view);
}

typedef struct {
  Pair *items;
  size_t count;
  size_t capacity;
} Pairs;

bool Step(Tokens *toks){
  StringBuf sb[toks->count];
  for (size_t i = 0; i < toks->count; ++i) VecInit(&sb[i]);

  Pairs pairs = {0};

  for (size_t i = 0; i + 1 < toks->count; ++i) {
    if (!toks->items[i].active) continue;
    PushTokMerge(toks, i, i+1);
    TokDump(toks, toks->count - 1, &sb[i]);
    StringBufNullT(&sb[i]);
    StringView sv = SvFromSb(sb[i]);
    Pair pair = { .sv = sv, .id = i, .freq = 1 };
    ToksPop(toks);
    VecPush(&pairs, pair);
  }

  qsort(pairs.items, pairs.count, sizeof(Pair), PairComp);

  Pair last = VecFront(&pairs);
  size_t last_index = 0;
  for (size_t i = 1; i < pairs.count; ++i) {
    Pair *current = &pairs.items[i];
    if (PairComp((void*)current, (void*)&last) == 0){
      last.freq++;
    } else {
      for (size_t j = last_index; j < i; ++j) pairs.items[j].freq = last.freq;
      last_index = i;
      last = *current; 
    }
  }

  Pair best_pair = VecFront(&pairs);
  for (size_t i = 1; i < pairs.count; ++i) {
    if (pairs.items[i].freq > best_pair.freq) best_pair = pairs.items[i];
  }

  printf("Most frequent pair: \'"Sv_Fmt"\'\n", Sv_Arg(best_pair.sv));

  VecFree(&pairs);
  for (size_t i = 0; i < toks->count; ++i) VecFree(&sb[i]);
}
