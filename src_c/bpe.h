#ifndef BPE_H
#define BPE_H

#include "core.h"

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

Token* TokChar(char c);
Token* TokMerge(Token* l, Token *r);
void TokFree(Token *tok);
void ToksFree(Tokens *toks);
void ToksDebug(Tokens *toks);
void TokStr(Token *tok, StringBuf *sb);
void FillToksFromSb(Tokens *toks, StringBuf *sb);

#endif // BPE_H
