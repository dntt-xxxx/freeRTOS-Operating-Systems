#include "ledHandle.h"

static StateBasic ledState=READY;
static timeHandle timeLed={0,0};

timeHandle ledHandle(){
		if((Led_CfgParam.BUTTON_GPIO->IDR & Led_CfgParam.BUTTON_PIN) == (uint32_t)0){
			timeLed.timeStartPressBut=get_Tick_us();
			ledState=PRESS_BUTTON;
		}
		switch(ledState){
			case READY:
				break;
			case PRESS_BUTTON:
				Led_CfgParam.LED_GPIO->BSRR = Led_CfgParam.LED_PIN;
				ledState=LED;
				break;
			case LED:
				timeLed.timeStartLight=get_Tick_us();
				ledState=READY;
				break;
			default:
				break;
		}
		return timeLed;
}

void initLed(){
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Led_CfgParam.BUTTON_GPIO, Led_CfgParam.BUTTON_PIN, GPIO_PIN_RESET);

  /*Configure GPIO pin : BUTTON_PIN */
  GPIO_InitStruct.Pin = Led_CfgParam.BUTTON_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(Led_CfgParam.BUTTON_GPIO, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_PIN */
  GPIO_InitStruct.Pin = Led_CfgParam.LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Led_CfgParam.LED_GPIO, &GPIO_InitStruct);
	
	delay_Init();
}
