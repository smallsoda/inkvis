/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 */

#include "converter.h"

#include <iostream>


int main()
{
    std::cout << "main >>" << std::endl;

    GpioOut reset{"/dev/gpiochip0", 0};
    GpioOut mode{"/dev/gpiochip0", 1};
    GpioIn busy{"/dev/gpiochip0", 2};
    I2c bus{3, 0x48};

    Converter conv{std::move(bus), busy, mode, reset};

    std::cout << "exit <<" << std::endl;
}
