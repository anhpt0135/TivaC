/*
 * Adchal.h
 *
 *  Created on: Jul 23, 2020
 *      Author: anh
 */

#ifndef SRC_ADC_HAL_H_
#define SRC_ADC_HAL_H_

void Adc0_SS3_sw_trigger_init(void);
void Adc0_SS3_sw_trigger_start(void);
uint8_t Adc0SS3GetResult(void);

#endif /* SRC_ADC_HAL_H_ */
