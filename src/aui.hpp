#ifndef AUI_HPP
#define AUI_HPP

#include <string>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "imagesource.hpp"

static void error_callback(int error, const char *desc);

typedef char auchar;

struct AuVec3 {
  float x, y, z; // Components of the vector
  // Constructor
  AuVec3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}

  // Vector addition
  AuVec3 operator+(const AuVec3& other) const {
    return AuVec3(x + other.x, y + other.y, z + other.z);
  }

  // Vector subtraction
  AuVec3 operator-(const AuVec3& other) const {
    return AuVec3(x - other.x, y - other.y, z - other.z);
  }

  // Scalar multiplication
  AuVec3 operator*(float scalar) const {
    return AuVec3(x * scalar, y * scalar, z * scalar);
  }

  // Dot product
  float dot(const AuVec3& other) const {
    return x * other.x + y * other.y + z * other.z;
  }

  // Cross product
  AuVec3 cross(const AuVec3& other) const {
    return AuVec3(
      y * other.z - z * other.y,
      z * other.x - x * other.z,
      x * other.y - y * other.x
    );
  }
};

typedef struct ascii_data {
  unsigned int width = 0;
  unsigned int height = 0;
  char *char_strip = nullptr;
  AuVec3 *colour_strip = nullptr;
} ascii_data_t;

typedef struct vertex {
  AuVec3 level;
} vertex_t;

class AUI {

private:

  Image *img_data = nullptr;
  unsigned int width, height;
  vertex_t *vertices = nullptr;

  const char const_points[8] = {' ','.',':','!','i','l','w','W'};
  //const char const_points[8] = {'+','+','+','+','+','+','+','+'};

public:
  AUI();
  ~AUI();
  bool getAsciiBuffer(ascii_data_t *data, auchar* points, unsigned int n_points);
  
  bool createVertexBuffer(unsigned int target_width_px, float aspect_ratio);
  void clean_vertex_buffer();

  bool load_base_image(std::string image_path);
  bool is_base_img_loaded();
  void clean_base_image();

private:
  void drawAUI(FILE *output_ptr);
  
};

#endif