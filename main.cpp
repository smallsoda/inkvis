/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 */

#include "converter.h"

#include <iostream>

#include <thread>
#include <chrono>

#include <getopt.h>

#define VERSION "0.1"


int main(int argc, char* argv[])
{
    std::cout << "main >>" << std::endl;

    /* parameters */
    bool param_reset = false;

    const struct option lopts[] = {
        { "reset-cnv", no_argument, NULL, 'r' },
        { "version",   no_argument, NULL, 'v' },
        { "help",      no_argument, NULL, 'h' },
        { NULL, 0, NULL, 0 },
    };
    int opt, idx;

    while ((opt = getopt_long(argc, argv, "rvh", lopts, &idx)) != -1)
    {
        switch (opt)
        {
        case 'r':
            param_reset = true;
            break;
        case 'v':
            std::cout << VERSION << std::endl;
            return 0;
        case 'h':
        case '?':
        default:
            std::cout << "Usage: " << argv[0] << std::endl;
            return 0;
        }
    }

    /* builder */
    // auto converter = Converter::Builder()
    //     .buildI2cBus(0, 0x51)
    //     .buildGpioBusy(0, 0)
    //     .buildGpioMode(0, 1)
    //     .buildGpioReset(0, 2)
    //     .build();

    /* manual */
    auto bus = std::make_unique<I2c>(0, 0x51);
    auto busy = std::make_unique<GpioIn>("busy", 0, 0);
    auto mode = std::make_shared<GpioOut>("mode", 0, 1, true);
    auto reset = std::make_unique<GpioOut>("reset", 0, 2, true);

    auto converter = std::make_unique<Converter>(std::move(bus),
        std::move(busy), std::move(mode), std::move(reset));

    /* reset */
    if (param_reset)
    {
        std::cout << "reset converter" << std::endl;
        converter->reset();
    }

    /* counter test */
    unsigned long rx, tx;

    converter->getCounter(Converter::Counter::RX, rx);
    converter->getCounter(Converter::Counter::TX, tx);

    std::cout << "rx counter: " << rx << std::endl;
    std::cout << "tx counter: " << tx << std::endl;

    /* gpio test */
    converter->setGpioMode(Converter::GpioNum::GPIO1,
        Converter::GpioMode::OUT_PP);
    
    bool value = false;
    for (;;)
    {
        value = value ? false : true;
        converter->setGpioValue(Converter::GpioNum::GPIO1, value);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    std::cout << "exit <<" << std::endl;
}
