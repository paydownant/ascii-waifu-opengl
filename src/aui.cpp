#include "aui.h"

#include <math.h>
#include <assert.h>

AUI :: AUI() {
}

AUI :: ~AUI() {
  free(vertices);
}

void AUI :: run(std::string file_path, FILE *output_ptr, auint target_px) {
  if (!createVertexBuffer(file_path, target_px, 0.4)) {
    error_callback(1, "Failed to create vertex buffer");
    exit(1);
  }

  if (output_ptr == nullptr) {
    error_callback(1, "Failed create output file");
    exit(1);
  }

  drawAUI(output_ptr);

}

void AUI :: drawAUI(FILE *output_ptr) {
  for (auint i = 0; i < height; ++i) {
    for (auint j = 0; j < width; ++j) {
      u_int points_index = (u_int)(pow(vertices[i * width + j].level, 1.2) * 8);
      fprintf(output_ptr, "%c", points[points_index]);
    }
    fprintf(output_ptr, "\n");
  }
}

ascii_data* AUI :: getAsciiBuffer() {
  char *strip = (char*)malloc(sizeof(char*) * width * height);
  assert(strip);
  for (auint i = 0; i < height; ++i) {
    for (auint j = 0; j < width; ++j) {
      auint point_index = (auint)(pow(vertices[i * width + j].level, 1.2) * 8);
      strip[i * width + j] = points[point_index];
    }
  }

  ascii_data_t *data = (ascii_data_t*)malloc(sizeof(ascii_data_t));
  assert(data);
  data->char_strip = strip;
  data->width = width;
  data->height = height;
  
  return data;
}

bool AUI :: createVertexBuffer(std::string file_path, u_int target_width_px, float aspect_ratio) {
  // Load Image
  Image img(file_path);
  fprintf(stderr, "Loaded %d x %d Image\n", img.w, img.h);

  float ratio_width = (float)target_width_px / img.w;
  float ratio_height = ratio_width * aspect_ratio;

  u_int target_width = (u_int)(ratio_width * img.w);
  u_int target_height = (u_int)(ratio_height * img.h);

  vertices = (vertex_t*)malloc(sizeof(vertex_t) * target_width * target_height);
  if (vertices == nullptr) {
    error_callback(1, "Failed to allocate memory");
    return false;
  }

  for (u_int i = 0; i < target_height; ++i) {
    for (u_int j = 0; j < target_width; ++j) {
      u_int index = i * target_width + j;
      vertices[index].pos[0] = ((float)i / target_width - 0.5) * 2;
      vertices[index].pos[1] = ((float)j / target_height - 0.5) * 2;

      u_int dataIndex = (u_int)((float)i / ratio_height) * img.w + (u_int)((float)j / ratio_width);
      float bw_level = (float)img.data[dataIndex] / 256;
      float threshold = 0.05;

      if (bw_level < threshold) {
        vertices[index].level = 0;
      } else {
        vertices[index].level = bw_level;
      }
    }
  }

  width = target_width;
  height = target_height;

  fprintf(stderr, "Created %d x %d Buffer\n", target_width, target_height);

  return true;
}


static void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
}