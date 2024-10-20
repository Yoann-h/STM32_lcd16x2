/*
 * lcd16x2.c
 *
 *  Created on: Oct 7, 2024
 *      Author: Yoann Hervagault
 *
 *      inspired from https://github.com/arduino-libraries/LiquidCrystal
 *      hardware used: https://wiki.seeedstudio.com/Grove-16x2_LCD_Series/
 */
#include "lcd16x2.h"

static LCD16x2_Result elcd16x2_icommand(ts_lcd16x2 *lcdstr, uint8_t value);
static LCD16x2_Result elcd16x2_idisplay(ts_lcd16x2 *lcdstr);
static LCD16x2_Result elcd16x2_inoDisplay(ts_lcd16x2 *lcdstr);
static LCD16x2_Result elcd16x2_isetCursor(ts_lcd16x2 *lcdstr, uint8_t col, uint8_t row);

LCD16x2_Result elcd16x2_init(ts_lcd16x2 *lcdstr)
{
	LCD16x2_Result eResult = LCD16x2_OK;
	lcdstr->displaycontrol=0;
	lcdstr->displaymode=0;
	lcdstr->initialized=0;
	lcdstr->numlines=0;
	lcdstr->currline=0;

    if (lcdstr->u8lines > 1) {
    	lcdstr->displayfunction |= LCD_2LINE;
    }
    lcdstr->numlines = lcdstr->u8lines;
    lcdstr->currline = 0;

    // for some 1 line displays you can select a 10 pixel high font
    if ((lcdstr->u8dotsize != 0) && (lcdstr->u8lines == 1)) {
    	lcdstr->displayfunction |= LCD_5x10DOTS;
    }

    HAL_Delay(50);

    // Send function set command sequence
    eResult = elcd16x2_icommand(lcdstr, LCD16x2_FUNCTIONSET | lcdstr->displayfunction);
    HAL_Delay(5);  // wait more than 4.1ms

    // second try
    eResult = elcd16x2_icommand(lcdstr, LCD16x2_FUNCTIONSET | lcdstr->displayfunction);
    HAL_Delay(1);

    // third go
    eResult = elcd16x2_icommand(lcdstr, LCD16x2_FUNCTIONSET | lcdstr->displayfunction);


    // finally, set # lines, font size, etc.
    eResult = elcd16x2_icommand(lcdstr, LCD16x2_FUNCTIONSET | lcdstr->displayfunction);

    // turn the display on with no cursor or blinking default
    lcdstr->displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    eResult = elcd16x2_idisplay(lcdstr);

    // clear it off
    eResult = elcd16x2_clear(lcdstr);

    lcdstr->displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    // set the entry mode
    eResult = elcd16x2_icommand(lcdstr, LCD16x2_ENTRYMODESET | lcdstr->displaymode);
    return eResult;
}

LCD16x2_Result elcd16x2_write(ts_lcd16x2 *lcdstr, uint8_t value)
{
	LCD16x2_Result eResult = LCD16x2_OK;
    unsigned char dta[2] = {0x40, value};
    if(HAL_I2C_Master_Transmit(lcdstr->hi2c, lcdstr->u8adress, dta, 2, 100)!=HAL_OK)
    {
    	eResult = LCD16x2_INTERNAL_ERROR;
    }
    return eResult;
}

LCD16x2_Result elcd16x2_writeMsg(ts_lcd16x2 *lcdstr, uint8_t* fpu8Value, uint8_t fu8Lentgh, uint8_t fu8ColInd, LCD16x2_LINE fu8RowInd)
{
	LCD16x2_Result eResult = LCD16x2_OK;
	eResult = elcd16x2_isetCursor(lcdstr, fu8ColInd, (uint8_t)fu8RowInd);
	if(eResult == LCD16x2_OK)
	{
		for(uint8_t i=0;i<fu8Lentgh;i++)
		{
			eResult = elcd16x2_write(lcdstr,*(fpu8Value+i));
			if(eResult != LCD16x2_OK)
			{
				break;
			}
		}
	}
    return eResult;
}

LCD16x2_Result elcd16x2_clear(ts_lcd16x2 *lcdstr)
{
	LCD16x2_Result eResult = LCD16x2_OK;
	eResult = elcd16x2_icommand(lcdstr,LCD16x2_CLEARDISPLAY);        // clear display, set cursor position to zero
    HAL_Delay(2);
    return eResult;
}

LCD16x2_Result elcd16x2_home(ts_lcd16x2 *lcdstr)
{
	LCD16x2_Result eResult = LCD16x2_OK;
	eResult = elcd16x2_icommand(lcdstr,LCD16x2_RETURNHOME);
    HAL_Delay(2);
    return eResult;
}

LCD16x2_Result elcd16x2_scroll(ts_lcd16x2 *lcdstr, char* fpcline1, uint32_t u32LoopCnt, uint32_t u32DelayMs, LCD16x2_ScrollDir u8Direction, LCD16x2_LINE u8selectline)
{
	static uint8_t u8BounceDir=0;
	LCD16x2_Result eResult = LCD16x2_OK;
	uint32_t u32loopCpt = 0;
	uint8_t u8impCpt = 0;
	char cline1Init[LCD16x2_MAX_SCROLL_SIZE];
	char cline1buffer[16];
	uint32_t u32scrollsize = 16;
	if(strlen(fpcline1)<16)
	{
		u32scrollsize = 16;
	}
	else
	{
		u32scrollsize = strlen(fpcline1);
	}
	memset(cline1Init,0x20,u32scrollsize);//fill with spaces
	memcpy(cline1Init,fpcline1,strlen(fpcline1));
	if(u8Direction == LCD16x2_SCROLLRIGHT || u8Direction == LCD16x2_SCROLLBOUNCE)
	{
		u8impCpt=u32scrollsize;
	}
	while(u32loopCpt<u32LoopCnt)
	{
		for(uint8_t i=0;i<16;i++)
		{
			if(u8impCpt+i<u32scrollsize)
				*(cline1buffer+i)=*(cline1Init+u8impCpt+i);
			else
				*(cline1buffer+i)=*(cline1Init+u8impCpt+i-u32scrollsize);
		}
		elcd16x2_writeMsg(lcdstr, cline1buffer, 16, 0, u8selectline);
		HAL_Delay(u32DelayMs);
		if(u8Direction == LCD16x2_SCROLLLEFT)
		{
			if(u8impCpt<u32scrollsize-1)
				u8impCpt++;
			else
			{
				u8impCpt=0;
				u32loopCpt++;
			}
		}
		else if(u8Direction == LCD16x2_SCROLLRIGHT)
		{
			if(u8impCpt>0)
				u8impCpt--;
			else
			{
				u8impCpt=u32scrollsize-1;
				u32loopCpt++;
			}
		}
		else if(u8Direction == LCD16x2_SCROLLBOUNCE)
		{
			if(u8BounceDir==1)
			{
				if(u8impCpt<u32scrollsize)
					u8impCpt++;
				else
				{
					u8impCpt--;
					u8BounceDir=0;
				}
			}
			else
			{
				if(u8impCpt>strlen(fpcline1))
					u8impCpt--;
				else
				{
					u8BounceDir=1;
					u8impCpt++;
					u32loopCpt++;
				}
			}
		}

	}
	return eResult;
}

LCD16x2_Result elcd16x2_DispLoading(ts_lcd16x2 *lcdstr, uint8_t u8Progress)
{
	LCD16x2_Result eResult = LCD16x2_OK;
	uint8_t pu8LoadDisp[16];
	elcd16x2_writeMsg(lcdstr, "     LOADING    ", 16, 0, LCD16x2_LINE1);
	uint8_t u8nBlock = u8Progress*16/90;
	for(uint8_t i=0;i<16;i++)
	{
		if(i<u8nBlock)
		{
			pu8LoadDisp[i] = 255;
		}
		else
		{
			pu8LoadDisp[i] = ' ';
		}
	}
	elcd16x2_writeMsg(lcdstr, pu8LoadDisp, 16, 0, LCD16x2_LINE2);


}



static LCD16x2_Result elcd16x2_icommand(ts_lcd16x2 *lcdstr, uint8_t value)
{
	LCD16x2_Result eResult = LCD16x2_OK;
    unsigned char dta[2] = {0x80, value};
    if(HAL_I2C_Master_Transmit(lcdstr->hi2c, lcdstr->u8adress, dta, 2, 100)!=HAL_OK)
    {
    	eResult = LCD16x2_INTERNAL_ERROR;
    }
    return eResult;
}
static LCD16x2_Result elcd16x2_idisplay(ts_lcd16x2 *lcdstr)
{
	LCD16x2_Result eResult = LCD16x2_OK;
	lcdstr->displaycontrol |= LCD_DISPLAYON;
	eResult = elcd16x2_icommand(lcdstr, LCD16x2_DISPLAYCONTROL | lcdstr->displaycontrol);
    return eResult;
}
static LCD16x2_Result elcd16x2_inoDisplay(ts_lcd16x2 *lcdstr)
{
	LCD16x2_Result eResult = LCD16x2_OK;
	lcdstr->displaycontrol &= ~LCD_DISPLAYON;
	eResult = elcd16x2_icommand(lcdstr, LCD16x2_DISPLAYCONTROL | lcdstr->displaycontrol);
    return eResult;
}
static LCD16x2_Result elcd16x2_isetCursor(ts_lcd16x2 *lcdstr, uint8_t col, uint8_t row)
{
	LCD16x2_Result eResult = LCD16x2_OK;
    col = (row == 0 ? col | 0x80 : col | 0xc0);
    eResult = elcd16x2_icommand(lcdstr, col);
    return eResult;
}

