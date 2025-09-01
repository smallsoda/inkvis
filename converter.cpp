/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 * 
 * i2c-to-spi-converter interface
 * https://github.com/smallsoda/i2c-to-spi-converter
 */

#include "converter.h"

#include <iostream>


Gpio::Gpio(std::string gpiochip, int line) : chip(gpiochip), line(line)
{
    std::cout << "Gpio" << std::endl;
}

Gpio::~Gpio()
{
    std::cout << "~Gpio" << std::endl;
}


GpioIn::GpioIn(std::string gpiochip, int line) : Gpio(gpiochip, line)
{
    std::cout << "GpioIn" << std::endl;
}
