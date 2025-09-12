/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 * 
 * Image processing
 */

#include "image.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE2_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include "stb/stb_image_resize2.h"

#define DESIRED_CHANNELS   3
#define GRAYSCALE_CHANNELS 1


Image::Image(const std::string& filename)
{
    channels_ = DESIRED_CHANNELS; /* 3 */
    image_ = stbi_load(filename.c_str(), &width_, &height_, NULL, channels_);
    if (!image_)
        throw std::runtime_error("can not load image");
}

Image::~Image()
{
    stbi_image_free(image_);
}

bool Image::resize(int width, int height)
{
    unsigned char* newimg = stbir_resize_uint8_linear(image_, width_, height_,
        0, NULL, width, height, 0, (stbir_pixel_layout) channels_);
    if (!newimg)
        return false;

    stbi_image_free(image_);
    image_ = newimg;
    width_ = width;
    height_ = height;

    return true;
}

bool Image::toGreyscale()
{
    size_t length = static_cast<size_t>(width_) * height_;

    if (channels_ != DESIRED_CHANNELS)
        return false;

    /* in-place */
    for (size_t i = 0; i < length; i++)
        image_[i] = 0.299 * image_[3 * i] + 0.587 * image_[3 * i + 1] +
            0.114 * image_[3 * i + 2];

    channels_ = GRAYSCALE_CHANNELS; /* 1 */
    return true;
}

bool Image::reduceColors(uint8_t colors)
{
    size_t length = static_cast<size_t>(width_) * height_;

    if (channels_ != GRAYSCALE_CHANNELS)
        return false;

    if (colors < 2)
        return false;

    /* in-place */
    for (size_t i = 0; i < length; i++)
        image_[i] = (static_cast<int>(image_[i]) * colors / 256) * 255 /
            (colors - 1);

    return true;
}

unsigned char* Image::get() const
{
    return image_;
}

int Image::getChannels() const
{
    return channels_;
}

void Image::getSize(int &width, int &height) const
{
    width = width_;
    height = height_;
}

bool Image::compress(int bpp, std::vector<uint8_t>& buffer) const
{
    size_t length = static_cast<size_t>(width_) * height_;
    int colors;

    if (channels_ != GRAYSCALE_CHANNELS)
        return false;

    if (bpp != 1 && bpp != 2 && bpp != 4 && bpp != 8)
        return false;

    colors = 1 << bpp;

    for (size_t i = 0; i < length; i++)
    {
        int tmp = (static_cast<int>(image_[i]) * colors / 256);
        int shift = (i * bpp) % 8;

        if (shift == 0)
            buffer.push_back(tmp);
        else
            buffer.back() |= tmp << shift;
    }

    return true;
}

bool Image::save(const std::string& filename) const
{
    int ret = stbi_write_png(filename.c_str(), width_, height_, channels_,
        image_, 0);
    return ret ? true : false;
}
