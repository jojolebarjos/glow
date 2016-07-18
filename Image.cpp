
#include "Image.hpp"

#include <cstdio>

Image::Image() : width(0), height(0) {}

GLuint Image::getWidth() const {
    return width;
}

GLuint Image::getHeight() const {
    return height;
}

GLuint const * Image::getPointer() const {
    return colors.data();
}

bool Image::load(std::string const & path) {
    // TODO improve this based on extension
    return loadBmp(path) || loadPng(path) || loadJpg(path);
}

bool Image::loadBmp(std::string const & path) {
    // See https://en.wikipedia.org/wiki/BMP_file_format
    
    // Open file
    FILE * file = fopen(path.c_str(), "rb");
    if (!file)
        return false;
    
    // Read header
    char header[54];
    if (fread(header, sizeof(header), 1, file) != 1) {
        fclose(file);
        return false;
    }
    
    // Check magic, header size and compression method
    if (header[0] != 'B' || header[1] != 'M' || *(uint32_t*)(header + 14) != 40 || *(uint32_t*)(header + 30) != 0) {
        fclose(file);
        return false;
    }
    
    // Get relevant informations
    uint32_t width = *(uint32_t*)(header + 18);
    uint32_t height = *(uint32_t*)(header + 22);
    uint16_t depth = *(uint16_t*)(header + 28);
    uint32_t offset = *(uint32_t*)(header + 10);
    fseek(file, 0, SEEK_END);
    uint32_t size = ftell(file) - offset;
    
    // Validate depth
    // TODO handle 32bits BMP?
    if (depth != 24) {
        fclose(file);
        return false;
    }
    depth /= 8;
    
    // Read pixels
    fseek(file, offset, SEEK_SET);
    std::vector<unsigned char> bytes(size);
    if (fread(bytes.data(), size, 1, file) != 1)
        return false;
    fclose(file);
    
    // Decode pixels
    this->width = width;
    this->height = height;
    colors.resize(width * height);
    uint32_t row = (width * 3 + 3) & ~3;
    for (uint32_t x = 0; x < width; x++)
        for (uint32_t y = 0; y < height; y++) {
            uint32_t i = x * 3 + y * row;
            uint32_t color = 0;
            color |= bytes[i + 2] & 0xff;
            color |= (bytes[i + 1] & 0xff) << 8;
            color |= (bytes[i] & 0xff) << 16;
            color |= 0xff << 24;
            colors[x + (height - y - 1) * width] = color;
	}
    return true;
}

bool Image::loadPng(std::string const & path) {
    // TODO use libPNG
    return false;
}

bool Image::loadJpg(std::string const & path) {
    // TODO use libJPEG
    return false;
}
