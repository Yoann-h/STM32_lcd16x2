/*
 * display_manager.h
 *
 *  Created on: Mar 2, 2025
 *      Author: yoyos
 */

#ifndef LCD16X2_DISPLAY_MANAGER_H_
#define LCD16X2_DISPLAY_MANAGER_H_

#include "lcd16x2.h"
#include "sensor_config.h"
#include "DS1307.h"
#include "local_time_manager.h"

typedef enum {
	DispMng_OK, DispMng_PARAMETER_ERROR, DispMng_INTERNAL_ERROR, DispMng_RESULTNB
} DispMng_Result;

typedef enum {
	IdleMode = 0,
	Mode1,
	Mode2,
	Mode3,
	LoadingMode,
	ModeCnt
} DispMng_ModeList;

void DispMng_eProcess(I2C_HandleTypeDef	*hi2c);

#endif /* LCD16X2_DISPLAY_MANAGER_H_ */
