/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 */

#include "converter.h"

#include <iostream>


int main(int argc, char* argv[])
{
    std::cout << "main >>" << std::endl;

    // auto converter = Converter::Builder()
    //     .buildI2cBus(0, 0x51)
    //     .buildGpioBusy(0, 0)
    //     .buildGpioMode(0, 1)
    //     .buildGpioReset(0, 2)
    //     .build();

    // unsigned long val;
    // converter->getCounter(Converter::Counter::RX, val);

    auto bus = std::make_unique<I2c>(0, 0x51);
    auto busy = std::make_unique<GpioIn>("busy", 0, 0);
    auto mode = std::make_shared<GpioOut>("mode", 0, 1, true);
    auto reset = std::make_unique<GpioOut>("reset", 0, 2, true);

    Converter converter(std::move(bus), std::move(busy), std::move(mode),
        std::move(reset));
    // Converter converter(std::move(bus), std::move(busy), nullptr,
    //     std::move(reset));

    if (argc > 1)
        converter.reset();

    unsigned long rx, tx;

    converter.getCounter(Converter::Counter::RX, rx);
    converter.getCounter(Converter::Counter::TX, tx);

    std::cout << "rx counter: " << rx << std::endl;
    std::cout << "tx counter: " << tx << std::endl;

    std::cout << "exit <<" << std::endl;
}
