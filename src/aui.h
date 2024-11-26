#ifndef AUI_H
#define AUI_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <string>

#include "imagesource.h"

static void error_callback(int error, const char *desc);

typedef unsigned int auint;
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
  auint width = 0;
  auint height = 0;
  char *char_strip = nullptr;
  AuVec3 *colour_strip = nullptr;
} ascii_data_t;

typedef struct vertex {
  AuVec3 level;
} vertex_t;

class AUI {

private:

  auint width, height;
  vertex_t *vertices = nullptr;

  const char const_points[8] = {' ','.',':','!','i','l','w','W'};
  //const char const_points[8] = {'+','+','+','+','+','+','+','+'};

public:
  AUI();
  ~AUI();
  ascii_data* getAsciiBuffer(auchar *points, auint n_points);
  bool createVertexBuffer(std::string file_path, u_int target_width_px, float aspect_ratio);

  void run(std::string file_path, FILE *output_ptr, auint target_px);

private:
  void drawAUI(FILE *output_ptr);
  
};

#endif