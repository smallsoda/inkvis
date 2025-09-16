/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 * 
 * Source: waveshareteam
 * https://github.com/waveshareteam/e-Paper
 */

#include "display.h"

#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>

#define EPD_4IN2_WIDTH  400
#define EPD_4IN2_HEIGHT 300


static const unsigned char EPD_4IN2_lut_vcom0[] = {
    0x00, 0x08, 0x08, 0x00, 0x00, 0x02,
    0x00, 0x0F, 0x0F, 0x00, 0x00, 0x01,
    0x00, 0x08, 0x08, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00};
static const unsigned char EPD_4IN2_lut_ww[] = {
    0x50, 0x08, 0x08, 0x00, 0x00, 0x02,
    0x90, 0x0F, 0x0F, 0x00, 0x00, 0x01,
    0xA0, 0x08, 0x08, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const unsigned char EPD_4IN2_lut_bw[] = {
    0x50, 0x08, 0x08, 0x00, 0x00, 0x02,
    0x90, 0x0F, 0x0F, 0x00, 0x00, 0x01,
    0xA0, 0x08, 0x08, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const unsigned char EPD_4IN2_lut_wb[] = {
    0xA0, 0x08, 0x08, 0x00, 0x00, 0x02,
    0x90, 0x0F, 0x0F, 0x00, 0x00, 0x01,
    0x50, 0x08, 0x08, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const unsigned char EPD_4IN2_lut_bb[] = {
    0x20, 0x08, 0x08, 0x00, 0x00, 0x02,
    0x90, 0x0F, 0x0F, 0x00, 0x00, 0x01,
    0x10, 0x08, 0x08, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// 0~3 gray
const unsigned char EPD_4IN2_4Gray_lut_vcom[] = {
    0x00, 0x0A, 0x00, 0x00, 0x00, 0x01,
    0x60, 0x14, 0x14, 0x00, 0x00, 0x01,
    0x00, 0x14, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x13, 0x0A, 0x01, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// R21
const unsigned char EPD_4IN2_4Gray_lut_ww[] = {
    0x40, 0x0A, 0x00, 0x00, 0x00, 0x01,
    0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
    0x10, 0x14, 0x0A, 0x00, 0x00, 0x01,
    0xA0, 0x13, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// R22H r
const unsigned char EPD_4IN2_4Gray_lut_bw[] = {
    0x40, 0x0A, 0x00, 0x00, 0x00, 0x01,
    0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
    0x00, 0x14, 0x0A, 0x00, 0x00, 0x01,
    0x99, 0x0C, 0x01, 0x03, 0x04, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// R23H w
const unsigned char EPD_4IN2_4Gray_lut_wb[] = {
    0x40, 0x0A, 0x00, 0x00, 0x00, 0x01,
    0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
    0x00, 0x14, 0x0A, 0x00, 0x00, 0x01,
    0x99, 0x0B, 0x04, 0x04, 0x01, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// R24H b
const unsigned char EPD_4IN2_4Gray_lut_bb[] = {
    0x80, 0x0A, 0x00, 0x00, 0x00, 0x01,
    0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
    0x20, 0x14, 0x0A, 0x00, 0x00, 0x01,
    0x50, 0x13, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static uint8_t framebuffer[EPD_4IN2_WIDTH * EPD_4IN2_HEIGHT / 8];


static Converter* converter;

static void delay(int val)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(val));
}

static int gpio_rd_busy()
{
    bool status, val;

    status = converter->getGpioValue(Converter::GpioNum::GPIO1, val);
    if (!status)
        std::cerr << "display: error: " << __func__ << std::endl;

    return val;
}

static void gpio_wr_reset(int val)
{
    bool status;

    status = converter->setGpioValue(Converter::GpioNum::GPIO2, val);
    if (!status)
        std::cerr << "display: error: " << __func__ << std::endl;
}

static void gpio_wr_dc(int val)
{
    bool status;

    status = converter->setGpioValue(Converter::GpioNum::GPIO3, val);
    if (!status)
        std::cerr << "display: error: " << __func__ << std::endl;
}

static void write_data(uint8_t* data, size_t len)
{
    std::vector<uint8_t> vec(data, data + len);
    bool status;

    status = converter->writeData(vec);
    if (!status)
        std::cerr << "display: error: " << __func__ << std::endl;
}

void display_init(Converter* cnv)
{
    converter = cnv;
    bool status;

    status = converter->reset();
    if (!status)
        std::cerr << "display: error: " << __func__ << " (reset)" << std::endl;

    /* busy */
    status = converter->setGpioMode(Converter::GpioNum::GPIO1,
        Converter::GpioMode::IN_PU);
    if (!status)
        std::cerr << "display: error: " << __func__ << " (gpio: busy)"
            << std::endl;

    /* reset */
    status = converter->setGpioMode(Converter::GpioNum::GPIO2,
        Converter::GpioMode::OUT_PP);
    if (!status)
        std::cerr << "display: error: " << __func__ << " (gpio: reset)"
            << std::endl;

    /* dc */
    status = converter->setGpioMode(Converter::GpioNum::GPIO3,
        Converter::GpioMode::OUT_PP);
    if (!status)
        std::cerr << "display: error: " << __func__ << " (gpio: dc)"
            << std::endl;
}

static void EPD_4IN2_Reset(void)
{
    gpio_wr_reset(0);
    delay(10);
    gpio_wr_reset(1);
    delay(10);

    gpio_wr_reset(0);
    delay(10);
    gpio_wr_reset(1);
    delay(10);

    gpio_wr_reset(0);
    delay(10);
    gpio_wr_reset(1);
    delay(10);
}

static void EPD_4IN2_SendCommand(uint8_t data)
{
    gpio_wr_dc(0);
    write_data(&data, 1);
}

static void EPD_4IN2_SendData(uint8_t data)
{
    gpio_wr_dc(1);
    write_data(&data, 1);
}

static void EPD_4IN2_SendDataBytes(uint8_t* data, size_t len)
{
    gpio_wr_dc(1);
    write_data(data, len);
}

void EPD_4IN2_ReadBusy(void)
{
    std::cout << "busy pin: wait" << std::endl;

    // LOW: idle, HIGH: busy
    while (gpio_rd_busy() == 0)
        delay(10);

    std::cout << "busy pin: done" << std::endl;
}

static void EPD_4IN2_TurnOnDisplay(void)
{
    EPD_4IN2_SendCommand(0x12);
    delay(100);
    EPD_4IN2_ReadBusy();
}

static void EPD_4IN2_SetLut(void)
{
    unsigned int count;

    EPD_4IN2_SendCommand(0x20);
    for (count = 0; count < 36; count++)
        EPD_4IN2_SendData(EPD_4IN2_lut_vcom0[count]);

    EPD_4IN2_SendCommand(0x21);
    for (count = 0; count < 36; count++)
        EPD_4IN2_SendData(EPD_4IN2_lut_ww[count]);
    
    EPD_4IN2_SendCommand(0x22);
    for (count = 0; count < 36; count++)
        EPD_4IN2_SendData(EPD_4IN2_lut_bw[count]);

    EPD_4IN2_SendCommand(0x23);
    for(count = 0; count < 36; count++)
        EPD_4IN2_SendData(EPD_4IN2_lut_wb[count]);

    EPD_4IN2_SendCommand(0x24);
    for(count = 0; count < 36; count++)
        EPD_4IN2_SendData(EPD_4IN2_lut_bb[count]);
}

static void EPD_4IN2_4Gray_lut(void)
{
    unsigned int count;

    EPD_4IN2_SendCommand(0x20); // vcom
    for (count = 0; count < 42; count++)
        EPD_4IN2_SendData(EPD_4IN2_4Gray_lut_vcom[count]);

    EPD_4IN2_SendCommand(0x21); // red not use
    for (count = 0; count < 42; count++)
        EPD_4IN2_SendData(EPD_4IN2_4Gray_lut_ww[count]);

    EPD_4IN2_SendCommand(0x22); // bw r
    for (count = 0; count < 42; count++)
        EPD_4IN2_SendData(EPD_4IN2_4Gray_lut_bw[count]);

    EPD_4IN2_SendCommand(0x23); // wb w
    for (count = 0; count < 42; count++)
        EPD_4IN2_SendData(EPD_4IN2_4Gray_lut_wb[count]);

    EPD_4IN2_SendCommand(0x24); // bb b
    for (count = 0; count < 42; count++)
        EPD_4IN2_SendData(EPD_4IN2_4Gray_lut_bb[count]);

    EPD_4IN2_SendCommand(0x25); // vcom
    for (count = 0; count < 42; count++)
        EPD_4IN2_SendData(EPD_4IN2_4Gray_lut_ww[count]);
}

void EPD_4IN2_Init_Fast(void)
{
    EPD_4IN2_Reset();

    EPD_4IN2_SendData(0x12);

    EPD_4IN2_SendCommand(0x01); // POWER SETTING 
    EPD_4IN2_SendData(0x03);
    EPD_4IN2_SendData(0x00);
    EPD_4IN2_SendData(0x2b);
    EPD_4IN2_SendData(0x2b);

    EPD_4IN2_SendCommand(0x06); // boost soft start
    EPD_4IN2_SendData(0x17);    // A
    EPD_4IN2_SendData(0x17);    // B
    EPD_4IN2_SendData(0x17);    // C

    EPD_4IN2_SendCommand(0x04);
    EPD_4IN2_ReadBusy();

    EPD_4IN2_SendCommand(0x00); // panel setting
    EPD_4IN2_SendData(0xbf);    // KW-bf KWR-2F BWROTP 0f BWOTP 1f

    EPD_4IN2_SendCommand(0x30);
    EPD_4IN2_SendData(0x3c);    // 3A 100HZ, 29 150Hz, 39 200HZ, 31 171HZ

    EPD_4IN2_SendCommand(0x61); // resolution setting
    EPD_4IN2_SendData(0x01);
    EPD_4IN2_SendData(0x90);    // 400
    EPD_4IN2_SendData(0x01);    // 300
    EPD_4IN2_SendData(0x2c);

    EPD_4IN2_SendCommand(0x82); // vcom_DC setting
    EPD_4IN2_SendData(0x12);

    EPD_4IN2_SendCommand(0x50);
    EPD_4IN2_SendData(0x97);

    EPD_4IN2_SetLut();
}

void EPD_4IN2_Clear(void)
{
    std::memset(framebuffer, 0xFF, sizeof(framebuffer));

    EPD_4IN2_SendCommand(0x10);
    EPD_4IN2_SendDataBytes(framebuffer, sizeof(framebuffer));

    EPD_4IN2_SendCommand(0x13);
    EPD_4IN2_SendDataBytes(framebuffer, sizeof(framebuffer));

    EPD_4IN2_SendCommand(0x12); // DISPLAY REFRESH
    delay(1);    
    EPD_4IN2_TurnOnDisplay();
}

void EPD_4IN2_Display(uint8_t *image)
{
    memset(framebuffer, 0x00, sizeof(framebuffer));

    EPD_4IN2_SendCommand(0x10);
    EPD_4IN2_SendDataBytes(framebuffer, sizeof(framebuffer));

    EPD_4IN2_SendCommand(0x13);
    EPD_4IN2_SendDataBytes(image, sizeof(framebuffer));

    EPD_4IN2_SendCommand(0x12); // DISPLAY REFRESH
    delay(10);
    EPD_4IN2_TurnOnDisplay();
}

void EPD_4IN2_4GrayDisplay(uint8_t *buf10, uint8_t *buf13)
{
    EPD_4IN2_SendCommand(0x10);
    EPD_4IN2_SendDataBytes(buf10, sizeof(framebuffer));

    EPD_4IN2_SendCommand(0x13);
    EPD_4IN2_SendDataBytes(buf13, sizeof(framebuffer));

    EPD_4IN2_4Gray_lut(); // ?
    // EPD_4IN2_TurnOnDisplay();

    // ?
    EPD_4IN2_SendCommand(0x12); // DISPLAY REFRESH
    delay(10);
    EPD_4IN2_TurnOnDisplay();
}

void EPD_4IN2_Sleep(void)
{
    EPD_4IN2_SendCommand(0x50); // DEEP_SLEEP
    EPD_4IN2_SendData(0XF7);

    EPD_4IN2_SendCommand(0x02); // POWER_OFF
    EPD_4IN2_ReadBusy();

    EPD_4IN2_SendCommand(0x07); // DEEP_SLEEP
    EPD_4IN2_SendData(0XA5);
}
