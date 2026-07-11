#include "bpe.h"

int main(int argc, char *argv[]) {
  if (argc < 3){
    printf("Usage:\n");
    printf("./%s save <data_file> <save_file>\n", argv[0]);
    printf("./%s load <data_file> <save_file>\n", argv[0]);
    return 1;
  }
  
  const char* opt = argv[1];
  const char* data_file = argv[2];
  const char* save_file = argv[3];

  Tokens toks = {0};
  toks.delim[0] = ',', toks.delim[1] = '(', toks.delim[2] = ')', toks.delim[3] = '[', toks.delim[4] = ']'; 
  StringBuf sb = {0};
  
  if (strcmp(opt, "save") == 0){
    ReadEntireFile(data_file, &sb);
    ToksFromSb(&toks, &sb);
  } else if (strcmp(opt, "load") == 0){
    ToksLoad(&toks, data_file);
  } else {
    printf("nuh\n");
    return 1;
  }

  size_t i = 1;
  while(ToksStep(&toks)){
    printf("Iterated %zu time\n", i);
    if (i % 1000 == 0) {
      char path[256];
      snprintf(path, sizeof(path), "%s%dk.txt", save_file, i / 1000);
      ToksSave(&toks, path);
      printf("Saved checkpoint %zu at iteration %zu\n", i / 1000, i);
    }
    i += 1;
  }
 
  ToksSave(&toks, save_file);
  ToksFree(&toks);
  VecFree(&sb);
  return 0;
}
