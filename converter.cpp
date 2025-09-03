/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 * 
 * i2c-to-spi-converter interface
 * https://github.com/smallsoda/i2c-to-spi-converter
 */

#include "converter.h"

#include <iostream>
#include <sstream>

extern "C"
{
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
}


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


I2c::I2c(int dev, int address) : address(address)
{
    std::cout << "I2c" << std::endl;

    std::stringstream stream;
    int ret;

    stream << "/dev/i2c-" << dev;

    fd = open(stream.str().c_str(), O_RDWR);
    if (fd < 0)
        throw std::runtime_error("can not open i2c dev");

    ret = ioctl(fd, I2C_SLAVE, address);
    if (ret < 0)
        throw std::runtime_error("can not set i2c slave address");
}

I2c::I2c(I2c &&other) : fd(other.fd), address(other.address)
{
    std::cout << "I2c (move)" << std::endl;

    other.fd = -1;
}

I2c::~I2c()
{
    std::cout << "~I2c" << " " << this << " " << fd << std::endl;
}


Converter::Converter(I2c &&bus, GpioIn &busy, GpioOut &mode, GpioOut &reset)
    : i2cBus(std::move(bus)), gpioBusy(busy), gpioMode(mode), gpioReset(reset)
{
    std::cout << "Converter" << std::endl;
}

Converter::~Converter()
{
    std::cout << "~Converter" << std::endl;
}
