/*
 * ili9341.h
 *
 *  Created on: Feb 22, 2019
 *      Author: fyy
 */

#ifndef MAIN_ILI9341_H_
#define MAIN_ILI9341_H_
#define ILI9341_HOR_RES	320
#define ILI9341_VER_RES	320

#define ILI9341_DC   12
#define ILI9341_RST  15
#define ILI9341_BCKL 14
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/spi.h"
#include "../lvgl/lvgl.h"

#define OLED_DC_GPIO     12
#define OLED_RST_GPIO    15
#define OLED_PIN_SEL  (1ULL<<OLED_DC_GPIO) | (1ULL<<OLED_RST_GPIO)
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
extern uint8_t oled_dc_level;
void ili9431_init(void);
void ili9431_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color);
void ili9431_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_map);

/**********************
 *      MACROS
 **********************/

#endif /* MAIN_ILI9341_H_ */
