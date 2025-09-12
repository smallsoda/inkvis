/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 */

#include <iostream>
#include <vector>

#include "image.h"


int main()
{
    std::cout << "main >>" << std::endl;

    Image image("forest.jpg");
    image.resize(400, 300);
    image.toGreyscale();

    // image.reduceColors(4);
    // image.save("forest-reduceColors-4.png");

    std::vector<uint8_t> cmp;
    image.compress(2, cmp);

    std::cout << std::endl;
    for (int i = 0; i < 20; i++)
        std::cout << (int) cmp[i] << std::endl;

    std::cout << "exit <<" << std::endl;
}
