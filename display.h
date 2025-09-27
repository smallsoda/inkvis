/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 * 
 * Source: waveshareteam
 * https://github.com/waveshareteam/e-Paper
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "converter.h"
#include "image.h"

#include <iostream>
#include <vector>
#include <memory>

#define DISPLAY_WIDTH  400
#define DISPLAY_HEIGHT 300


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

    bool init() const;
    bool sleep() const;
    bool clear() const; /* ? */
    virtual bool show(Image& image) const = 0;

protected:
    bool reset() const;
    bool sendCommand(uint8_t data) const;
    bool sendData(const uint8_t* data, size_t len) const;
    bool sendData(const std::vector<uint8_t>& data) const;
    bool waitForBusy() const;
    bool turnOn() const;
    virtual bool setLut() const = 0;
    virtual std::vector<uint8_t> getSettingPower() const = 0;
    virtual std::vector<uint8_t> getSettingPanel() const = 0;

    std::shared_ptr<Connector> con_;
};

class Display2Colors : public Display
{
public:
    Display2Colors(std::shared_ptr<Connector> con);
    bool show(Image& image) const override;

private:
    bool setLut() const override;
    std::vector<uint8_t> getSettingPower() const override;
    std::vector<uint8_t> getSettingPanel() const override;
};

class Display4Colors : public Display
{
public:
    Display4Colors(std::shared_ptr<Connector> con);
    bool show(Image& image) const override;

private:
    bool setLut() const override;
    std::vector<uint8_t> getSettingPower() const override;
    std::vector<uint8_t> getSettingPanel() const override;
};

#endif /* DISPLAY_H_ */
