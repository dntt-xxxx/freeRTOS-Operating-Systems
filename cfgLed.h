#ifndef CFG_LED_H
#define CFG_LED_H

#include "stm32f1xx_hal.h"

typedef struct{
	GPIO_TypeDef* BUTTON_GPIO;
	uint16_t BUTTON_PIN;
	GPIO_TypeDef* LED_GPIO;
	uint16_t LED_PIN;
}Led_conf_IO;

 extern const Led_conf_IO Led_CfgParam;
#endif /* CFG_LED_H*/
