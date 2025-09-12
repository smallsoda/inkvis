/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 * 
 * Image processing
 */

#ifndef IMAGE_H_
#define IMAGE_H_

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
    void getSize(int &width, int &height) const;

    bool compress(int bpp, std::vector<uint8_t>& buffer) const;
    bool save(const std::string& filename) const;

private:
    int width_;
    int height_;
    int channels_;
    unsigned char* image_;
};

#endif /* IMAGE_H_ */
