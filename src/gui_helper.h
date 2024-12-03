#ifndef GUI_HELPER
#define GUI_HELPER

#include <iostream>
#include <fstream>
#include <cstring>
#include <ctype.h>

bool is_file_ttf(const char *file_path) {
  const char *ext = strrchr(file_path, '.');
  if (!ext || strcmp(ext, ".ttf") != 0) {
    return false;
  }

  std::ifstream file(file_path, std::ios::binary);
  if (!file) {
    return false;
  }

  char magic[4];
  file.read(magic, 4);

  bool ttf = false;
  ttf = (magic[0] == 0x00 && magic[1] == 0x01 &&
         magic[2] == 0x00 && magic[3] == 0x00) ||
        (magic[0] == 'O' && magic[1] == 'T' &&
         magic[2] == 'T' && magic[3] == 'O');

  file.close();

  return ttf;
}

static void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

#endif