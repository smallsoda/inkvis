/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 */

#include "converter.h"

#include <iostream>


int main()
{
    std::cout << "main >>" << std::endl;

    // auto converter = Converter::Builder()
    //     .buildI2cBus(3, 0x48)
    //     .buildGpioBusy("/dev/gpiochip0", 2)
    //     .buildGpioMode("/dev/gpiochip0", 1)
    //     .buildGpioReset("/dev/gpiochip0", 0)
    //     .build();

    // unsigned long val;
    // converter->getCounter(Converter::Counter::RX, val);

    auto bus = std::make_unique<I2c>(3, 0x48);
    auto busy = std::make_unique<GpioIn>("/dev/gpiochip0", 2);
    auto mode = std::make_shared<GpioOut>("/dev/gpiochip0", 1);
    auto reset = std::make_unique<GpioOut>("/dev/gpiochip0", 0);

    Converter converter(std::move(bus), std::move(busy), std::move(mode),
        std::move(reset));

    // Converter converter(std::move(bus), std::move(busy), nullptr,
    //     std::move(reset));

    // Converter converter(std::make_unique<I2c>(3, 0x48),
    //     std::make_unique<GpioIn>("/dev/gpiochip0", 2),
    //     std::make_shared<GpioOut>("/dev/gpiochip0", 1),
    //     std::make_unique<GpioOut>("/dev/gpiochip0", 0));

    unsigned long val;
    converter.getCounter(Converter::Counter::RX, val);

    std::cout << "exit <<" << std::endl;
}
