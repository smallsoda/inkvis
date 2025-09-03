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
    Gpio(const Gpio&) = delete;
    Gpio& operator=(const Gpio&) = delete;
    ~Gpio();

private:
    gpiod::chip chip_;
    const int line_;
};


class GpioIn : public Gpio
{
public:
    GpioIn(std::string gpiochip, int line);
    bool get(int& val);
};


class GpioOut : public Gpio
{
public:
    GpioOut(std::string gpiochip, int line);
    bool set(int val);
};


class I2c
{
public:
    I2c(int dev, int address);
    I2c(const I2c&) = delete;
    I2c& operator=(const I2c&) = delete;
    ~I2c();

    bool read(std::vector<uint8_t>& buf);
    bool write(std::vector<uint8_t>& buf);
    
    bool readReg8(uint8_t reg, uint8_t& val);
    bool writeReg8(uint8_t reg, uint8_t val);

    bool readReg32(uint8_t reg, uint32_t& val);
    bool writeReg32(uint8_t reg, uint32_t val);

private:
    int fd_;
    const int address_;
};


class Converter
{
public:
    Converter();
    ~Converter();

    class Builder
    {
    public:
        Builder();
        ~Builder();

        Builder& buildI2cBus(int dev, int address);
        Builder& buildGpioBusy(std::string gpiochip, int line);
        Builder& buildGpioMode(std::string gpiochip, int line);
        Builder& buildGpioReset(std::string gpiochip, int line);
        std::unique_ptr<Converter> build();

    private:
        std::unique_ptr<Converter> cnv_;
    };

private:
    std::unique_ptr<I2c> i2cBus_;
    std::unique_ptr<GpioIn> gpioBusy_;
    std::unique_ptr<GpioOut> gpioMode_;
    std::unique_ptr<GpioOut> gpioReset_;
};

#endif /* CONVERTER_H_ */
