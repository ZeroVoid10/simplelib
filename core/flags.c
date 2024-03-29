/*******************************************************************************
 * Copyright:		BUPT
 * File Name:		flags.c
 * Description:		None
 * Author:			ZeroVoid
 * Version:			0.1
 * Data:			2019/10/01 Tue 08:52
 *******************************************************************************/
#include "flags.h"

/* CAN Callback -----------------------------------------------------*/
int can_exc_callback_flag = 0;

/* CMD_DMA -----------------------------------------------------*/
int DMA_RxOK_Flag = 0;

/* Time Flags -----------------------------------------------------*/
uint8_t _5ms = 0;

/* Test Flags -----------------------------------------------------*/
uint8_t test_flag = 1;

/*******************************************************************************
 * Flow Control Flags
 *******************************************************************************/
int send_wave_flag = 0;

/* Kick Flags -----------------------------------------------------*/
uint8_t kick_test_flag = 0;