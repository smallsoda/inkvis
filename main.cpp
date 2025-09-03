/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 */

#include "converter.h"

#include <iostream>


int main()
{
    std::cout << "main >>" << std::endl;

    auto converter = Converter::Builder()
        .buildI2cBus(3, 0x48)
        .buildGpioBusy("/dev/gpiochip0", 2)
        .buildGpioMode("/dev/gpiochip0", 1)
        .buildGpioReset("/dev/gpiochip0", 0)
        .build();

    unsigned long val;
    converter->getCounter(Converter::Counter::RX, val);

    std::cout << "exit <<" << std::endl;
}
