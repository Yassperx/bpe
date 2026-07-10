#include "core.h"

void StringBufNullT(StringBuf *sb){
  if (VecBack(sb) != '\0') VecPush(sb, '\0');
}

void StringBufPushCstr(StringBuf *sb, const char* str){
  while(*str) { VecPush(sb, *str); str++; }
}

void ReadEntireFile(const char* file, StringBuf *sb) {
  FILE *f = fopen(file, "rb");
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);
  VecEnsureCap(sb, fsize);
  fread(sb->items, fsize, 1, f);
  sb->count = fsize;
  fclose(f);
}

StringView SvFromSb(StringBuf sb){
  return (StringView) {
    .view = sb.items, 
    .count = sb.count
  };
}
