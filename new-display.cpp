/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 * 
 * Source: waveshareteam
 * https://github.com/waveshareteam/e-Paper
 */

#include "new-display.h"

#include <iostream>
#include <thread>
#include <chrono>

#define CMD_PANEL_SETTING                  0x00
#define CMD_POWER_SETTING                  0x01
#define CMD_POWER_ON                       0x04
#define CMD_BOOSTER_SOFT_START             0x06
#define CMD_DISPLAY_REFRESH                0x12
#define CMD_PLL_CONTROL                    0x30
#define CMD_VCOM_AND_DATA_INTERVAL_SETTING 0x50
#define CMD_RESOLUTION_SETTING             0x61
#define CMD_VCM_DC_SETTING                 0x82


class Logger
{
public:
    Logger(const std::string s) : s_(s)
    {
        std::cout << s_ << " >" << std::endl;
    }
    ~Logger()
    {
        std::cout << s_ << " <" << std::endl;
    }

private:
    const std::string s_;
};

static void delay(int val)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(val));
}


ConnectorConverter::ConnectorConverter(std::shared_ptr<Converter> cnv,
    Converter::GpioNum reset, Converter::GpioNum busy, Converter::GpioNum dc)
    : cnv_(cnv), reset_(reset), busy_(busy), dc_(dc)
{
    bool status;

    std::cout << __func__ << std::endl;

    /* without reset */

    /* busy */
    status = cnv_->setGpioMode(busy_, Converter::GpioMode::IN_PU);
    if (!status)
        throw std::runtime_error("can not set gpio mode (busy)");

    /* reset */
    status = cnv_->setGpioMode(reset_, Converter::GpioMode::OUT_PP);
    if (!status)
        throw std::runtime_error("can not set gpio mode (reset)");
    status = cnv_->setGpioValue(reset_, true);
    if (!status)
        throw std::runtime_error("can not set gpio value (reset)");

    /* dc */
    status = cnv_->setGpioMode(dc_, Converter::GpioMode::OUT_PP);
    if (!status)
        throw std::runtime_error("can not set gpio mode (dc)");

    /* spi */
    status = cnv_->setSpiMode(Converter::SpiMode::MODE0);
    if (!status)
        throw std::runtime_error("can not set spi mode");
    status = cnv_->setSpiBaudrate(Converter::SpiBaudrate::BR32);
    if (!status)
        throw std::runtime_error("can not set spi baudrate");
}

ConnectorConverter::~ConnectorConverter()
{
    std::cout << __func__ << std::endl;
}

bool ConnectorConverter::readBusy(bool& val) const
{
    std::cout << __func__ << std::endl;

    return cnv_->getGpioValue(busy_, val);
}

bool ConnectorConverter::writeReset(bool val) const
{
    std::cout << __func__ << std::endl;

    return cnv_->setGpioValue(reset_, val);
}

bool ConnectorConverter::writeDc(bool val) const
{
    std::cout << __func__ << std::endl;

    return cnv_->setGpioValue(dc_, val);
}

bool ConnectorConverter::writeData(const uint8_t* data, size_t len) const
{
    std::cout << __func__ << std::endl;

    std::vector<uint8_t> vec(data, data + len);

    return cnv_->writeData(vec);
}


Display::Display(std::shared_ptr<Connector> con) : con_(std::move(con))
{
    std::cout << __func__ << std::endl;
}

Display::~Display()
{
    std::cout << __func__ << std::endl;
}

bool Display::init() const
{
    std::cout << __func__ << std::endl;

    bool status;

    status = reset();
    if (!status)
        return false;

    status = sendCommand(CMD_POWER_SETTING);
    if (!status)
        return false;
    status = sendData({0x03, 0x00, 0x2B, 0x2B});
    if (!status)
        return false;

    return true;
}

bool Display::sleep() const
{
    std::cout << __func__ << std::endl;

    return true;
}

bool Display::clear() const
{
    std::cout << __func__ << std::endl;

    return true;
}

bool Display::reset() const
{
    std::cout << __func__ << std::endl;

    bool status;

    // 3 times?
    for (int i = 0; i < 3; i++)
    {
        status = con_->writeReset(false);
        if (!status)
            return false;
        delay(10);

        status = con_->writeReset(true);
        if (!status)
            return false;
        delay(10);
    }

    return true;
}

bool Display::sendCommand(uint8_t data) const
{
    std::cout << __func__ << std::endl;

    bool status;

    status = con_->writeDc(false);
    if (!status)
        return false;

    status = con_->writeData(&data, 1);
    if (!status)
        return false;

    return true;
}

bool Display::sendData(const uint8_t* data, size_t len) const
{
    std::cout << __func__ << std::endl;

    bool status;

    status = con_->writeDc(true);
    if (!status)
        return false;

    status = con_->writeData(data, len);
    if (!status)
        return false;

    return true;
}

bool Display::sendData(const std::vector<uint8_t>& data) const
{
    std::cout << __func__ << std::endl;

    bool status;

    status = con_->writeDc(true);
    if (!status)
        return false;

    status = con_->writeData(data.data(), data.size());
    if (!status)
        return false;

    return true;
}

bool Display::waitForBusy() const
{
    Logger logger(__func__);
    bool value, status;

    // todo: add timeout
    for (;;)
    {
        /**
         * low: busy
         * high: idle
         */
        status = con_->readBusy(value);
        if (!status)
            return false;
        if (value)
            return true;

        delay(10);
    }
}

bool Display::turnOn() const
{
    std::cout << __func__ << std::endl;

    bool status;

    status = sendCommand(CMD_DISPLAY_REFRESH);
    if (!status)
        return false;

    delay(100);

    status = waitForBusy();
    if (!status)
        return false;

    return true;
}


Display2Colors::Display2Colors(std::shared_ptr<Connector> con)
    : Display(std::move(con))
{
    std::cout << __func__ << std::endl;
}

bool Display2Colors::show(Image& image) const
{
    std::cout << __func__ << std::endl;

    return true;
}

bool Display2Colors::setLut() const
{
    std::cout << __func__ << std::endl;

    return true;
}


Display4Colors::Display4Colors(std::shared_ptr<Connector> con)
    : Display(std::move(con))
{
    std::cout << __func__ << std::endl;
}

bool Display4Colors::show(Image& image) const
{
    std::cout << __func__ << std::endl;

    return true;
}

bool Display4Colors::setLut() const
{
    std::cout << __func__ << std::endl;

    return true;
}
