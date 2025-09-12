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
#include <unistd.h>
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


class GpioSetter
{
public:
    GpioSetter(std::shared_ptr<GpioOut> gpio, bool state)
        : initState_(state), gpio_(gpio)
    {
        if (gpio_)
            gpio_->set(initState_);
    }

    ~GpioSetter()
    {
        if (gpio_)
            gpio_->set(!initState_);
    }

private:
    bool initState_;
    std::shared_ptr<GpioOut> gpio_;
};


Gpio::Gpio(int dev, int line)
{
    std::stringstream stream;

    stream << "gpiochip" << dev;

    chip_ = std::make_unique<gpiod::chip>(stream.str());
    /* Is it ok? */
    line_ = std::make_unique<gpiod::line>(std::move(chip_->get_line(line)));
}


GpioIn::GpioIn(const std::string& name, int dev, int line) : Gpio(dev, line)
{
    line_->request({name, gpiod::line_request::DIRECTION_INPUT, 0}, 0);
}

GpioIn::~GpioIn()
{
    line_->release();
}

int GpioIn::get() const
{
    return line_->get_value();
}


GpioOut::GpioOut(const std::string& name, int dev, int line) : Gpio(dev, line)
{
    line_->request({name, gpiod::line_request::DIRECTION_OUTPUT, 0}, 0);
}

GpioOut::~GpioOut()
{
    line_->release();
}

void GpioOut::set(int val) const
{
    line_->set_value(val);
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

    close(fd_);
}

bool I2c::readData(std::vector<uint8_t>& buf, size_t len) const
{
    std::cout << __func__ << std::endl;

    // TODO
    return false;
}

bool I2c::writeData(const std::vector<uint8_t>& buf) const
{
    std::cout << __func__ << std::endl;

    // TODO
    return false;
}

bool I2c::readReg8(uint8_t reg, uint8_t& val) const
{
    std::cout << __func__ << std::endl;

    // TODO
    return false;
}

bool I2c::writeReg8(uint8_t reg, uint8_t val) const
{
    std::cout << __func__ << std::endl;

    // TODO
    return false;
}

bool I2c::readReg32(uint8_t reg, uint32_t& val) const
{
    std::cout << __func__ << std::endl;

    // TODO
    return false;
}

bool I2c::writeReg32(uint8_t reg, uint32_t val) const
{
    std::cout << __func__ << std::endl;

    // TODO
    return false;
}


Converter::Builder::Builder() : cnv_(std::make_unique<Converter>()) {}

Converter::Builder& Converter::Builder::buildI2cBus(int dev, int address)
{
    cnv_->i2cBus_ = std::make_unique<I2c>(dev, address);
    return *this;
}

Converter::Builder& Converter::Builder::buildGpioBusy(int dev, int line)
{
    cnv_->gpioBusy_ = std::make_unique<GpioIn>("busy", dev, line);
    return *this;
}

Converter::Builder& Converter::Builder::buildGpioMode(int dev, int line)
{
    cnv_->gpioMode_ = std::make_shared<GpioOut>("mode", dev, line);
    return *this;
}

Converter::Builder& Converter::Builder::buildGpioReset(int dev, int line)
{
    cnv_->gpioReset_ = std::make_unique<GpioOut>("reset", dev, line);
    return *this;
}

std::unique_ptr<Converter> Converter::Builder::build()
{
    if (!cnv_->i2cBus_)
        throw std::runtime_error("i2c is not initialized");

    return std::move(cnv_);
}


Converter::Converter(std::unique_ptr<I2c> bus, std::unique_ptr<GpioIn> busy,
        std::shared_ptr<GpioOut> mode, std::unique_ptr<GpioOut> reset)
    : i2cBus_(std::move(bus)), gpioBusy_(std::move(busy)),
    gpioMode_(std::move(mode)), gpioReset_(std::move(reset)) {}

Converter::Converter()
{
    std::cout << __func__ << std::endl;
}

bool Converter::readData(std::vector<uint8_t>& buf, size_t len) const
{
    std::cout << __func__ << std::endl;

    // TODO
    return false;
}

bool Converter::writeData(const std::vector<uint8_t>& buf) const
{
    std::cout << __func__ << std::endl;

    // TODO
    return false;
}

bool Converter::getCounter(Counter counter, unsigned long& val) const
{
    std::cout << __func__ << std::endl;

    GpioSetter mode_setter(gpioMode_, false);
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

bool Converter::resetCounter(Counter counter) const
{
    std::cout << __func__ << std::endl;

    GpioSetter mode_setter(gpioMode_, false);
    std::vector<uint8_t> val;

    switch (counter)
    {
        case Counter::RX: val.push_back(CMD_RXC_RESET); break;
        case Counter::TX: val.push_back(CMD_TXC_RESET); break;
    }

    return i2cBus_->write(val);
}

bool Converter::getGpioValue(GpioNum num, bool& value) const
{
    std::cout << __func__ << std::endl;

    // TODO
    return false;
}

bool Converter::setGpioValue(GpioNum num, bool value) const
{
    std::cout << __func__ << std::endl;

    // TODO
    return false;
}

bool Converter::setGpioMode(GpioNum num, GpioMode mode) const
{
    std::cout << __func__ << std::endl;

    // TODO
    return false;
}

bool Converter::setSpiMode(SpiMode mode) const
{
    std::cout << __func__ << std::endl;

    // TODO
    return false;
}

bool Converter::setSpiBaudrate(SpiBaudrate baudrate) const
{
    std::cout << __func__ << std::endl;

    // TODO
    return false;
}

bool Converter::reset() const
{
    std::cout << __func__ << std::endl;

    // TODO
    return false;
}
