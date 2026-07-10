#include "bpe.h"

int main() {
  Tokens toks = {0};
  toks.delim[0] = ',', toks.delim[1] = '(', toks.delim[2] = ')', toks.delim[3] = '[', toks.delim[4] = ']'; 
  StringBuf sb = {0};
  ReadEntireFile("assets/small_data.txt", &sb);
  FillToksFromSb(&toks, &sb);
  ToksDebug(&toks);
  VecFree(&sb);
  ToksFree(&toks);
  return 0;
}
