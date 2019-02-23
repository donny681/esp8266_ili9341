# _SPI OLED Example_  

_This example uses the ESP8266 hspi Master to drive 2.7-inch SPI ILI9341, and the LCD driver chip is SSD1306._

## How to use example  
SDK

### Hardware Required  

* Connection:  

| Signal    | Oled   | ESP8266|
|-----------|--------|--------|
| 3.3V      | VCC    | VCC    |
| SCLK      | D0(SCL)| GPIO14 |
| D/C       | DC     | GPIO12 |
| MOSI      | D1(SDA)| GPIO13 |
| RST       | RES    | GPIO15 |
| CS        | CS     | GND    |
| GND       | GND    | GND    |

### Configure the project  

```
make menuconfig
```

* Set serial port under Serial Flasher Options.


### Build and Flash  

Build the project and flash it to the board, then run monitor tool to view serial output:

```
make -j4 flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output  

* LOG:  

```
load 0x40100000, len 7188, room 16 
0x40100000: _stext at ??:?

tail 4
chksum 0x61
load 0x3ffe8408, len 24, room 4 
tail 4
chksum 0xe8
load 0x3ffe8420, len 3552, room 4 
tail 12
chksum 0xdf
I (60) boot: ESP-IDF v3.2-dev-81-g6d3b3e3-dirty 2nd stage bootloader
I (60) boot: compile time 10:00:18
I (61) boot: SPI Speed      : 40MHz
I (68) boot: SPI Mode       : DIO
I (74) boot: SPI Flash Size : 2MB
I (80) boot: Partition Table:
I (85) boot: ## Label            Usage          Type ST Offset   Length
I (97) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (108) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (120) boot:  2 factory          factory app      00 00 00010000 000f0000
I (131) boot: End of partition table
I (138) esp_image: segment 0: paddr=0x00010010 vaddr=0x40210010 size=0x3fc00 (261120) map
I (260) esp_image: segment 1: paddr=0x0004fc18 vaddr=0x3ffe8000 size=0x005a8 (  1448) load
I (261) esp_image: segment 2: paddr=0x000501c8 vaddr=0x3ffe85a8 size=0x0011c (   284) load
I (271) esp_image: segment 3: paddr=0x000502ec vaddr=0x40100000 size=0x06504 ( 25860) load
0x40100000: _stext at ??:?

I (295) boot: Loaded app from partition at offset 0x10000
I (363) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (373) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (603) reset_reason: RTC reset 2 wakeup 0 store 0, reason is 2
I (613) spi_oled: init gpio
I (623) gpio: GPIO[12]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (633) gpio: GPIO[15]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (643) spi_oled: init hspi
I (653) spi_oled: init oled

```

* WAVE FORM:  

  ![wave](wave.png)  
