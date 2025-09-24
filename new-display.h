/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 * 
 * Source: waveshareteam
 * https://github.com/waveshareteam/e-Paper
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

/* todo: uncomment */
// #include "converter.h"

#include <iostream>
#include <vector>
#include <memory>


/* todo: remove */
class Image
{
    Image()
    {
        std::cout << "Image::" << __func__ << std::endl;
    }

    ~Image()
    {
        std::cout << "Image::" << __func__ << std::endl;
    }
};

class Converter
{
public:
    enum class GpioNum { GPIO1, GPIO2, GPIO3 };
    enum class GpioMode { IN_FL, IN_PU, OUT_PP, OUT_OD };
    enum class SpiMode { MODE0, MODE1, MODE2, MODE3 };
    enum class SpiBaudrate { BR2, BR4, BR8, BR16, BR32, BR64, BR128, BR256 };

    Converter()
    {
        std::cout << "Converter::" << __func__ << std::endl;
    }

    ~Converter()
    {
        std::cout << "Converter::" << __func__ << std::endl;
    }

    bool writeData(const std::vector<uint8_t>& buf) const
    {
        std::cout << "Converter::" << __func__ << std::endl;

        return true;
    }

    bool getGpioValue(GpioNum num, bool& val) const
    {
        std::cout << "Converter::" << __func__ << std::endl;

        return true;
    }

    bool setGpioValue(GpioNum num, bool val) const
    {
        std::cout << "Converter::" << __func__ << std::endl;

        return true;
    }

    bool setGpioMode(GpioNum num, GpioMode mode) const
    {
        std::cout << "Converter::" << __func__ << std::endl;

        return true;
    }

    bool setSpiMode(SpiMode mode) const
    {
        std::cout << "Converter::" << __func__ << std::endl;

        return true;
    }

    bool setSpiBaudrate(SpiBaudrate baudrate) const
    {
        std::cout << "Converter::" << __func__ << std::endl;

        return true;
    }
};


/* display interface */
class Connector
{
public:
    virtual bool readBusy(bool& val) const = 0;
    virtual bool writeReset(bool val) const = 0;
    virtual bool writeDc(bool val) const = 0;
    
    virtual bool writeData(const uint8_t* data, size_t len) const = 0;
};

class ConnectorConverter : public Connector
{
public:
    ConnectorConverter(std::shared_ptr<Converter> cnv, Converter::GpioNum reset,
        Converter::GpioNum busy, Converter::GpioNum dc);
    ~ConnectorConverter();

    bool readBusy(bool& val) const override;
    bool writeReset(bool val) const override;
    bool writeDc(bool val) const override;
    
    bool writeData(const uint8_t* data, size_t len) const override;

private:
    std::shared_ptr<Converter> cnv_;
    Converter::GpioNum reset_;
    Converter::GpioNum busy_;
    Converter::GpioNum dc_;
};

/* display */
class Display
{
public:
    Display(std::shared_ptr<Connector> con);
    ~Display();

    bool init() const;
    bool sleep() const;
    bool clear() const; // ?
    virtual bool show(Image& image) const = 0;

protected:
    bool reset() const;
    bool sendCommand(uint8_t data) const;
    bool sendData(const uint8_t* data, size_t len) const;
    bool waitForBusy() const;
    bool turnOn() const;
    virtual bool setLut() const = 0;

    std::shared_ptr<Connector> con_;
};

class Display2Colors : public Display
{
public:
    Display2Colors(std::shared_ptr<Connector> con);
    bool show(Image& image) const override;

private:
    bool setLut() const override;
};

class Display4Colors : public Display
{
public:
    Display4Colors(std::shared_ptr<Connector> con);
    bool show(Image& image) const override;

private:
    bool setLut() const override;
};

#endif /* DISPLAY_H_ */
