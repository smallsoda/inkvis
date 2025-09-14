/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 */

#include <iostream>
#include <cstring>

#include <getopt.h>

#include "converter.h"
#include "display.h"

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

    /* display test */
    display_init(converter.get());
    EPD_4IN2_Init_Fast();
    EPD_4IN2_Clear();

    uint8_t image[300 * 400 / 8];
    std::memset(image, 0xFE, sizeof image);
    EPD_4IN2_Display(image);
    EPD_4IN2_Sleep();

    std::cout << "exit <<" << std::endl;
}
