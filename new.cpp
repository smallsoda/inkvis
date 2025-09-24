/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 */

#include <iostream>

#include "new-display.h"

#define GPIO_RESET Converter::GpioNum::GPIO2
#define GPIO_BUSY  Converter::GpioNum::GPIO1
#define GPIO_DC    Converter::GpioNum::GPIO3


int main(int argc, char* argv[])
{
    std::cout << "main >>" << std::endl;

    auto converter = std::make_shared<Converter>();
    auto con_cnv = std::make_shared<ConnectorConverter>(converter, GPIO_RESET,
        GPIO_BUSY, GPIO_DC);
    auto connector = std::static_pointer_cast<Connector>(con_cnv);

    Display2Colors display(std::move(connector));

    std::cout << "exit <<" << std::endl;
}
