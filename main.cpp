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

#define VERSION "0.2.1"

// todo
#define CONV_I2C_BUS         0
#define CONV_I2C_ADDRESS     0x51
#define CONV_GPIO_BUSY_DEV   0
#define CONV_GPIO_BUSY_LINE  0
#define CONV_GPIO_MODE_DEV   0
#define CONV_GPIO_MODE_LINE  1
#define CONV_GPIO_RESET_DEV  0
#define CONV_GPIO_RESET_LINE 2

#define GPIO_RESET Converter::GpioNum::GPIO2
#define GPIO_BUSY  Converter::GpioNum::GPIO1
#define GPIO_DC    Converter::GpioNum::GPIO3


struct Params
{
    std::string pic;
    std::string outpic;
    bool bw;
    bool resetConv;
    bool resetCount;
    bool showCount;
};


int main(int argc, char* argv[])
{
    /* default parameters */
    Params params = { "", "", false, false, false, false };

    const struct option lopts[] = {
        { "picture",          required_argument, NULL, 'p' },
        { "save-picture",     required_argument, NULL, 's' },
        { "colors-bw",        no_argument,       NULL, 'b' },
        { "conv-reset",       no_argument,       NULL, 'r' },
        { "conv-count-reset", no_argument,       NULL, 't' },
        { "conv-count-show",  no_argument,       NULL, 'c' },
        { "version",          no_argument,       NULL, 'v' },
        { "help",             no_argument,       NULL, 'h' },
        { NULL, 0, NULL, 0 },
    };
    int opt, idx;

    while ((opt = getopt_long(argc, argv, "p:s:brtcvh", lopts, &idx)) != -1)
    {
        switch (opt)
        {
        case 'p':
            params.pic = optarg;
            break;
        case 's':
            params.outpic = optarg;
            break;
        case 'b':
            params.bw = true;
            break;
        case 'r':
            params.resetConv = true;
            break;
        case 't':
            params.resetCount = true;
            break;
        case 'c':
            params.showCount = true;
            break;
        case 'v':
            std::cout << VERSION << std::endl;
            return 0;
        case 'h':
        case '?':
        default:
            std::cout << "Usage: " << argv[0]
                << " [-p FILE] [-s FILE] [-b] [-r] [-t] [-c] [-v] [-h]"
                << std::endl;
            std::cout << std::endl;
            std::cout << "-p, --picture             image file to show"
                << std::endl;
            std::cout << "-s, --save-picture        image file to save "
                << "after resizing" << std::endl;
            std::cout << "-b, --colors-bw           black and white mode"
                << std::endl;
            std::cout << "-r, --conv-reset          reset converter"
                << std::endl;
            std::cout << "-t, --conv-count-reset    reset converter counters"
                << std::endl;
            std::cout << "-c, --conv-count-show     show converter counters"
                << std::endl;
            std::cout << "-v, --version             version" << std::endl;
            std::cout << "-h, --help                help" << std::endl;
            return 0;
        }
    }

    /* save picture */
    if (!params.outpic.empty())
    {
        std::cout << "> resize and save picture" << std::endl;

        if (params.pic.empty())
        {
            std::cerr << "can not save: no input picture" << std::endl;
            return -1;
        }

        bool status;
        Image image(params.pic);

        status = image.resize(DISPLAY_WIDTH, DISPLAY_HEIGHT);
        if (!status)
        {
            std::cerr << "can not resize picture" << std::endl;
            return -1;
        }
        status = image.save(params.outpic);
        if (!status)
        {
            std::cerr << "can not save picture" << std::endl;
            return -1;
        }

        return 0;
    }

    /* converter: builder */
    // auto converter = Converter::Builder()
    //     .buildI2cBus(CONV_I2C_BUS, CONV_I2C_ADDRESS)
    //     .buildGpioBusy(CONV_GPIO_BUSY_DEV, CONV_GPIO_BUSY_LINE)
    //     .buildGpioMode(CONV_GPIO_MODE_DEV, CONV_GPIO_MODE_LINE)
    //     .buildGpioReset(CONV_GPIO_RESET_DEV, CONV_GPIO_RESET_LINE)
    //     .build();

    /* converter: manual */
    auto bus = std::make_unique<I2c>(CONV_I2C_BUS, CONV_I2C_ADDRESS);
    auto busy = std::make_unique<GpioIn>("busy", CONV_GPIO_BUSY_DEV,
        CONV_GPIO_BUSY_LINE);
    auto mode = std::make_shared<GpioOut>("mode", CONV_GPIO_MODE_DEV,
        CONV_GPIO_MODE_LINE, true);
    auto reset = std::make_unique<GpioOut>("reset", CONV_GPIO_RESET_DEV,
        CONV_GPIO_RESET_LINE, true);

    auto converter = std::make_shared<Converter>(std::move(bus),
        std::move(busy), std::move(mode), std::move(reset));

    /* reset converter */
    if (params.resetConv)
    {
        std::cout << "> reset converter" << std::endl;
        bool status;

        status = converter->reset();
        if (!status)
        {
            std::cerr << "can not reset converter" << std::endl;
            return -1;
        }
    }

    /* reset converter counters */
    if (params.resetCount)
    {
        std::cout << "> reset converter counters" << std::endl;
        bool status;

        status = converter->resetCounter(Converter::Counter::RX);
        if (!status)
        {
            std::cerr << "can not reset converter counter RX" << std::endl;
            return -1;
        }
        status = converter->resetCounter(Converter::Counter::TX);
        if (!status)
        {
            std::cerr << "can not reset converter counter TX" << std::endl;
            return -1;
        }
    }

    /* show converter counters */
    if (params.showCount)
    {
        unsigned long rx, tx;
        bool status;

        status = converter->getCounter(Converter::Counter::RX, rx);
        if (!status)
        {
            std::cerr << "can not get converter counter RX" << std::endl;
            return -1;
        }
        status = converter->getCounter(Converter::Counter::TX, tx);
        if (!status)
        {
            std::cerr << "can not get converter counter TX" << std::endl;
            return -1;
        }

        std::cout << "> rx counter: " << rx << std::endl;
        std::cout << "> tx counter: " << tx << std::endl;
    }

    /* display */
    auto con_over_cnv = std::make_shared<ConnectorConverter>(converter,
        GPIO_RESET, GPIO_BUSY, GPIO_DC);
    auto connector = std::static_pointer_cast<Connector>(con_over_cnv);

    std::shared_ptr<Display> display;
    if (params.bw)
    {
        auto d2c = std::make_shared<Display2Colors>(std::move(connector));
        display = std::static_pointer_cast<Display>(d2c);
    }
    else
    {
        auto d4c = std::make_shared<Display4Colors>(std::move(connector));
        display = std::static_pointer_cast<Display>(d4c);
    }

    std::cout << "> display: initialization" << std::endl;
    display->init();

    if (params.pic.empty())
    {
        std::cout << "> clear display" << std::endl;
        display->clear();
    }
    else
    {
        std::cout << "> show picture" << std::endl;
        Image image(params.pic);
        display->show(image);
    }

    std::cout << "> display: sleep" << std::endl;
    display->sleep();
}
