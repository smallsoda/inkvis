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
    // Gpio(const Gpio &) = delete;
    // Gpio &operator=(const Gpio &) = delete;
    ~Gpio();

protected:
    gpiod::chip chip;
    const int line;
};


class GpioIn : public Gpio
{
public:
    GpioIn(std::string gpiochip, int line);
    bool get(int &val);
};


class GpioOut : public Gpio
{
public:
    // using Gpio::Gpio;
    GpioOut(std::string gpiochip, int line) : Gpio(gpiochip, line) {};
    bool set(int val);
};


class I2c
{
public:
    I2c(int dev, int address);
    I2c(I2c &&other);
    I2c(const I2c &) = delete;
    I2c &operator=(const I2c &) = delete;
    ~I2c();

    bool read(std::vector<uint8_t> &buf);
    bool write(std::vector<uint8_t> &buf);
    
    bool readReg8(uint8_t reg, uint8_t &val);
    bool writeReg8(uint8_t reg, uint8_t val);

    bool readReg32(uint8_t reg, uint32_t &val);
    bool writeReg32(uint8_t reg, uint32_t val);

protected:
    int fd;
    const int address;
};


class Converter
{
public:
    Converter(I2c &&bus, GpioIn &busy, GpioOut &mode, GpioOut &reset);
    ~Converter();

protected:
    I2c i2cBus;
    GpioIn gpioBusy;
    GpioOut gpioMode;
    GpioOut gpioReset;
};

#endif /* CONVERTER_H_ */
