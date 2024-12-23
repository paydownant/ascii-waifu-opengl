#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdexcept>
#include <fstream>
#include <iostream>
#include <iomanip>

#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

#include "imagesource.hpp"

Image :: Image(const std::string filename) {
  const char *filename_c = filename.c_str();
  if (read(filename_c)) {
    //fprintf(stderr, "Read: %s\n", filename_c);
  } else {
    fprintf(stderr, "Failed to Read: %s\n", filename_c);
    throw std::invalid_argument("File Not Found");
  }
  size = w*h*components;
}

Image :: Image(int w, int h, int channels) : w(w), h(h), channels(channels) {
  size = w*h*components;
  data = new uint8_t[size];
  if (data == nullptr) {
    throw std::invalid_argument("Failed to Create Buffer");
  }
}

Image :: Image(const Image &img) : Image(img.w, img.h, img.channels) {
  memcpy(data, img.data, size);
}

Image :: ~Image() {
  stbi_image_free(data);
}

bool Image :: read(const char *filename) {
  data = stbi_load(filename, &w, &h, &channels, components);
  return data != NULL;
}

bool Image :: write(const char *filename) {
  ImageType type = getFileType(filename);
  int success = 0;
  switch (type) {
    case PNG:
      success = stbi_write_png(filename, w, h, components, data, w*components);
      break;
    case JPG:
      success = stbi_write_jpg(filename, w, h, components, data, 100);
      break;
    case BMP:
      success = stbi_write_bmp(filename, w, h, components, data);
      break;
    
    default:
      break;
  }

  return success;
}

bool Image :: write_strip_hpp(const char *file_path) {
  if (getFileType(file_path) != HPP) {
    return false;
  }
  
  std::ofstream outfile(file_path);
  outfile << "static unsigned int icon_image_pixel_size[2] = {" << (int)w  << ", " << (int)h << "};" << std::endl;
  outfile << "static unsigned char icon_image_pixel_data[" << size << "] = {";
  for (unsigned int i = 0; i < size; i++) {
    if (i % w == 0) {
      outfile << std::endl;
    }
    outfile << (int)data[i];
    if (i < size - 1) {
      outfile << ',';
    }
  }
  outfile << std::endl << "};" << std::endl;
  outfile.close();
  return true;
}

ImageType Image :: getFileType(const char *filename) {
  const char *extension = strrchr(filename, '.');
  if (extension == nullptr) {
    return INVALID;
  }

  if (strcmp(extension, ".png") == 0) {
    return PNG;
  }

  if (strcmp(extension, ".jpg") == 0) {
    return JPG;
  }

  if (strcmp(extension, ".bmp") == 0) {
    return BMP;
  }

  if (strcmp(extension, ".hpp") == 0) {
    return HPP;
  }

  return INVALID;

}