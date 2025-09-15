/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 */

#include <iostream>
#include <vector>

#include "image.h"


static void show(const uint8_t* buf, int width, int height)
{
    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width / 8; x++)
        {
            for (int b = 0; b < 8; b++)
            {
                if ((buf[y * width / 8 + x] >> b) & 0x01)
                    std::cout << " ";
                else
                    std::cout << "*";
            }
        }
        std::cout << std::endl;
    }
}

int main()
{
    std::cout << "main >>" << std::endl;

    Image image("pic.jpg");
    image.resize(32, 32);
    image.toGreyscale();
    image.reduceColors(2);
    image.save("pic-new.png");

    int width;
    int height;
    image.getSize(width, height);

    uint8_t* buffer = new uint8_t[static_cast<size_t>(width) * height / 8 + 1];
    image.compress2Colors(buffer);
    show(buffer, width, height);
    delete[] buffer;

    std::cout << "exit <<" << std::endl;
}
