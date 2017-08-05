

#include "stm32f4xx.h"     
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


void ADC_Configuration(void);
void ADC_Set_EOC_Event(void (*func)(void));
uint16_t ADC_GetRawValue(uint8_t ch);
void ADC_IRQHandler(void);



void ADC_Task_init(void);

                                  
extern uint32_t adcValue;





