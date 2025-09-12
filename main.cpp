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
    //     .buildGpioBusy(0, 1)
    //     .buildGpioMode(0, 3)
    //     .buildGpioReset(0, 4)
    //     .build();

    // unsigned long val;
    // converter->getCounter(Converter::Counter::RX, val);

    auto bus = std::make_unique<I2c>(3, 0x49); // 0x48
    auto busy = std::make_unique<GpioIn>("busy", 0, 1);
    auto mode = std::make_shared<GpioOut>("mode", 0, 3);
    auto reset = std::make_unique<GpioOut>("reset", 0, 4);

    busy->get();

    Converter converter(std::move(bus), std::move(busy), std::move(mode),
        std::move(reset));

    // Converter converter(std::move(bus), std::move(busy), nullptr,
    //     std::move(reset));

    // Converter converter(std::make_unique<I2c>(3, 0x48),
    //     std::make_unique<GpioIn>("busy", 0, 1),
    //     std::make_shared<GpioOut>("mode", 0, 3),
    //     std::make_unique<GpioOut>("reset", 0, 4));

    unsigned long val;
    converter.getCounter(Converter::Counter::RX, val);

    std::cout << "exit <<" << std::endl;
}
