#include "stdafx.h"
#include <cstdio>
#include "tqdm/tqdm.h"
#include "tqdm/utils.h"

int cat(FILE *in, FILE *out) {
  char buffer[4096];
  while (size_t bytes_read = fread(buffer, 1, sizeof(buffer), in)) {
    char *buf_p = buffer;
    size_t bytes_remaining = bytes_read;
    while (bytes_remaining) {
      size_t bytes_written = fwrite(buf_p, 1, bytes_remaining, out);
      bytes_remaining -= bytes_written;
      buf_p += bytes_written;
      if (ferror(out)) {
        perror("fwrite");
        return 1;
      }
    }
  }
  if (ferror(in)) {
    perror("fread");
    return 1;
  }
  return 0;
}

int main() { return cat(stdin, stdout); }
