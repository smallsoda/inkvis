/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 * 
 * Image processing
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include <cstdint>
#include <string>
#include <vector>


class Image
{
public:
    Image(const std::string& filename);
    ~Image();

    bool resize(int width, int height);
    bool toGreyscale();
    bool reduceColors(uint8_t colors);
    
    unsigned char* get() const;
    int getChannels() const;
    void getSize(int& width, int& height) const;

    bool compress2Colors(uint8_t* buf) const;
    bool compress4Colors(uint8_t* buf10, uint8_t* buf13) const;
    bool save(const std::string& filename) const;

private:
    int width_;
    int height_;
    int channels_;
    unsigned char* image_;
};

#endif /* IMAGE_H_ */
