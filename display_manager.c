/*
 * display_manager.c
 *
 *  Created on: Mar 2, 2025
 *      Author: yoyos
 */

#include "display_manager.h"

void DispMng_eDispMode1(void);
void DispMng_eDispMode2(void);
void DispMng_eDispClear(void);

uint8_t u8DispMng_Mode = 0;
uint8_t u8LoadingValue = 0;

char lcdString[32]={0};
ts_lcd16x2 lcd;
extern ts_ltm LocalTime;

void DispMng_eDispMode1(void)
{
	char cStrLine[17];
	memset(cStrLine, 0, 17);
	sensorCfg_eGenerateString(SensorCfg_dist, cStrLine);
	elcd16x2_writeMsg(&lcd, cStrLine, strlen(cStrLine), 0, LCD16x2_LINE1);
	memset(cStrLine, 0, 17);
	sRTCDateTime dt;
	ltm_UnixToDateTime(LocalTime.u32LocalTime, &dt);
	dateFormat("dmy  H:i:s", dt, cStrLine);
	elcd16x2_writeMsg(&lcd, cStrLine, strlen(cStrLine), 0, LCD16x2_LINE2);
}

void DispMng_eDispMode2(void)
{
	char cStrLine[17];
	memset(cStrLine, 0, 17);
	sensorCfg_eGenerateString(SensorCfg_lum, cStrLine);
	elcd16x2_writeMsg(&lcd, cStrLine, strlen(cStrLine), 0, LCD16x2_LINE1);
	memset(cStrLine, 0, 17);
	sensorCfg_eGenerateString(SensorCfg_moist, cStrLine);
	elcd16x2_writeMsg(&lcd, cStrLine, strlen(cStrLine), 0, LCD16x2_LINE2);
}

void DispMng_eDispClear(void)
{
	char cStrLine[17]="                ";
	elcd16x2_writeMsg(&lcd, cStrLine, strlen(cStrLine), 0, LCD16x2_LINE1);
	elcd16x2_writeMsg(&lcd, cStrLine, strlen(cStrLine), 0, LCD16x2_LINE2);
}

void DispMng_eProcess(I2C_HandleTypeDef	*hi2c)
{
	uint8_t u8DispMng_old = u8DispMng_Mode;
	lcd.hi2c = hi2c;
	lcd.u8adress = LCD16x2_ADDRESS;
	lcd.u8col = 16;
	lcd.u8lines = 2;
	lcd.u8dotsize=1;
	elcd16x2_init(&lcd);

	while(1)
	{
		if(u8DispMng_Mode!=u8DispMng_old)
		{
			DispMng_eDispClear();
		}
		switch(u8DispMng_Mode)
		{
		case IdleMode:
			DispMng_eDispClear();
			break;
		case Mode1:
			DispMng_eDispMode1();
			break;
		case Mode2:
			DispMng_eDispMode2();
			break;
		case Mode3:
			elcd16x2_DispLoading(&lcd, u8LoadingValue);
			u8LoadingValue++;
			if(u8LoadingValue>100)
			{
				u8LoadingValue=0;
			}
			break;
		default:
			DispMng_eDispClear();
			break;
		}
		u8DispMng_old=u8DispMng_Mode;
		osDelay(200);
	}
}
