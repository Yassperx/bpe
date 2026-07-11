#ifndef BPE_H
#define BPE_H

#include "core.h"
#include <string.h>

static size_t current_tok_id = 0;

typedef struct Token {
  size_t id;
  union {
    char c;
    struct Token *ptr;
  } left;
  struct Token *right;
} Token;

typedef struct {
  Token **items;
  size_t count;
  size_t capacity;
  char delim[5];
} Tokens;

Token* TokChar(char c, size_t id);
Token* TokMerge(Token* l, Token *r, size_t id);
Token* TokLoad(char delim[5], StringView sv);

void TokFree(Token *tok);
void TokStr(Token *tok, StringBuf *sb);
void TokDump(Token *tok, FILE *file, size_t level);

void ToksFree(Tokens *toks);
void ToksDebug(Tokens *toks);
void ToksFromSb(Tokens *toks, StringBuf *sb);
bool ToksStep(Tokens *toks);

void ToksSave(Tokens* toks, const char* path);
void ToksLoad(Tokens* toks, const char* path);

#endif // BPE_H
