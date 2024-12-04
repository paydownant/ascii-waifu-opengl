#include <stdexcept>

#include "export.h"

bool export_buffer_to_img(const uint width, const uint height, const uint channels, const uint *buffer, const char *image_path) {
  if (width * height <= 0 || buffer == nullptr || image_path == nullptr) {
    return false;
  }

  Image *export_image = nullptr;
  try {
    export_image = new Image(width, height, channels);
  } catch(const std::invalid_argument& e) {
    return false;
  }

  // Copy the pixel data from buffer to data, flipping vertically
  for (uint y = 0; y < height; ++y) {
    uint flipped_y = height - 1 - y; // Reverse the row order
    for (uint x = 0; x < width; ++x) {
      for (uint channel = 0; channel < channels; ++channel) {
        uint src_index = (flipped_y * width + x) * channels + channel; // Source index (flipped)
        uint dest_index = (y * width + x) * channels + channel;        // Destination index
        export_image->data[dest_index] = buffer[src_index];
      }
    }
  }

  // Write to file
  if (!export_image->write(image_path)) {
    delete(export_image);
    return false;
  }

  return true;
}
