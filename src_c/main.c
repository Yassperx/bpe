#include "bpe.h"

int main() {
  Tokens toks = {0};
  toks.delim[0] = ',', toks.delim[1] = '(', toks.delim[2] = ')', toks.delim[3] = '[', toks.delim[4] = ']'; 
  StringBuf sb = {0};

  ReadEntireFile("assets/medium_data.txt", &sb);
  FillToksFromSb(&toks, &sb);
  
  size_t i = 1;
  while(ToksStep(&toks)){
    if (i % 100 == 0){
      printf("Iterated %zu time\n", i);
    }
    i += 1;
  }
 
  FILE* file = fopen("assets/medium_saved.txt", "w");
  fprintf(file, ",()[]\n");
  for (size_t i = 0; i < toks.count; ++i){
    TokDump(toks.items[i], file, 0);
  }

  ToksFree(&toks);
  VecFree(&sb);
  return 0;
}
