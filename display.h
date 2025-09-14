/**
 * Dmitry Proshutinsky <sodaspace@ayndex.ru>
 * 2025
 * 
 * Source: waveshareteam
 * https://github.com/waveshareteam/e-Paper
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "converter.h"


void display_init(Converter* cnv);
void EPD_4IN2_Init_Fast(void);
void EPD_4IN2_Clear(void);
void EPD_4IN2_Display(uint8_t *image);
void EPD_4IN2_Sleep(void);

#endif /* DISPLAY_H_ */
