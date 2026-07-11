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
  VecReserve(sb, fsize);
  int result = fread(sb->items, fsize, 1, f);
  sb->count = fsize;
  fclose(f);
}

StringView SvFromSb(StringBuf sb){
  return (StringView) {
    .view = sb.items, 
    .count = sb.count
  };
}

StringBuf SbFromSv(StringView sv){
  StringBuf sb = {0};
  VecReserve(&sb, sv.count);
  for (size_t i = 0; i < sv.count; ++i) VecPush(&sb, sv.view[i]);
  return sb;
}

void SvChopLeft(StringView *sv, size_t n){
  while(n--){
    if (sv->count > 0){
      sv->view++;
      sv->count--;
    }
  }
}

void SvChopRight(StringView *sv, size_t n){
  while(n--){
    if (sv->count > 0){
      sv->count--;
    }
  }
}

bool SvSkip(StringView *sv, char c){
  if (sv->count == 0) return false;
  bool ok = sv->view[0] == c;
  if (ok) SvChopLeft(sv, 1);
  return ok;
}

StringView SvChopDelim(StringView *sv, char delim){
  StringView res = *sv;
  size_t count = sv->count;
  size_t i = 0;
  while(i < count && sv->view[i] != delim) i++;
  SvChopLeft(sv, i+1);
  SvChopRight(&res, count - i);
  return res;
}
