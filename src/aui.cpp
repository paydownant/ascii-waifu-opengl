#include "aui.h"

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
      float bw_level = 0.2126 * vertices[i * width + j].level.x + 0.7152 * vertices[i * width + j].level.y + 0.0722 * vertices[i * width + j].level.z;
      auint points_index = (auint)(pow(bw_level, 1.2) * 8);
      fprintf(output_ptr, "%c", const_points[points_index]);
    }
    fprintf(output_ptr, "\n");
  }
}

ascii_data* AUI :: getAsciiBuffer(auchar *points, auint n_points) {
  char *strip = (char*)malloc(sizeof(char*) * width * height);
  assert(strip);
  AuVec3 *colour_strip = (AuVec3*)malloc(sizeof(AuVec3) * width * height);
  assert(colour_strip);
  for (auint i = 0; i < height; ++i) {
    for (auint j = 0; j < width; ++j) {
      float r_level, g_level, b_level;
      r_level = vertices[i * width + j].level.x;
      g_level = vertices[i * width + j].level.y;
      b_level = vertices[i * width + j].level.z;
      
      float bw_level = 0.2126 * r_level + 0.7152 * g_level + 0.0722 * b_level;
      float threshold = 0.01;

      auint point_index = (auint)(pow(bw_level, 1.2) * n_points);
      
      if (bw_level > threshold) {
        strip[i * width + j] = points[point_index];
      } else {
        strip[i * width + j] = ' ';
      }
      
      
      colour_strip[i * width + j].x = vertices[i * width + j].level.x;
      colour_strip[i * width + j].y = vertices[i * width + j].level.y;
      colour_strip[i * width + j].z = vertices[i * width + j].level.z;
    }
  }

  ascii_data_t *data = (ascii_data_t*)malloc(sizeof(ascii_data_t));
  assert(data);
  data->char_strip = strip;
  data->colour_strip = colour_strip;
  data->width = width;
  data->height = height;
  
  return data;
}

bool AUI :: createVertexBuffer(std::string file_path, auint target_width_px, float aspect_ratio) {
  // Load Image
  Image img(file_path);
  fprintf(stderr, "Loaded %d x %d Image\n", img.w, img.h);

  float ratio_width = (float)target_width_px / img.w;
  float ratio_height = ratio_width * aspect_ratio;

  auint target_width = (auint)(ratio_width * img.w);
  auint target_height = (auint)(ratio_height * img.h);

  vertices = (vertex_t*)malloc(sizeof(vertex_t) * target_width * target_height);
  if (vertices == nullptr) {
    error_callback(1, "Failed to allocate memory");
    return false;
  }

  for (auint i = 0; i < target_height; ++i) {
    for (auint j = 0; j < target_width; ++j) {
      auint index = i * target_width + j;
      auint dataIndexChunk = (auint)((float)i / ratio_height) * img.w * 4 + (auint)((float)j * 4 / ratio_width);
      auint dataIndex = dataIndexChunk - dataIndexChunk % 4;
      float r_level, b_level, g_level, alpha_level;
      r_level = (float)img.data[dataIndex + 0] / 256;
      g_level = (float)img.data[dataIndex + 1] / 256;
      b_level = (float)img.data[dataIndex + 2] / 256;
      alpha_level = (float)img.data[dataIndex + 3] / 256;
      
      vertices[index].level = AuVec3(r_level, g_level, b_level);
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