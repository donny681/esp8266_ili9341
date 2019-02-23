/* spi_oled example

 This example code is in the Public Domain (or CC0 licensed, at your option.)

 Unless required by applicable law or agreed to in writing, this
 software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 CONDITIONS OF ANY KIND, either express or implied.
 */

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp8266/gpio_struct.h"
#include "esp8266/spi_struct.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_libc.h"

#include "driver/gpio.h"
#include "driver/spi.h"
#include "ili9341.h"
#include "esp_freertos_hooks.h"
static const char *TAG = "spi_oled";
uint8_t oled_dc_level = 0;
static lv_obj_t *chart = NULL;
static lv_chart_series_t *series = NULL;

static void IRAM_ATTR spi_event_callback(int event, void *arg) {
	switch (event) {
	case SPI_INIT_EVENT: {

	}
		break;

	case SPI_TRANS_START_EVENT: {
		gpio_set_level(OLED_DC_GPIO, oled_dc_level);
	}
		break;

	case SPI_TRANS_DONE_EVENT: {

	}
		break;

	case SPI_DEINIT_EVENT: {
	}
		break;
	}
}

static void lv_tick_task(void) {
	lv_tick_inc(1);
}


static void my_spi_init(void)
{
	ESP_LOGI(TAG, "init gpio");
	gpio_config_t io_conf;
	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_OUTPUT;
	io_conf.pin_bit_mask = OLED_PIN_SEL;
	io_conf.pull_down_en = 0;
	io_conf.pull_up_en = 1;
	gpio_config(&io_conf);

	ESP_LOGI(TAG, "init hspi");
	spi_config_t spi_config;
	// Load default interface parameters
	// CS_EN:1, MISO_EN:1, MOSI_EN:1, BYTE_TX_ORDER:1, BYTE_TX_ORDER:1, BIT_RX_ORDER:0, BIT_TX_ORDER:0, CPHA:0, CPOL:0
	spi_config.interface.val = SPI_DEFAULT_INTERFACE;
	// Load default interrupt enable
	// TRANS_DONE: true, WRITE_STATUS: false, READ_STATUS: false, WRITE_BUFFER: false, READ_BUFFER: false
	spi_config.intr_enable.val = SPI_MASTER_DEFAULT_INTR_ENABLE;
	// Cancel hardware cs
	spi_config.interface.cs_en = 0;
	// MISO pin is used for DC
	spi_config.interface.miso_en = 0;
	// CPOL: 1, CPHA: 1
	spi_config.interface.cpol = 0;
	spi_config.interface.cpha = 0;
	// Set SPI to master mode
	// 8266 Only support half-duplex
	spi_config.mode = SPI_MASTER_MODE;
	// Set the SPI clock frequency division factor
	spi_config.clk_div = SPI_2MHz_DIV;
	// Register SPI event callback function
	spi_config.event_cb = spi_event_callback;
	spi_init(HSPI_HOST, &spi_config);

	ESP_LOGI(TAG, "init oled");
}

static void gui_init(void)
{
    //Initialize LittlevGL Library
    lv_init();
    //Register display driver
    lv_disp_drv_t disp;
    lv_disp_drv_init(&disp);
    disp.disp_flush = ili9431_flush;
    lv_disp_drv_register(&disp);
    //Bind littlevgl tick counter to FreeRTOS
    esp_register_freertos_tick_hook(lv_tick_task);

    //Create a new Screen
    lv_obj_t *scr = lv_obj_create(NULL, NULL);
    lv_scr_load(scr);

    //Use alien theme
    lv_theme_t *th = lv_theme_alien_init(100, NULL);
    lv_theme_set_current(th);

    chart = lv_chart_create(scr, NULL);
    lv_obj_set_size(chart, 300, 200);
    lv_obj_align(chart, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_POINT | LV_CHART_TYPE_LINE);
    lv_chart_set_series_opa(chart, LV_OPA_70);
    lv_chart_set_series_width(chart, 4);
    lv_chart_set_range(chart, 0, 100);
    series = lv_chart_add_series(chart, LV_COLOR_RED);
}
static void user_task(void *pvParameter)
{
    uint8_t value = 0;
    while (1)
    {
        value = esp_random() % 100;
        printf("value=%d........\r\n",value);
        lv_chart_set_next(chart, series, value);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

static void gui_turbo_task(void *pvParameter)
{
    while (1)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        lv_task_handler();
    }
}

void app_main(void) {
	rtc_clk_cpu_freq_set(RTC_CPU_FREQ_160M);
	my_spi_init();
	ili9431_init();
	gui_init();
	 xTaskCreate(
	        user_task,   //Task Function
	        "user_task", //Task Name
	        512,         //Stack Depth
	        NULL,        //Parameters
	        1,           //Priority
	        NULL);       //Task Handler
	 xTaskCreate(
	        gui_turbo_task,   //Task Function
	        "gui_turbo_task", //Task Name
	        2048,             //Stack Depth
	        NULL,             //Parameters
	        1,                //Priority
	        NULL);            //Task Handler
}
