#include "aui.hpp"

#include <stdexcept>

AUI :: AUI() {
    width = 0;
    height = 0;
}

AUI :: ~AUI() {
  printf("Terminating AUI\n");
  clean_vertex_buffer();
  clean_base_image();
}

void AUI :: drawAUI(FILE *output_ptr) {
  for (unsigned int i = 0; i < height; ++i) {
    for (unsigned int j = 0; j < width; ++j) {
      float bw_level = 0.2126 * vertices[i * width + j].level.x + 0.7152 * vertices[i * width + j].level.y + 0.0722 * vertices[i * width + j].level.z;
      unsigned int points_index = (unsigned int)(pow(bw_level, 1.2) * 8);
      fprintf(output_ptr, "%c", const_points[points_index]);
    }
    fprintf(output_ptr, "\n");
  }
}

bool AUI :: getAsciiBuffer(ascii_data_t *data, auchar *points, unsigned int n_points) {

  data->width = width;
  data->height = height;

  data->char_strip = (char*)malloc(sizeof(char*) * width * height + 1);
  if (data->char_strip == NULL) {
    return false;
  }
  data->colour_strip = (AuVec3*)malloc(sizeof(AuVec3) * width * height);
  if (data->colour_strip == NULL) {
    free(data->char_strip);
    return false;
  }

  for (unsigned int i = 0; i < height; ++i) {
    for (unsigned int j = 0; j < width; ++j) {
      float r_level, g_level, b_level;
      r_level = vertices[i * width + j].level.x;
      g_level = vertices[i * width + j].level.y;
      b_level = vertices[i * width + j].level.z;
      
      float bw_level = 0.2126 * r_level + 0.7152 * g_level + 0.0722 * b_level;
      float threshold = 0.01;

      unsigned int point_index = (unsigned int)(pow(bw_level, 1.2) * n_points);
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
  return true;
}

bool AUI :: createVertexBuffer(unsigned int target_width_px, float aspect_ratio) {
  // Load Image

  if (!is_base_img_loaded()) {
    fprintf(stderr, "No img_data found\n");
    return false;
  }

  float ratio_width = (float)target_width_px / img_data->w;
  float ratio_height = ratio_width * aspect_ratio;

  unsigned int target_width = (unsigned int)(ratio_width * img_data->w);
  unsigned int target_height = (unsigned int)(ratio_height * img_data->h);

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

  for (unsigned int i = 0; i < target_height; ++i) {
    for (unsigned int j = 0; j < target_width; ++j) {
      unsigned int index = i * target_width + j;
      unsigned int dataIndexChunk = (unsigned int)((float)i / ratio_height) * img_data->w * 4 + (unsigned int)((float)j * 4 / ratio_width);
      unsigned int dataIndex = dataIndexChunk - dataIndexChunk % 4;
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
  Image *tmp_img_data = nullptr;
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