#ifndef IMAGESOURCE_HPP
#define IMAGESOURCE_HPP

#include <stdint.h>
#include <cstdio>
#include <string>
#include <cstring>

enum ImageType {
    INVALID, PNG, JPG, BMP
};

enum Channels {
    R,G,B,Alpha
};

class Image {
    public:
        Image(const std::string filename);
        Image(int w, int h, int channels);
        Image(const Image &img);
        ~Image();

        bool read(const char *filename);
        bool write(const char *filename);
    
    private:
        ImageType getFileType(const char *filename);
        int channels;

    public:
        int w;
        int h;
        const int components = 4;
        uint8_t* data = NULL;
        size_t size = 0;
        
    private:
    
};

#endif