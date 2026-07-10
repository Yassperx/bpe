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

void FillToksFromSb(Tokens *toks, StringBuf *sb){
  for (size_t i = 0; i < sb->count; ++i){
    VecPush(toks, TokChar(sb->items[i]));
  }
}
