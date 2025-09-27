/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 * 
 * Source: waveshareteam
 * https://github.com/waveshareteam/e-Paper
 */

#include "display.h"

#include <iostream>
#include <thread>
#include <chrono>

#define DISPLAY_BPP 8
#define DISPLAY_BUF_SIZE \
    ((DISPLAY_WIDTH * DISPLAY_HEIGHT + (DISPLAY_BPP - 1)) / DISPLAY_BPP)

#define CMD_PANEL_SETTING                  0x00
#define CMD_POWER_SETTING                  0x01
#define CMD_POWER_OFF                      0x02
#define CMD_POWER_ON                       0x04
#define CMD_BOOSTER_SOFT_START             0x06
#define CMD_DEEP_SLEEP                     0x07
#define CMD_DATA_START_TRANSMISSION_1      0x10
#define CMD_DISPLAY_REFRESH                0x12
#define CMD_DATA_START_TRANSMISSION_2      0x13
#define CMD_VCOM_LUT                       0x20
#define CMD_W2W_LUT                        0x21
#define CMD_B2W_LUT                        0x22
#define CMD_W2B_LUT                        0x23
#define CMD_B2B_LUT                        0x24
#define CMD_UNKNOWN_LUT                    0x25
#define CMD_PLL_CONTROL                    0x30
#define CMD_VCOM_AND_DATA_INTERVAL_SETTING 0x50
#define CMD_RESOLUTION_SETTING             0x61
#define CMD_VCM_DC_SETTING                 0x82

struct Lut2Colors
{
    unsigned char vcom[36];
    unsigned char ww[36];
    unsigned char bw[36];
    unsigned char wb[36];
    unsigned char bb[36];
};

struct Lut4Colors
{
    unsigned char vcom[42];
    unsigned char ww[42];
    unsigned char bw[42];
    unsigned char wb[42];
    unsigned char bb[42];
};

Lut2Colors lut2 = {
    {0x00, 0x08, 0x08, 0x00, 0x00, 0x02, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x01,
     0x00, 0x08, 0x08, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x50, 0x08, 0x08, 0x00, 0x00, 0x02, 0x90, 0x0F, 0x0F, 0x00, 0x00, 0x01,
     0xA0, 0x08, 0x08, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x50, 0x08, 0x08, 0x00, 0x00, 0x02, 0x90, 0x0F, 0x0F, 0x00, 0x00, 0x01,
     0xA0, 0x08, 0x08, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0xA0, 0x08, 0x08, 0x00, 0x00, 0x02, 0x90, 0x0F, 0x0F, 0x00, 0x00, 0x01,
     0x50, 0x08, 0x08, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x20, 0x08, 0x08, 0x00, 0x00, 0x02, 0x90, 0x0F, 0x0F, 0x00, 0x00, 0x01,
     0x10, 0x08, 0x08, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

Lut4Colors lut4 = {
    {0x00, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x60, 0x14, 0x14, 0x00, 0x00, 0x01,
     0x00, 0x14, 0x00, 0x00, 0x00, 0x01, 0x00, 0x13, 0x0A, 0x01, 0x00, 0x01,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x40, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
     0x10, 0x14, 0x0A, 0x00, 0x00, 0x01, 0xA0, 0x13, 0x01, 0x00, 0x00, 0x01,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x40, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
     0x00, 0x14, 0x0A, 0x00, 0x00, 0x01, 0x99, 0x0C, 0x01, 0x03, 0x04, 0x01,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x40, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
     0x00, 0x14, 0x0A, 0x00, 0x00, 0x01, 0x99, 0x0B, 0x04, 0x04, 0x01, 0x01,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x80, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
     0x20, 0x14, 0x0A, 0x00, 0x00, 0x01, 0x50, 0x13, 0x01, 0x00, 0x00, 0x01,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};


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

bool ConnectorConverter::readBusy(bool& val) const
{
    bool status = cnv_->getGpioValue(busy_, val);
    if (!status)
        std::cerr << "converter: error: " << __func__ << std::endl;
    return status;
}

bool ConnectorConverter::writeReset(bool val) const
{
    bool status = cnv_->setGpioValue(reset_, val);
    if (!status)
        std::cerr << "converter: error: " << __func__ << std::endl;
    return status;
}

bool ConnectorConverter::writeDc(bool val) const
{
    bool status = cnv_->setGpioValue(dc_, val);
    if (!status)
        std::cerr << "converter: error: " << __func__ << std::endl;
    return status;
}

bool ConnectorConverter::writeData(const uint8_t* data, size_t len) const
{
    std::vector<uint8_t> vec(data, data + len);
    bool status = cnv_->writeData(vec);
    if (!status)
        std::cerr << "converter: error: " << __func__ << std::endl;
    return status;
}


Display::Display(std::shared_ptr<Connector> con) : con_(std::move(con)) {}

bool Display::init() const
{
    bool status;

    status = reset();
    if (!status)
        return false;

    status = sendCommand(CMD_POWER_SETTING);
    if (!status)
        return false;
    status = sendData(getSettingPower());
    if (!status)
        return false;

    status = sendCommand(CMD_BOOSTER_SOFT_START);
    if (!status)
        return false;
    status = sendData({0x17, 0x17, 0x17}); /* A B C */
    if (!status)
        return false;

    status = sendCommand(CMD_POWER_ON);
    if (!status)
        return false;
    status = waitForBusy();
    if (!status)
        return false;

    status = sendCommand(CMD_PANEL_SETTING);
    if (!status)
        return false;
    status = sendData(getSettingPanel());
    if (!status)
        return false;

    status = sendCommand(CMD_PLL_CONTROL);
    if (!status)
        return false;
    status = sendData({0x3C}); /* 3A 100HZ, 29 150Hz, 39 200HZ, 31 171HZ */
    if (!status)
        return false;

    status = sendCommand(CMD_RESOLUTION_SETTING);
    if (!status)
        return false;
    status = sendData({0x01, 0x90, 0x01, 0x2C}); /* 400x300 */
    if (!status)
        return false;

    status = sendCommand(CMD_VCM_DC_SETTING);
    if (!status)
        return false;
    status = sendData({0x12});
    if (!status)
        return false;

    status = sendCommand(CMD_VCOM_AND_DATA_INTERVAL_SETTING);
    if (!status)
        return false;
    status = sendData({0x17}); /* {0x97} */
    if (!status)
        return false;

    status = setLut();
    if (!status)
        return false;

    return true;
}

bool Display::sleep() const
{
    bool status;

    status = sendCommand(CMD_VCOM_AND_DATA_INTERVAL_SETTING); /* Deep sleep? */
    if (!status)
        return false;
    status = sendData({0xF7});
    if (!status)
        return false;

    status = sendCommand(CMD_POWER_OFF);
    if (!status)
        return false;
    status = waitForBusy();
    if (!status)
        return false;

    status = sendCommand(CMD_DEEP_SLEEP);
    if (!status)
        return false;
    status = sendData({0xA5}); /* Deep sleep code */
    if (!status)
        return false;

    return true;
}

bool Display::clear() const
{
    std::vector<uint8_t> buf(DISPLAY_BUF_SIZE, 0xFF);
    bool status;

    status = sendCommand(CMD_DATA_START_TRANSMISSION_1);
    if (!status)
        return false;
    status = sendData(buf);
    if (!status)
        return false;

    status = sendCommand(CMD_DATA_START_TRANSMISSION_2);
    if (!status)
        return false;
    status = sendData(buf);
    if (!status)
        return false;

    /* ? */
    status = sendCommand(CMD_DISPLAY_REFRESH);
    if (!status)
        return false;
    delay(10);

    status = turnOn();
    if (!status)
        return false;

    return true;
}

bool Display::reset() const
{
    bool status;

    /* 3 times? */
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
    return sendData(data.data(), data.size());
}

bool Display::waitForBusy() const
{
    Logger logger(std::string("display: ") + __func__);
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
    : Display(std::move(con)) {}

bool Display2Colors::show(Image& image) const
{
    std::vector<uint8_t> buf(DISPLAY_BUF_SIZE, 0x00);
    bool status;
    int height;
    int width;

    image.getSize(width, height);

    if (height != DISPLAY_HEIGHT || width != DISPLAY_WIDTH)
    {
        std::cout << "display: resize image: " << width << "x" << height
            << " > " << DISPLAY_WIDTH << "x" << DISPLAY_HEIGHT << std::endl;

        status = image.resize(DISPLAY_WIDTH, DISPLAY_HEIGHT);
        if (!status)
            return false;
    }

    status = image.toGreyscale();
    if (!status)
        return false;

    status = sendCommand(CMD_DATA_START_TRANSMISSION_1);
    if (!status)
        return false;
    status = sendData(buf);
    if (!status)
        return false;

    status = image.compress2Colors(buf.data());
    if (!status)
        return false;

    status = sendCommand(CMD_DATA_START_TRANSMISSION_2);
    if (!status)
        return false;
    status = sendData(buf);
    if (!status)
        return false;

    /* ? */
    status = sendCommand(CMD_DISPLAY_REFRESH);
    if (!status)
        return false;
    delay(10);

    status = turnOn();
    if (!status)
        return false;

    return true;
}

bool Display2Colors::setLut() const
{
    bool status;

    status = sendCommand(CMD_VCOM_LUT);
    if (!status)
        return false;
    status = sendData(lut2.vcom, sizeof lut2.vcom);
    if (!status)
        return false;

    status = sendCommand(CMD_W2W_LUT);
    if (!status)
        return false;
    status = sendData(lut2.ww, sizeof lut2.ww);
    if (!status)
        return false;

    status = sendCommand(CMD_B2W_LUT);
    if (!status)
        return false;
    status = sendData(lut2.bw, sizeof lut2.bw);
    if (!status)
        return false;

    status = sendCommand(CMD_W2B_LUT);
    if (!status)
        return false;
    status = sendData(lut2.wb, sizeof lut2.wb);
    if (!status)
        return false;

    status = sendCommand(CMD_B2B_LUT);
    if (!status)
        return false;
    status = sendData(lut2.bb, sizeof lut2.bb);
    if (!status)
        return false;

    return true;
}

std::vector<uint8_t> Display2Colors::getSettingPower() const
{
    return {0x03, 0x00, 0x2B, 0x2B};
}

std::vector<uint8_t> Display2Colors::getSettingPanel() const
{
    return {0xBF}; /* ? */ /* KW-bf KWR-2F BWROTP 0f BWOTP 1f */
}


Display4Colors::Display4Colors(std::shared_ptr<Connector> con)
    : Display(std::move(con)) {}

bool Display4Colors::show(Image& image) const
{
    std::vector<uint8_t> buf10(DISPLAY_BUF_SIZE);
    std::vector<uint8_t> buf13(DISPLAY_BUF_SIZE);
    bool status;
    int height;
    int width;

    image.getSize(width, height);

    if (height != DISPLAY_HEIGHT || width != DISPLAY_WIDTH)
    {
        std::cout << "display: resize image: " << width << "x" << height
            << " > " << DISPLAY_WIDTH << "x" << DISPLAY_HEIGHT << std::endl;

        status = image.resize(DISPLAY_WIDTH, DISPLAY_HEIGHT);
        if (!status)
            return false;
    }

    status = image.toGreyscale();
    if (!status)
        return false;

    status = image.compress4Colors(buf10.data(), buf13.data());
    if (!status)
        return false;

    status = sendCommand(CMD_DATA_START_TRANSMISSION_1);
    if (!status)
        return false;
    status = sendData(buf10);
    if (!status)
        return false;

    status = sendCommand(CMD_DATA_START_TRANSMISSION_2);
    if (!status)
        return false;
    status = sendData(buf13);
    if (!status)
        return false;

    /* ? */
    status = sendCommand(CMD_DISPLAY_REFRESH);
    if (!status)
        return false;
    delay(10);

    status = turnOn();
    if (!status)
        return false;

    return true;
}

bool Display4Colors::setLut() const
{
    bool status;

    status = sendCommand(CMD_VCOM_LUT); /* vcom */
    if (!status)
        return false;
    status = sendData(lut4.vcom, sizeof lut4.vcom);
    if (!status)
        return false;

    status = sendCommand(CMD_W2W_LUT); /* red not use */
    if (!status)
        return false;
    status = sendData(lut4.ww, sizeof lut4.ww);
    if (!status)
        return false;

    status = sendCommand(CMD_B2W_LUT); /* bw r */
    if (!status)
        return false;
    status = sendData(lut4.bw, sizeof lut4.bw);
    if (!status)
        return false;

    status = sendCommand(CMD_W2B_LUT); /* wb w */
    if (!status)
        return false;
    status = sendData(lut4.wb, sizeof lut4.wb);
    if (!status)
        return false;

    status = sendCommand(CMD_B2B_LUT); /* bb b */
    if (!status)
        return false;
    status = sendData(lut4.bb, sizeof lut4.bb);
    if (!status)
        return false;

    status = sendCommand(CMD_UNKNOWN_LUT); /* vcom? */
    if (!status)
        return false;
    status = sendData(lut4.ww, sizeof lut4.ww);
    if (!status)
        return false;

    return true;
}

std::vector<uint8_t> Display4Colors::getSettingPower() const
{
    return {0x03, 0x00, 0x2B, 0x2B, 0x13};
}

std::vector<uint8_t> Display4Colors::getSettingPanel() const
{
    return {0x3F}; /* KW-3f KWR-2F BWROTP 0f BWOTP 1f */
}
