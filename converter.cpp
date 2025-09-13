/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 * 
 * i2c-to-spi-converter interface
 * https://github.com/smallsoda/i2c-to-spi-converter
 */

#include "converter.h"

#include <filesystem>
#include <iostream>
#include <sstream>
#include <cstring>
#include <thread>
#include <chrono>

extern "C"
{
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
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


Gpio::Gpio(int dev)
{
    std::stringstream stream;

    stream << "/dev/gpiochip" << dev;
    chip_ = std::make_unique<gpiod::chip>(std::filesystem::path(stream.str()));
}


GpioIn::GpioIn(const std::string& name, int dev, int line)
    : Gpio(dev), offset_(line)
{
    auto settings =
        gpiod::line_settings().set_direction(gpiod::line::direction::INPUT);

    request_ = std::make_unique<gpiod::line_request>(
        chip_->prepare_request()
            .set_consumer(name)
            .add_line_settings(offset_, settings)
            .do_request());
}

bool GpioIn::get() const
{
    return static_cast<bool>(request_->get_value(offset_));
}


GpioOut::GpioOut(const std::string& name, int dev, int line, bool val = false)
    : Gpio(dev), offset_(line)
{
    auto settings = gpiod::line_settings()
        .set_direction(gpiod::line::direction::OUTPUT)
        .set_output_value(val ? gpiod::line::value::ACTIVE :
            gpiod::line::value::INACTIVE);

    request_ = std::make_unique<gpiod::line_request>(
        chip_->prepare_request()
            .set_consumer(name)
            .add_line_settings(offset_, settings)
            .do_request());
}

void GpioOut::set(bool val) const
{
    request_->set_value(offset_, val ? gpiod::line::value::ACTIVE :
        gpiod::line::value::INACTIVE);
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

    struct i2c_rdwr_ioctl_data msgset;
    struct i2c_msg msgs[2];
    uint8_t txb, rxb[4];
    int ret;

    msgset.msgs = msgs;
    msgset.nmsgs = 2;

    msgs[0].addr = address_;
    msgs[0].flags = 0;
    msgs[0].buf = &txb;
    msgs[0].len = 1;
    txb = reg;

    msgs[1].addr = address_;
    msgs[1].flags = I2C_M_RD;
    msgs[1].buf = rxb;
    msgs[1].len = 4;
    std::memset(rxb, 0, sizeof(rxb));

    ret = ioctl(fd_, I2C_RDWR, &msgset);
    if (ret < 0)
        return false;

    val = ((uint32_t) rxb[3] << 24) | ((uint32_t) rxb[2] << 16) |
        ((uint32_t) rxb[1] << 8) | rxb[0];
    return true;
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
    cnv_->gpioMode_ = std::make_shared<GpioOut>("mode", dev, line, true);
    return *this;
}

Converter::Builder& Converter::Builder::buildGpioReset(int dev, int line)
{
    cnv_->gpioReset_ = std::make_unique<GpioOut>("reset", dev, line, true);
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
    gpioMode_(std::move(mode)), gpioReset_(std::move(reset))
{
    if (gpioMode_)
        gpioMode_->set(true);
    if (gpioReset_)
        gpioReset_->set(true);
}

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

    return i2cBus_->writeData(val);
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

    if (!gpioReset_)
        return false;

    gpioReset_->set(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    gpioReset_->set(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    return true;
}
