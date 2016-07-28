
#include "Image.hpp"

#include <cstdio>
#include <setjmp.h>

#ifdef GLOW_JPEG
extern "C" {
#include <jpeglib.h>
}
#endif

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

#ifdef GLOW_JPEG

namespace {

struct jpeg_error_struct {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

void jpeg_error(j_common_ptr info) {
    jpeg_error_struct * err = (jpeg_error_struct*)info->err;
    longjmp(err->setjmp_buffer, 1);
}

}

bool Image::loadJpg(std::string const & path) {
    // http://www.opensource.apple.com/source/tcl/tcl-87/tcl_ext/tkimg/tkimg/libjpeg/libjpeg.doc
    // http://stackoverflow.com/questions/5616216/need-help-in-reading-jpeg-file-using-libjpeg
    // http://stackoverflow.com/questions/19857766/error-handling-in-libjpeg

    // Open file
    FILE * file = fopen(path.c_str(), "rb");
    if (!file)
        return false;
    
    // Create JPEG objects
    JSAMPLE * buffer = nullptr;
    struct jpeg_decompress_struct info;
    jpeg_error_struct err;
    info.err = jpeg_std_error(&err.pub);
    err.pub.error_exit = jpeg_error;

    // Error handler
    if (setjmp(err.setjmp_buffer)) {
        jpeg_destroy_decompress(&info);
        fclose(file);
        delete buffer;
        return false;
    }

    // Setup IO
    jpeg_create_decompress(&info);
    jpeg_stdio_src(&info, file);

    // Read infos
    jpeg_read_header(&info, TRUE);
    width = info.image_width;
    height = info.image_height;

    // Configure RGB 8bits
    info.out_color_space = JCS_RGB;
    info.out_color_components = 3;
    info.output_components = 3;

    // Read image
    colors.resize(width * height);
    buffer = new JSAMPLE[width * 3];
    jpeg_start_decompress(&info);
    for (unsigned y = 0; y < height; ++y) {
        jpeg_read_scanlines(&info, &buffer, 1);
        JSAMPLE * line = (JSAMPLE*)&colors[(height - y - 1) * width];
        for (unsigned i = 0; i < width; ++i) {
            line[4 * i    ] = buffer[3 * i    ];
            line[4 * i + 1] = buffer[3 * i + 1];
            line[4 * i + 2] = buffer[3 * i + 2];
            line[4 * i + 3] = (JSAMPLE)0xff;
        }
    }
    jpeg_finish_decompress(&info);

    // Clean up
    jpeg_destroy_decompress(&info);
    fclose(file);
    delete buffer;
    return true;
}

#else

bool Image::loadJpg(std::string const & path) {
    return false;
}

#endif
