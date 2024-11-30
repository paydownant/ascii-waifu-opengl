#include "aui.h"

#include <assert.h>

#include <stdexcept>

AUI :: AUI() {
}

AUI :: ~AUI() {
  printf("Terminating AUI\n");
  clean_vertex_buffer();
  clean_base_image();
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

  ascii_data_t *data = (ascii_data_t*)malloc(sizeof(ascii_data_t));
  assert(data);
  data->width = width;
  data->height = height;

  data->char_strip = (char*)malloc(sizeof(char*) * width * height);
  if (data->char_strip == NULL) {
    free(data);
  }
  data->colour_strip = (AuVec3*)malloc(sizeof(AuVec3) * width * height);
  if (data->colour_strip == NULL) {
    free(data->char_strip);
    free(data);
  }

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
        data->char_strip[i * width + j] = points[point_index];
      } else {
        data->char_strip[i * width + j] = ' ';
      }
      
      data->colour_strip[i * width + j].x = vertices[i * width + j].level.x;
      data->colour_strip[i * width + j].y = vertices[i * width + j].level.y;
      data->colour_strip[i * width + j].z = vertices[i * width + j].level.z;
    }
  }
  
  return data;
}

bool AUI :: createVertexBuffer(auint target_width_px, float aspect_ratio) {
  // Load Image

  if (!img_data) {
    fprintf(stderr, "No img_data found\n");
    return false;
  }

  float ratio_width = (float)target_width_px / img_data->w;
  float ratio_height = ratio_width * aspect_ratio;

  auint target_width = (auint)(ratio_width * img_data->w);
  auint target_height = (auint)(ratio_height * img_data->h);

  if (target_width < 1) target_width = 1;
  if (target_height < 1) target_height = 1;

  if (vertices == nullptr) {
    vertices = (vertex_t*)malloc(sizeof(vertex_t) * target_width * target_height);
  } else {
    vertices = (vertex_t*)realloc(vertices, sizeof(vertex_t) * target_width * target_height);
  }

  if (vertices == nullptr) {
    error_callback(1, "Failed to allocate memory");
    return false;
  }

  for (auint i = 0; i < target_height; ++i) {
    for (auint j = 0; j < target_width; ++j) {
      auint index = i * target_width + j;
      auint dataIndexChunk = (auint)((float)i / ratio_height) * img_data->w * 4 + (auint)((float)j * 4 / ratio_width);
      auint dataIndex = dataIndexChunk - dataIndexChunk % 4;
      float r_level, b_level, g_level, alpha_level;
      r_level = (float)img_data->data[dataIndex + 0] / 256;
      g_level = (float)img_data->data[dataIndex + 1] / 256;
      b_level = (float)img_data->data[dataIndex + 2] / 256;
      alpha_level = (float)img_data->data[dataIndex + 3] / 256;
      
      vertices[index].level = AuVec3(r_level, g_level, b_level);
    }
  }

  width = target_width;
  height = target_height;

  return true;
}

void AUI :: clean_vertex_buffer() {
  free(vertices);
}

bool AUI :: load_base_image(std::string image_path) {
  
  Image *tmp_img_data;
  try {
    tmp_img_data = new Image(image_path);
  } catch(const std::invalid_argument& e) {
    return false;
  }

  if (img_data != nullptr) {
    clean_base_image();
  }

  img_data = tmp_img_data;

  return true;
}

bool AUI :: is_base_img_loaded() {
  if (img_data == nullptr) return false;
  return true;
}

void AUI :: clean_base_image() {
  delete(img_data);
}


static void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
}