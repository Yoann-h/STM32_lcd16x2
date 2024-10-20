/*
 * lcd16x2.h
 *
 *  Created on: Oct 7, 2024
 *      Author: Yoann Hervagault
 *
 *      inspired from https://github.com/arduino-libraries/LiquidCrystal
 *      hardware used: https://wiki.seeedstudio.com/Grove-16x2_LCD_Series/
 *
 */

#ifndef YHLIB_LCD16X2_LCD16X2_H_
#define YHLIB_LCD16X2_LCD16X2_H_

#include "stm32l4xx_hal.h"
#include "lcd16x2_cfg.h"

#define LCD16x2_REG_MODE1       (0x00)
#define LCD16x2_REG_MODE2       (0x01)
#define LCD16x2_REG_OUTPUT      (0x08)

// commands
#define LCD16x2_CLEARDISPLAY 	(0x01)
#define LCD16x2_RETURNHOME 		(0x02)
#define LCD16x2_ENTRYMODESET 	(0x04)
#define LCD16x2_DISPLAYCONTROL 	(0x08)
#define LCD16x2_CURSORSHIFT 	(0x10)
#define LCD16x2_FUNCTIONSET 	(0x20)
#define LCD16x2_SETCGRAMADDR 	(0x40)
#define LCD16x2_SETDDRAMADDR 	(0x80)

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

typedef enum {
	LCD16x2_OK, LCD16x2_PARAMETER_ERROR, LCD16x2_INTERNAL_ERROR, LCD16x2_RESULTNB
} LCD16x2_Result;

typedef enum {
	LCD16x2_SCROLLLEFT, LCD16x2_SCROLLRIGHT, LCD16x2_SCROLLBOUNCE, LCD16x2_SCROLLDIRNB
} LCD16x2_ScrollDir;

typedef enum{
	LCD16x2_LINE1, LCD16x2_LINE2, LCD16x2_LINENB
} LCD16x2_LINE;

typedef struct {
	I2C_HandleTypeDef	*hi2c;
	uint8_t    			u8col;
	uint8_t				u8lines;
	uint8_t				u8dotsize;
	uint8_t				u8adress;
	uint8_t 			displayfunction;
	uint8_t 			displaycontrol;
	uint8_t 			displaymode;
	uint8_t 			initialized;
	uint8_t 			numlines;
	uint8_t 			currline;
}ts_lcd16x2;

LCD16x2_Result elcd16x2_init(ts_lcd16x2 *lcdstr);
LCD16x2_Result elcd16x2_write(ts_lcd16x2 *lcdstr, uint8_t value);
LCD16x2_Result elcd16x2_writeMsg(ts_lcd16x2 *lcdstr, uint8_t* fpu8Value, uint8_t fu8Lentgh, uint8_t fu8ColInd, uint8_t fu8RowInd);
LCD16x2_Result elcd16x2_clear(ts_lcd16x2 *lcdstr);
LCD16x2_Result elcd16x2_home(ts_lcd16x2 *lcdstr);
LCD16x2_Result elcd16x2_scroll(ts_lcd16x2 *lcdstr, char* fpcline1, uint32_t u32LoopCnt, uint32_t u32DelayMs,  LCD16x2_ScrollDir u8Direction, LCD16x2_LINE u8selectline);
LCD16x2_Result elcd16x2_DispLoading(ts_lcd16x2 *lcdstr, uint8_t u8Progress);

#endif /* YHLIB_LCD16X2_LCD16X2_H_ */
