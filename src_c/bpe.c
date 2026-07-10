#include "bpe.h"

Token* TokChar(char c){
  Token *tok = malloc(sizeof(Token));
  tok->id = current_tok_id++;
  tok->left.c = c;
  tok->right = NULL;
  return tok;
}

Token* TokMerge(Token* l, Token *r){
  Token *tok = malloc(sizeof(Token));
  tok->id = current_tok_id++;
  tok->left.ptr = l;
  tok->right = r;
  return tok;
}

void TokFree(Token *tok){
  if (tok->right != NULL){
    TokFree(tok->left.ptr);
    TokFree(tok->right);
  }
  free(tok);
}

void ToksFree(Tokens *toks){
  for (size_t i = 0; i < toks->count; ++i) TokFree(toks->items[i]);
  free(toks->items);
  toks->count = 0;
  toks->capacity = 0;
}

void ToksDebug(Tokens *toks){
  for (size_t i = 0; i < toks->count; ++i) {
    StringBuf sb = {0};
    TokStr(toks->items[i], &sb);
    printf("%c%.*s%c", toks->delim[1], (int)sb.count, sb.items, toks->delim[2]);
    VecFree(&sb);
  }
  printf("\n");
}

void TokStr(Token *tok, StringBuf *sb){
  if (tok->right == NULL){
    VecPush(sb, tok->left.c);
  } else {
    TokStr(tok->left.ptr, sb);
    TokStr(tok->right, sb);
  }
}

void TokDump(Token *tok, FILE *file, size_t level){
  fprintf(file, "[%zu]", tok->id);
  if (tok->right == NULL){
    fprintf(file, "%c", (char)tok->left.c);
  } else {
    fprintf(file, "(");
    TokDump(tok->left.ptr, file, level + 1);
    fprintf(file, ",");
    TokDump(tok->right, file, level + 1);
    fprintf(file, ")");
  }
  if (level == 0) fprintf(file, "\n");
}

void FillToksFromSb(Tokens *toks, StringBuf *sb){
  for (size_t i = 0; i < sb->count; ++i){
    if (sb->items[i] == '\n') continue;
    VecPush(toks, TokChar(sb->items[i]));
  }
}

typedef struct {
  const char* str;
  size_t tok_id, freq;
} Pair;

typedef struct {
  Pair *items;
  size_t count;
  size_t capacity;
} Pairs;

int PairComp(const void* x, const void* y){
  const Pair *a = x;
  const Pair *b = y;
  return strcmp(a->str, b->str);
}

bool ToksStep(Tokens *toks){
  if (toks->count <= 1) return false;

  size_t sbs_count = toks->count - 1; 
  StringBuf sbs[sbs_count] = {};
  Pairs pairs = {0};
  VecEnsureCap(&pairs, sbs_count);

  for (size_t i = 0; i + 1 < toks->count; ++i){
    Token *tok_pair = TokMerge(toks->items[i], toks->items[i+1]);
    TokStr(tok_pair, &sbs[i]);
    StringBufNullT(&sbs[i]);
    Pair pair = { .str = sbs[i].items, .tok_id = i, .freq = 1 };
    VecPush(&pairs, pair);
    free(tok_pair);
  }

  qsort(pairs.items, pairs.count, sizeof(Pair), PairComp);

  Pair last = VecFront(&pairs);
  size_t last_idx = 0;
  for (size_t i = 1; i < pairs.count; ++i){
    Pair cur = pairs.items[i];
    if (strcmp(cur.str, last.str) == 0){
      cur.freq = last.freq + 1; 
    } else {
      for (size_t j = last_idx; j < i; ++j){
        pairs.items[j].freq = last.freq;
      }
      last_idx = i;
    }
    last = cur;
  }
  
  Pair best = VecFront(&pairs);
  for (size_t i = 0; i < pairs.count; ++i){
    Pair cur = pairs.items[i];
    if (cur.freq > best.freq) best = cur;
  }

  if (best.freq == 1) {
    VecFree(&pairs);
    for (size_t i = 0; i < sbs_count; ++i) VecFree(&sbs[i]);
    return false;
  }

  Tokens new_toks = {0};
  VecEnsureCap(&new_toks, toks->capacity);

  size_t i = 0;
  while (i < toks->count){
    if (i == toks->count - 1) {
      VecPush(&new_toks, toks->items[i]); 
      i += 1;
    } else {
      StringBuf tmp = {0};
      Token *tok_pair = TokMerge(toks->items[i], toks->items[i + 1]);
      TokStr(tok_pair, &tmp);
      StringBufNullT(&tmp);
      if (strcmp(best.str, tmp.items) == 0){
        VecPush(&new_toks, tok_pair); 
        i += 2;
      } else {
        VecPush(&new_toks, toks->items[i]); 
        free(tok_pair);
        i += 1;
      }
      VecFree(&tmp);
    }
  }
  Swap(Tokens, &new_toks, toks);
  VecFree(&new_toks);
  VecFree(&pairs);
  for (size_t i = 0; i < sbs_count; ++i) VecFree(&sbs[i]);
  return true;
}
