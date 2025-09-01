/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 * 
 * i2c-to-spi-converter interface
 * https://github.com/smallsoda/i2c-to-spi-converter
 */

#ifndef CONVERTER_H_
#define CONVERTER_H_

#include <string>
#include <gpiod.hpp>

class Gpio
{
public:
    Gpio(std::string gpiochip, int line);
    ~Gpio();

protected:
    gpiod::chip chip;
    const int line;
};


class GpioIn : public Gpio
{
public:
    GpioIn(std::string gpiochip, int line);
    int get();
};


class GpioOut : public Gpio
{
public:
    using Gpio::Gpio;
    int set();
};


#endif /* CONVERTER_H_ */
