/***************************************************
  This is a library for our I2C LED Backpacks

  Designed specifically to work with the Adafruit LED Matrix backpacks
  ----> http://www.adafruit.com/products/
  ----> http://www.adafruit.com/products/

  These displays use I2C to communicate, 2 pins are required to
  interface. There are multiple selectable I2C addresses. For backpacks
  with 2 Address Select pins: 0x70, 0x71, 0x72 or 0x73. For backpacks
  with 3 Address Select pins: 0x70 thru 0x77

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#pragma once
#ifndef Adafruit_LEDBackpack_h
#define Adafruit_LEDBackpack_h

#include <applibs/i2c.h>

//#define LED_ON 1
//#define LED_OFF 0

#define LED_RED 1
#define LED_YELLOW 2
#define LED_GREEN 3

#define HT16K33_BLINK_CMD 0x80
#define HT16K33_BLINK_DISPLAYON 0x01
#define HT16K33_BLINK_OFF 0
#define HT16K33_BLINK_2HZ  1
#define HT16K33_BLINK_1HZ  2
#define HT16K33_BLINK_HALFHZ  3

#define HT16K33_CMD_BRIGHTNESS 0xE0

#define SEVENSEG_DIGITS 5


// base
#define BYTE 2
#define DEC 10

 // this is the raw HT16K33 controller
class Adafruit_LEDBackpack {
public:
	Adafruit_LEDBackpack(I2C_InterfaceId interface);
	void begin(uint8_t _addr = 0x70);
	void setBrightness(uint8_t b);
	void blinkRate(uint8_t b);
	void writeDisplay(void);
	void clear(void);
	uint16_t displaybuffer[8];
protected:
	I2C_InterfaceId interfaceId;
	int fd;
	uint8_t i2c_addr;
};

class Adafruit_7segment : public Adafruit_LEDBackpack {
public:
	Adafruit_7segment(I2C_InterfaceId interface);
	size_t write(uint8_t c);

	void print(char, int = BYTE);
	void print(unsigned char, int = BYTE);
	void print(int, int = DEC);
	void print(unsigned int, int = DEC);
	void print(long, int = DEC);
	void print(unsigned long, int = DEC);
	void print(double, int = 2);
	void println(char, int = BYTE);
	void println(unsigned char, int = BYTE);
	void println(int, int = DEC);
	void println(unsigned int, int = DEC);
	void println(long, int = DEC);
	void println(unsigned long, int = DEC);
	void println(double, int = 2);
	void println(void);

	void writeDigitRaw(uint8_t x, uint8_t bitmask);
	void writeDigitNum(uint8_t x, uint8_t num, bool dot = false);
	void drawColon(bool state);
	void printNumber(long, uint8_t = 2);
	void printFloat(double, uint8_t = 2, uint8_t = DEC);
	void printError(void);

	void writeColon(void);

private:
	uint8_t position;
};
#endif // Adafruit_LEDBackpack_h
