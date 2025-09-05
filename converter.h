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
    Gpio(std::string const& gpiochip, int line);
    Gpio(const Gpio&) = delete;
    Gpio& operator=(const Gpio&) = delete;
    ~Gpio();

protected:
    gpiod::chip chip_;

private:
    const int line_;
};


class GpioIn : public Gpio
{
public:
    GpioIn(std::string const& gpiochip, int line);
    bool get(int& val) const;
};


class GpioOut : public Gpio
{
public:
    GpioOut(std::string const& gpiochip, int line);
    bool set(int val) const;
};


class I2c
{
public:
    I2c(int dev, int address);
    I2c(const I2c&) = delete;
    I2c& operator=(const I2c&) = delete;
    ~I2c();

    bool read(std::vector<uint8_t>& buf) const;
    bool write(std::vector<uint8_t> const& buf) const;
    
    bool readReg8(uint8_t reg, uint8_t& val) const;
    bool writeReg8(uint8_t reg, uint8_t val) const;

    bool readReg32(uint8_t reg, uint32_t& val) const;
    bool writeReg32(uint8_t reg, uint32_t val) const;

private:
    int fd_;
    const int address_;
};


class Converter
{
public:
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

    enum class Counter { RX, TX };
    enum class GpioNum { GPIO1, GPIO2, GPIO3 };
    enum class GpioMode { IN_FL, IN_PU, OUT_PP, OUT_OD };
    enum class SpiMode { MODE0, MODE1, MODE2, MODE3 };
    enum class SpiBaudrate { BR2, BR4, BR8, BR16, BR32, BR64, BR128, BR256 };

    Converter(std::unique_ptr<I2c> bus, std::unique_ptr<GpioIn> busy,
        std::shared_ptr<GpioOut> mode, std::unique_ptr<GpioOut> reset);
    Converter();
    ~Converter();

    bool read(std::vector<uint8_t>& buf) const;
    bool write(std::vector<uint8_t> const& buf) const;

    bool getCounter(Counter counter, unsigned long& val) const;
    bool resetCounter(Counter counter) const;

    bool getGpioValue(GpioNum num, bool& value) const;
    bool setGpioValue(GpioNum num, bool value) const;
    bool setGpioMode(GpioNum num, GpioMode mode) const;
    bool setSpiMode(SpiMode mode) const;
    bool setSpiBaudrate(SpiBaudrate baudrate) const;

    bool reset() const;

private:
    std::unique_ptr<I2c> i2cBus_;
    std::unique_ptr<GpioIn> gpioBusy_;
    std::shared_ptr<GpioOut> gpioMode_;
    std::unique_ptr<GpioOut> gpioReset_;
};

#endif /* CONVERTER_H_ */
