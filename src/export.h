#ifndef EXPORT_H
#define EXPORT_H

#include "imagesource.h"

bool export_buffer_to_img(const uint width, const uint height, const uint channels, const uint *buffer, const char *image_path);

#endif