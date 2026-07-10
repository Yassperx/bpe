#include "bpe.h"

int main() {
  Tokens toks = {0};
  StringBuf sb = {0};
  ReadEntireFile("assets/small_data.txt", &sb);
  FillToksFromSb(&toks, &sb);
  Step(&toks);
  VecFree(&sb);
  VecFree(&toks);
  return 0;
}
