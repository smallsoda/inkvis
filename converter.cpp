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


/* Converter commands */
#define CMD_RXC_GET      0x04
#define CMD_TXC_GET      0x05
#define CMD_RXC_RESET    0x14
#define CMD_TXC_RESET    0x15
#define CMD_GPIO1_GET    0x01
#define CMD_GPIO2_GET    0x02
#define CMD_GPIO3_GET    0x03
#define CMD_GPIO1_SET    0x21
#define CMD_GPIO2_SET    0x22
#define CMD_GPIO3_SET    0x23
#define CMD_GPIO1_MODE   0x31
#define CMD_GPIO2_MODE   0x32
#define CMD_GPIO3_MODE   0x33
#define CMD_SPI_MODE     0x30
#define CMD_SPI_BAUDRATE 0x40


Gpio::Gpio(std::string gpiochip, int line) : chip_(gpiochip), line_(line)
{
    std::cout << __func__ << std::endl;
}

Gpio::~Gpio()
{
    std::cout << __func__ << std::endl;
}


GpioIn::GpioIn(std::string gpiochip, int line) : Gpio(gpiochip, line)
{
    std::cout << __func__ << std::endl;
}


GpioOut::GpioOut(std::string gpiochip, int line) : Gpio(gpiochip, line)
{
    std::cout << __func__ << std::endl;
}


I2c::I2c(int dev, int address) : address_(address)
{
    std::cout << __func__ << std::endl;

    std::stringstream stream;
    int ret;

    stream << "/dev/i2c-" << dev;

    fd_ = open(stream.str().c_str(), O_RDWR);
    if (fd_ < 0)
        throw std::runtime_error("can not open i2c dev");

    ret = ioctl(fd_, I2C_SLAVE, address_);
    if (ret < 0)
        throw std::runtime_error("can not set i2c slave address");
}

I2c::~I2c()
{
    std::cout << __func__ << std::endl;
}

bool I2c::readReg32(uint8_t reg, uint32_t& val)
{
    std::cout << __func__ << std::endl;

    return false;
}


Converter::Converter()
{
    std::cout << __func__ << std::endl;
}

Converter::~Converter()
{
    std::cout << __func__ << std::endl;
}

bool Converter::getCounter(Counter counter, unsigned long& val)
{
    std::cout << __func__ << std::endl;
    
    uint32_t regval;
    uint8_t reg;
    bool status;

    switch (counter)
    {
    case Counter::RX: reg = CMD_RXC_GET; break;
    case Counter::TX: reg = CMD_TXC_GET; break;
    }

    status = i2cBus_->readReg32(reg, regval);
    if (!status)
        return false;

    val = regval;
    return true;
}


Converter::Builder::Builder() : cnv_(std::make_unique<Converter>())
{
    std::cout << __func__ << std::endl;
}

Converter::Builder::~Builder()
{
    std::cout << __func__ << std::endl;
}

Converter::Builder& Converter::Builder::buildI2cBus(int dev, int address)
{
    cnv_->i2cBus_ = std::make_unique<I2c>(dev, address);
    return *this;
}

Converter::Builder& Converter::Builder::buildGpioBusy(std::string gpiochip,
    int line)
{
    cnv_->gpioBusy_ = std::make_unique<GpioIn>(gpiochip, line);
    return *this;
}

Converter::Builder& Converter::Builder::buildGpioMode(std::string gpiochip,
    int line)
{
    cnv_->gpioMode_ = std::make_unique<GpioOut>(gpiochip, line);
    return *this;
}

Converter::Builder& Converter::Builder::buildGpioReset(std::string gpiochip,
    int line)
{
    cnv_->gpioReset_ = std::make_unique<GpioOut>(gpiochip, line);
    return *this;
}

std::unique_ptr<Converter> Converter::Builder::build()
{
    return std::move(cnv_);
}
