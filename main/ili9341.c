/*
 * ili9341.c
 *
 *  Created on: Feb 22, 2019
 *      Author: fyy
 */

/**********************
 *      TYPEDEFS
 **********************/
#include "ili9341.h"
#include "esp_log.h"
static const char *TAG = "SPI";
/*The LCD needs a bunch of command/argument values to be initialized. They are stored in this struct. */
typedef struct {
	uint8_t cmd;
	uint8_t data[16];
	uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;



/**********************
 *  STATIC PROTOTYPES
 **********************/
static void ili9441_send_cmd(uint8_t cmd);
static void ili9341_send_data(void * data, uint16_t length);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
void disp_spi_send(uint8_t * data, uint16_t length) {
	if (length == 0)
		return;           //no need to send anything

//	spi_transaction_t t;
//	memset(&t, 0, sizeof(t));       	//Zero out the transaction
//	t.length = length * 8;  //Length is in bytes, transaction length is in bits.
//	t.tx_buffer = data;               	//Data
//
//	esp_err_t ret;
////	ret=spi_device_transmit(spi, &t);  //Transmit!
////	assert(ret==ESP_OK);            	 //Should have had no issues.
//
//	spi_device_queue_trans(spi, &t, portMAX_DELAY);
//
//	spi_transaction_t * rt;
//	spi_device_get_trans_result(spi, &rt, portMAX_DELAY);

	for(uint16_t i=0;i<length;i++)
	{
//		ESP_LOGI(TAG, "%x",data[i]);
		uint32_t buf = data[i] << 24;
		spi_trans_t trans = { 0 };
		trans.mosi = &buf;
		trans.bits.mosi = 8;
		spi_trans(HSPI_HOST, trans);
	}
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ili9431_init(void) {
	lcd_init_cmd_t ili_init_cmds[]={
			/* Power contorl B, power control = 0, DC_ENA = 1 */
			    {0xCF, {0x00, 0x83, 0X30}, 3},
			    /* Power on sequence control,
			     * cp1 keeps 1 frame, 1st frame enable
			     * vcl = 0, ddvdh=3, vgh=1, vgl=2
			     * DDVDH_ENH=1
			     */
			    {0xED, {0x64, 0x03, 0X12, 0X81}, 4},
			    /* Driver timing control A,
			     * non-overlap=default +1
			     * EQ=default - 1, CR=default
			     * pre-charge=default - 1
			     */
			    {0xE8, {0x85, 0x01, 0x79}, 3},
			    /* Power control A, Vcore=1.6V, DDVDH=5.6V */
			    {0xCB, {0x39, 0x2C, 0x00, 0x34, 0x02}, 5},
			    /* Pump ratio control, DDVDH=2xVCl */
			    {0xF7, {0x20}, 1},
			    /* Driver timing control, all=0 unit */
			    {0xEA, {0x00, 0x00}, 2},
			    /* Power control 1, GVDD=4.75V */
			    {0xC0, {0x26}, 1},
			    /* Power control 2, DDVDH=VCl*2, VGH=VCl*7, VGL=-VCl*3 */
			    {0xC1, {0x11}, 1},
			    /* VCOM control 1, VCOMH=4.025V, VCOML=-0.950V */
			    {0xC5, {0x35, 0x3E}, 2},
			    /* VCOM control 2, VCOMH=VMH-2, VCOML=VML-2 */
			    {0xC7, {0xBE}, 1},
			    /* Memory access contorl, MX=MY=0, MV=1, ML=0, BGR=1, MH=0 */
			    {0x36, {0x28}, 1},
			    /* Pixel format, 16bits/pixel for RGB/MCU interface */
			    {0x3A, {0x55}, 1},
			    /* Frame rate control, f=fosc, 70Hz fps */
			    {0xB1, {0x00, 0x1B}, 2},
			    /* Enable 3G, disabled */
			    {0xF2, {0x08}, 1},
			    /* Gamma set, curve 1 */
			    {0x26, {0x01}, 1},
			    /* Positive gamma correction */
			    {0xE0, {0x1F, 0x1A, 0x18, 0x0A, 0x0F, 0x06, 0x45, 0X87, 0x32, 0x0A, 0x07, 0x02, 0x07, 0x05, 0x00}, 15},
			    /* Negative gamma correction */
			    {0XE1, {0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3A, 0x78, 0x4D, 0x05, 0x18, 0x0D, 0x38, 0x3A, 0x1F}, 15},
			    /* Column address set, SC=0, EC=0xEF */
			    {0x2A, {0x00, 0x00, 0x00, 0xEF}, 4},
			    /* Page address set, SP=0, EP=0x013F */
			    {0x2B, {0x00, 0x00, 0x01, 0x3f}, 4},
			    /* Memory write */
			    {0x2C, {0}, 0},
			    /* Entry mode set, Low vol detect disabled, normal display */
			    {0xB7, {0x07}, 1},
			    /* Display function control */
			    {0xB6, {0x0A, 0x82, 0x27, 0x00}, 4},
			    /* Sleep out */
			    {0x11, {0}, 0x80},
			    /* Display on */
			    {0x29, {0}, 0x80},
			    {0, {0}, 0xff},
		};

	//Initialize non-SPI GPIOs
	gpio_set_direction(ILI9341_DC, GPIO_MODE_OUTPUT);
	gpio_set_direction(ILI9341_RST, GPIO_MODE_OUTPUT);
	gpio_set_direction(ILI9341_BCKL, GPIO_MODE_OUTPUT);

	//Reset the display
	gpio_set_level(ILI9341_RST, 0);
	vTaskDelay(100 / portTICK_RATE_MS);
	gpio_set_level(ILI9341_RST, 1);
	vTaskDelay(100 / portTICK_RATE_MS);

	printf("ILI9341 initialization.\n");

	//Send all the commands
	uint16_t cmd = 0;
	while (ili_init_cmds[cmd].databytes != 0xff) {
		ili9441_send_cmd(ili_init_cmds[cmd].cmd);
		ili9341_send_data(ili_init_cmds[cmd].data,
				ili_init_cmds[cmd].databytes & 0x1F);
		if (ili_init_cmds[cmd].databytes & 0x80) {
			vTaskDelay(100 / portTICK_RATE_MS);
		}
		cmd++;
	}

	///Enable backlight
	printf("Enable backlight.\n");
	gpio_set_level(ILI9341_BCKL, 1);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static esp_err_t ili9341_set_dc(uint8_t dc)
{
    oled_dc_level = dc;
    return ESP_OK;
}

static void ili9441_send_cmd(uint8_t cmd) {
//	ESP_LOGE(TAG, "send cmd");
//	gpio_set_level(ILI9341_DC, 0); /*Command mode*/
	ili9341_set_dc(0);
	disp_spi_send(&cmd, 1);
}

static void ili9341_send_data(void * data, uint16_t length) {
	ili9341_set_dc(1);
//	gpio_set_level(ILI9341_DC, 1); /*Data mode*/
//	ESP_LOGE(TAG, "send data ,length=%d",length);
	disp_spi_send(data, length);
//	static int i=0;
//	if(i==50)
//	vTaskDelay(20 / portTICK_RATE_MS);
}

void ili9431_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color)
{
	uint8_t data[4];

	/*Column addresses*/
	ili9441_send_cmd(0x2A);
	data[0] = (x1 >> 8) & 0xFF;
	data[1] = x1 & 0xFF;
	data[2] = (x2 >> 8) & 0xFF;
	data[3] = x2 & 0xFF;
	ili9341_send_data(data, 4);

	/*Page addresses*/
	ili9441_send_cmd(0x2B);
	data[0] = (y1 >> 8) & 0xFF;
	data[1] = y1 & 0xFF;
	data[2] = (y2 >> 8) & 0xFF;
	data[3] = y2 & 0xFF;
	ili9341_send_data(data, 4);

	/*Memory write*/
	ili9441_send_cmd(0x2C);

	uint32_t size = (x2 - x1 + 1) * (y2 - y1 + 1);
	uint16_t color_swap = ((color.full >> 8) & 0xFF) | ((color.full & 0xFF) << 8);	/*It's a 8 bit SPI bytes need to be swapped*/
	uint16_t buf[ILI9341_HOR_RES];

	uint32_t i;
	if(size < ILI9341_HOR_RES) {
		for(i = 0; i < size; i++) buf[i] = color_swap;

	} else {
		for(i = 0; i < ILI9341_HOR_RES; i++) buf[i] = color_swap;
	}

	while(size > ILI9341_HOR_RES) {
		ili9341_send_data(buf, ILI9341_HOR_RES * 2);
		size -= ILI9341_HOR_RES;
	}

	ili9341_send_data(buf, size * 2);	/*Send the remaining data*/
}



void ili9431_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_map)
{
	uint8_t data[4];

	/*Column addresses*/
	ili9441_send_cmd(0x2A);
	data[0] = (x1 >> 8) & 0xFF;
	data[1] = x1 & 0xFF;
	data[2] = (x2 >> 8) & 0xFF;
	data[3] = x2 & 0xFF;
	ili9341_send_data(data, 4);

	/*Page addresses*/
	ili9441_send_cmd(0x2B);
	data[0] = (y1 >> 8) & 0xFF;
	data[1] = y1 & 0xFF;
	data[2] = (y2 >> 8) & 0xFF;
	data[3] = y2 & 0xFF;
	ili9341_send_data(data, 4);

	/*Memory write*/
	ili9441_send_cmd(0x2C);


	uint32_t size = (x2 - x1 + 1) * (y2 - y1 + 1);

	/*Byte swapping is required*/
	uint32_t i;
	uint8_t * color_u8 = (uint8_t *) color_map;
	uint8_t color_tmp;
	for(i = 0; i < size * 2; i += 2) {
		color_tmp = color_u8[i + 1];
		color_u8[i + 1] = color_u8[i];
		color_u8[i] = color_tmp;
	}


	while(size > ILI9341_HOR_RES) {

		ili9341_send_data((void*)color_map, ILI9341_HOR_RES * 2);
		//vTaskDelay(10 / portTICK_PERIOD_MS);
		size -= ILI9341_HOR_RES;
		color_map += ILI9341_HOR_RES;
	}

	ili9341_send_data((void*)color_map, size * 2);	/*Send the remaining data*/

	lv_flush_ready();

}

