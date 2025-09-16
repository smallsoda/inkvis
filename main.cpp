/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 */

#include <iostream>
#include <cstring>

#include <getopt.h>

#include "converter.h"
#include "display.h"
#include "image.h"

#define VERSION "0.1"


int main(int argc, char* argv[])
{
    std::cout << "main >>" << std::endl;

    /* parameters */
    bool param_reset = false;
    std::string picture;

    const struct option lopts[] = {
        { "picture",   required_argument, NULL, 'p' },
        { "reset-cnv", no_argument,       NULL, 'r' },
        { "version",   no_argument,       NULL, 'v' },
        { "help",      no_argument,       NULL, 'h' },
        { NULL, 0, NULL, 0 },
    };
    int opt, idx;

    while ((opt = getopt_long(argc, argv, "p:rvh", lopts, &idx)) != -1)
    {
        switch (opt)
        {
        case 'p':
            picture = optarg;
            break;
        case 'r':
            param_reset = true;
            break;
        case 'v':
            std::cout << VERSION << std::endl;
            return 0;
        case 'h':
        case '?':
        default:
            // TODO
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

    uint8_t* buf10 = new uint8_t[400 * 300 / 8];
    uint8_t* buf13 = new uint8_t[400 * 300 / 8];

    if (picture.empty())
    {
        std::memset(buf10, 0x00, 400 * 300 / 8);
        std::memset(buf13, 0xFF, 400 * 300 / 8);
    }
    else
    {
        std::cout << "image: processing" << std::endl;

        std::cout << "image: processing: open" << std::endl;
        Image image(picture);
        std::cout << "image: processing: resize" << std::endl;
        image.resize(400, 300);
        std::cout << "image: processing: toGreyscale" << std::endl;
        image.toGreyscale();
        // image.compress2Colors(buf13);
        std::cout << "image: processing: compress4Colors" << std::endl;
        image.compress4Colors(buf10, buf13);

        std::cout << "image: processing: done" << std::endl;
    }

    // EPD_4IN2_Display(buf13);
    EPD_4IN2_4GrayDisplay(buf10, buf13);
    EPD_4IN2_Sleep();

    delete[] buf10;
    delete[] buf13;

    std::cout << "exit <<" << std::endl;
}
