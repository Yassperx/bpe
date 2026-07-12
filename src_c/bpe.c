#include "bpe.h"

Token* TokChar(char c, size_t id){
  Token *tok = malloc(sizeof(Token));
  tok->id = id;
  tok->left.c = c;
  tok->right = NULL;
  return tok;
}

Token* TokMerge(Token* l, Token *r, size_t id){
  Token *tok = malloc(sizeof(Token));
  tok->id = id;
  tok->left.ptr = l;
  tok->right = r;
  return tok;
}

Token *TokLoad(char delim[5], StringView sv){
  if (!SvSkip(&sv, delim[3])){
    fprintf(stderr, "ERROR: token must present his id \'"Sv_Fmt"\'\n", Sv_Arg(sv));
    return NULL;
  }
  StringView num = SvChopDelim(&sv, delim[4]);
  StringBuf sb = SbFromSv(num);
  StringBufNullT(&sb);
  size_t id = atoi(sb.items);
  Token *tok = NULL;
  if (sv.view[0] == delim[1]) {
    SvChopLeft(&sv, 1);
    SvChopRight(&sv, 1);
    size_t i = 0, depth = 0;
    while(!(sv.view[i] == delim[0] && depth == 0)){
      if (sv.view[i] == delim[1]) depth++;
      if (sv.view[i] == delim[2]) assert(depth > 0), depth--;
      i++;
    }
    StringView left = sv, right = sv;
    SvChopLeft(&left, i + 1);
    SvChopRight(&right, sv.count - i);
    Token *l = TokLoad(delim, left);
    Token *r = TokLoad(delim, right);
    tok = TokMerge(l, r, current_tok_id++);
  } else {
    tok = TokChar(sv.view[0], current_tok_id++);
  }
  VecFree(&sb);
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
  for (size_t i = 0; i < toks->count; ++i){
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

void ToksFromSb(Tokens *toks, StringBuf *sb){
  for (size_t i = 0; i < sb->count; ++i){
    if (sb->items[i] == '\n') sb->items[i] = ' ';
    VecPush(toks, TokChar(sb->items[i], current_tok_id++));
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
  StringBuf *sbs = calloc(sbs_count, sizeof(StringBuf));
  Pairs pairs = {0};
  VecReserve(&pairs, sbs_count);

  for (size_t i = 0; i + 1 < toks->count; ++i){
    Token *tok_pair = TokMerge(toks->items[i], toks->items[i+1], 0);
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
  for (size_t i = 0; i < 5; ++i) new_toks.delim[i] = toks->delim[i];
  VecReserve(&new_toks, toks->capacity);

  size_t i = 0;
  while (i < toks->count){
    if (i == toks->count - 1) {
      VecPush(&new_toks, toks->items[i]); 
      i += 1;
    } else {
      StringBuf tmp = {0};
      Token *tok_pair = TokMerge(toks->items[i], toks->items[i + 1], 0);
      TokStr(tok_pair, &tmp);
      StringBufNullT(&tmp);
      if (strcmp(best.str, tmp.items) == 0){
        tok_pair->id = current_tok_id++;
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
  free(sbs);
  return true;
}

void ToksSave(Tokens* toks, const char* path) {
  FILE* file = fopen(path, "w");
  if (!file) {
    fprintf(stderr, "Failed to open %s for writing\n", path);
    return;
  }
  fprintf(file, "%.*s\n", 5, toks->delim);
  for (size_t i = 0; i < toks->count; ++i) TokDump(toks->items[i], file, 0);
  fclose(file);
}

void ToksLoad(Tokens *toks, const char* path){
  current_tok_id = 0;
  StringBuf sb = {0};
  ReadEntireFile(path, &sb);
  StringView content = SvFromSb(sb);
  StringView line = {0};
  bool first = true;
  while(true){
    line = SvChopDelim(&content, '\n');
    if (!line.count) break;
    if (first){
      first = false;
      for (size_t i = 0; i < 5; ++i) toks->delim[i] = line.view[i];
    } else {
      Token *tok = TokLoad(toks->delim, line);
      if (!tok) continue;
      VecPush(toks, tok);
    }
  }
  VecFree(&sb);
}
