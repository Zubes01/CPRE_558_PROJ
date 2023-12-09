/*
 * adc.h
 *
 *  Created on: Mar 19, 2021
 *      Author: jgortiz
 */
#include "Timer.h"
#include "lcd.h"
#include "resetSimulation.h"
#include "string.h"
#include "movement.h"
#include "open_interface.h"

#ifndef ADC_H_
#define ADC_H_

void ADC0_InitSWTriggerSeq3_Ch10(void);

uint32_t ADC0_InSeq3(void);

#endif /* ADC_H_ */
