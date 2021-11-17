/*
 *	Delay.c
 *
 *  Created on: April 4, 2021
 *      Author: DNTT
 */

/******************************************************************************
 * Include
 * ***************************************************************************/
#include "Delay.h"



/******************************************************************************
 * Define
 * ***************************************************************************/
 /*Select one of timer: TIM2 or TIM3 or TIM4*/
#define DELAY_TIMER                TIM3

/*TIMx global Interrupt, x:2,3,4 */
#define DELAY_IRQn                    TIM3_IRQn

/*Enable RCC clock TIMx, x:2,3,4 */
#define DELAY_RCC_CLC_ENABLE       __HAL_RCC_TIM3_CLK_ENABLE

/*Handles TIM2 global interrupt */
#define DELAY_IRQHandler              TIM3_IRQHandler

/*Changes according to the configured clock timer */
#define APBx_TIMER_CLOCK           72 /*MHz */

/*Initialize value Count Preriod */
#define DELAY_TIMER_ARR               65000



/******************************************************************************
 * Variable
 * ***************************************************************************/
static TIM_HandleTypeDef htim_delay;
volatile static uint32_t tick_ms=0,tick_us=0;



/******************************************************************************
 * Prototype
 * ***************************************************************************/
static void Error_Handler(void){
  __disable_irq();
  while (1)
  {
  }
}
void DELAY_IRQHandler(void){
  HAL_TIM_IRQHandler(&htim_delay);
  tick_ms+=DELAY_TIMER_ARR/1000;
  tick_us+=DELAY_TIMER_ARR;
}
uint32_t get_Tick_ms(void){

  return tick_ms + (htim_delay.Instance->CNT)/1000;
}
uint32_t get_Tick_us(void){

  return tick_us + (htim_delay.Instance->CNT);
}

void delay_ms(uint32_t ms){
  uint32_t tick_Current=get_Tick_ms();
  while ((get_Tick_ms()-tick_Current) < ms){};
}
void delay_us(uint32_t us){
  uint32_t tick_Current=get_Tick_us();
  while ((get_Tick_us()-tick_Current) < us){};
}


void delay_Init(void){
  /*config interrupt,RCC */
    DELAY_RCC_CLC_ENABLE();
    HAL_NVIC_SetPriority(DELAY_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(DELAY_IRQn);

  /*Setting timer*/
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim_delay.Instance = DELAY_TIMER;
  htim_delay.Init.Prescaler = APBx_TIMER_CLOCK-1;
  htim_delay.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim_delay.Init.Period = DELAY_TIMER_ARR;
  htim_delay.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim_delay.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim_delay) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim_delay, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim_delay, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_TIM_Base_Start_IT(&htim_delay);
}
