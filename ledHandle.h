#ifndef LED_HANDLE_H
#define LED_HANDLE_H

#include "Delay.h"
#include "cfgLed.h"

typedef enum{
	READY,
	PRESS_BUTTON,
	LED
}StateBasic;

typedef struct {
	uint32_t timeStartPressBut;
	uint32_t timeStartLight;
}timeHandle;

void initLed(void);
timeHandle ledHandle(void);

#endif /*LED_HANDLE_H*/
