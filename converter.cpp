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


/* converter commands */
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

/* gpio modes */
#define GPIO_MODE_IN_FL  0x00
#define GPIO_MODE_IN_PU  0x01
#define GPIO_MODE_OUT_PP 0x02
#define GPIO_MODE_OUT_OD 0x03

/* spi modes */
#define SPI_MODE_0 0x00
#define SPI_MODE_1 0x01
#define SPI_MODE_2 0x02
#define SPI_MODE_3 0x03

/* spi baudrate */
#define SPI_BAUDRATE_2   0x01
#define SPI_BAUDRATE_4   0x02
#define SPI_BAUDRATE_8   0x03
#define SPI_BAUDRATE_16  0x04
#define SPI_BAUDRATE_32  0x05
#define SPI_BAUDRATE_64  0x06
#define SPI_BAUDRATE_128 0x07
#define SPI_BAUDRATE_256 0x08


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
    ssize_t ret;

    buf.resize(len);
    ret = read(fd_, buf.data(), buf.size());
    if (ret <= 0)
        return false;

    if (static_cast<size_t>(ret) != len)
        buf.resize(ret);

    return true;
}

bool I2c::writeData(const std::vector<uint8_t>& buf) const
{
    ssize_t ret = write(fd_, buf.data(), buf.size());
    if (ret <= 0)
        return false;

    return true;
}

bool I2c::readReg8(uint8_t reg, uint8_t& val) const
{
    struct i2c_rdwr_ioctl_data msgset;
    struct i2c_msg msgs[2];
    uint8_t txb, rxb;
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
    msgs[1].buf = &rxb;
    msgs[1].len = 1;
    rxb = 0;

    ret = ioctl(fd_, I2C_RDWR, &msgset);
    if (ret < 0)
        return false;

    val = rxb;
    return true;
}

bool I2c::writeReg8(uint8_t reg, uint8_t val) const
{
    struct i2c_rdwr_ioctl_data msgset;
    struct i2c_msg msg;
    uint8_t txb[2];
    int ret;

    msgset.msgs = &msg;
    msgset.nmsgs = 1;

    msg.addr = address_;
    msg.flags = 0;
    msg.buf = txb;
    msg.len = 2;
    txb[0] = reg;
    txb[1] = val;

    ret = ioctl(fd_, I2C_RDWR, &msgset);
    if (ret < 0)
        return false;

    return true;
}

bool I2c::readReg32(uint8_t reg, uint32_t& val) const
{
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
    struct i2c_rdwr_ioctl_data msgset;
    struct i2c_msg msg;
    uint8_t txb[5];
    int ret;

    msgset.msgs = &msg;
    msgset.nmsgs = 1;

    msg.addr = address_;
    msg.flags = 0;
    msg.buf = txb;
    msg.len = 5;
    txb[0] = reg;
    txb[1] = val;
    txb[2] = val >> 8;
    txb[3] = val >> 16;
    txb[4] = val >> 24;

    ret = ioctl(fd_, I2C_RDWR, &msgset);
    if (ret < 0)
        return false;

    return true;
}


Converter::Builder& Converter::Builder::buildI2cBus(int dev, int address)
{
    i2cBus_ = std::make_unique<I2c>(dev, address);
    return *this;
}

Converter::Builder& Converter::Builder::buildGpioBusy(int dev, int line)
{
    gpioBusy_ = std::make_unique<GpioIn>("busy", dev, line);
    return *this;
}

Converter::Builder& Converter::Builder::buildGpioMode(int dev, int line)
{
    gpioMode_ = std::make_shared<GpioOut>("mode", dev, line, true);
    return *this;
}

Converter::Builder& Converter::Builder::buildGpioReset(int dev, int line)
{
    gpioReset_ = std::make_unique<GpioOut>("reset", dev, line, true);
    return *this;
}

std::unique_ptr<Converter> Converter::Builder::build()
{
    return std::make_unique<Converter>(std::move(i2cBus_),
        std::move(gpioBusy_), std::move(gpioMode_), std::move(gpioReset_));
}


Converter::Converter(std::unique_ptr<I2c> bus, std::unique_ptr<GpioIn> busy,
        std::shared_ptr<GpioOut> mode, std::unique_ptr<GpioOut> reset)
    : i2cBus_(std::move(bus)), gpioBusy_(std::move(busy)),
    gpioMode_(std::move(mode)), gpioReset_(std::move(reset))
{
    std::cout << __func__ << std::endl;

    if (!i2cBus_)
        throw std::runtime_error("i2c is not initialized");

    if (gpioMode_)
        gpioMode_->set(true);
    if (gpioReset_)
        gpioReset_->set(true);
}

Converter::~Converter()
{
    std::cout << __func__ << std::endl;
}

bool Converter::readData(std::vector<uint8_t>& buf, size_t len) const
{
    // TODO: busy

    return i2cBus_->readData(buf, len);
}

bool Converter::writeData(const std::vector<uint8_t>& buf) const
{
    // TODO: busy

    return i2cBus_->writeData(buf);
}

bool Converter::getCounter(Counter counter, unsigned long& val) const
{
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
    GpioSetter mode_setter(gpioMode_, false);

    std::vector<uint8_t> val;

    switch (counter)
    {
        case Counter::RX: val.push_back(CMD_RXC_RESET); break;
        case Counter::TX: val.push_back(CMD_TXC_RESET); break;
    }

    return i2cBus_->writeData(val);
}

bool Converter::getGpioValue(GpioNum num, bool& val) const
{
    GpioSetter mode_setter(gpioMode_, false);

    uint8_t regval;
    uint8_t reg;
    bool status;

    switch (num)
    {
        case GpioNum::GPIO1: reg = CMD_GPIO1_GET; break;
        case GpioNum::GPIO2: reg = CMD_GPIO2_GET; break;
        case GpioNum::GPIO3: reg = CMD_GPIO3_GET; break;
    }

    status = i2cBus_->readReg8(reg, regval);
    if (!status)
        return false;

    val = static_cast<bool>(regval);
    return true;
}

bool Converter::setGpioValue(GpioNum num, bool val) const
{
    GpioSetter mode_setter(gpioMode_, false);

    uint8_t regval = static_cast<uint8_t>(val);
    uint8_t reg;
    bool status;

    switch (num)
    {
        case GpioNum::GPIO1: reg = CMD_GPIO1_SET; break;
        case GpioNum::GPIO2: reg = CMD_GPIO2_SET; break;
        case GpioNum::GPIO3: reg = CMD_GPIO3_SET; break;
    }

    status = i2cBus_->writeReg8(reg, regval);
    if (!status)
        return false;

    return true;
}

bool Converter::setGpioMode(GpioNum num, GpioMode mode) const
{
    GpioSetter mode_setter(gpioMode_, false);

    uint8_t regval;
    uint8_t reg;
    bool status;

    switch (num)
    {
        case GpioNum::GPIO1: reg = CMD_GPIO1_MODE; break;
        case GpioNum::GPIO2: reg = CMD_GPIO2_MODE; break;
        case GpioNum::GPIO3: reg = CMD_GPIO3_MODE; break;
    }

    switch (mode)
    {
        case GpioMode::IN_FL:  regval = GPIO_MODE_IN_FL;  break;
        case GpioMode::IN_PU:  regval = GPIO_MODE_IN_PU;  break;
        case GpioMode::OUT_PP: regval = GPIO_MODE_OUT_PP; break;
        case GpioMode::OUT_OD: regval = GPIO_MODE_OUT_OD; break;
    }

    status = i2cBus_->writeReg8(reg, regval);
    if (!status)
        return false;

    return true;
}

bool Converter::setSpiMode(SpiMode mode) const
{
    GpioSetter mode_setter(gpioMode_, false);

    uint8_t regval;
    uint8_t reg = CMD_SPI_MODE;
    bool status;

    switch (mode)
    {
        case SpiMode::MODE0: regval = SPI_MODE_0; break;
        case SpiMode::MODE1: regval = SPI_MODE_1; break;
        case SpiMode::MODE2: regval = SPI_MODE_2; break;
        case SpiMode::MODE3: regval = SPI_MODE_3; break;
    }

    status = i2cBus_->writeReg8(reg, regval);
    if (!status)
        return false;

    return true;
}

bool Converter::setSpiBaudrate(SpiBaudrate baudrate) const
{
    GpioSetter mode_setter(gpioMode_, false);

    uint8_t regval;
    uint8_t reg = CMD_SPI_BAUDRATE;
    bool status;

    switch (baudrate)
    {
        case SpiBaudrate::BR2:   regval = SPI_BAUDRATE_2;   break;
        case SpiBaudrate::BR4:   regval = SPI_BAUDRATE_4;   break;
        case SpiBaudrate::BR8:   regval = SPI_BAUDRATE_8;   break;
        case SpiBaudrate::BR16:  regval = SPI_BAUDRATE_16;  break;
        case SpiBaudrate::BR32:  regval = SPI_BAUDRATE_32;  break;
        case SpiBaudrate::BR64:  regval = SPI_BAUDRATE_64;  break;
        case SpiBaudrate::BR128: regval = SPI_BAUDRATE_128; break;
        case SpiBaudrate::BR256: regval = SPI_BAUDRATE_256; break;
    }

    status = i2cBus_->writeReg8(reg, regval);
    if (!status)
        return false;

    return true;
}

bool Converter::reset() const
{
    if (!gpioReset_)
        return false;

    gpioReset_->set(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    gpioReset_->set(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    return true;
}
