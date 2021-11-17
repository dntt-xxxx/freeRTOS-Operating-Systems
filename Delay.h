/*
 *	Delay.c
 *
 *  Created on: April 16, 2021
 *      Author: DNTT
 */
#ifndef _DELAY_H
#define _DELAY_H

/******************************************************************************
 * Inclusion
 * ***************************************************************************/
#include "stm32f1xx_hal.h"



/******************************************************************************
 * API
 * ***************************************************************************/
 /*Function to initialize timer values:RCC and parameters of TIMER */
void delay_Init(void);

/*Delay milliseconds */
void delay_ms(uint32_t ms);

/*Delay microseconds */
void delay_us(uint32_t us);

/*Function to get numbers TICK MILLISECONDS  from start timer */
uint32_t get_Tick_ms(void);

/*Function to get numbers TICK MICROSECONDS from start timer */
uint32_t get_Tick_us(void);



#endif /*_DELAY_H */
