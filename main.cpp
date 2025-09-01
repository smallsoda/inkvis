/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 */

#include "converter.h"

#include <iostream>


int main()
{
    std::cout << "main >>" << std::endl;

    GpioIn in{"/dev/gpiochip0", 1};
    GpioOut out{"/dev/gpiochip0", 2};

    std::cout << "exit" << std::endl;
}
