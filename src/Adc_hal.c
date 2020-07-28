/*
 * Adc_hal.c
 *
 *  Created on: Jul 23, 2020
 *      Author: anh
 */
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "Adc_hal.h"

volatile uint8_t result = 0;

void Adc0_SS3_sw_trigger_init(void){
	SYSCTL_RCGCGPIO_R|=SYSCTL_RCGCGPIO_R5;// Enable clock for Port E
	SYSCTL_RCGCADC_R|=SYSCTL_RCGCADC_R0;
	while((SYSCTL_RCGCADC_R&SYSCTL_RCGCADC_R0)==0);
	/* Configure PE3 for Ain0 */
	GPIO_PORTE_AFSEL_R |= GPIO_PIN_3;//PE3
	GPIO_PORTE_AMSEL_R |= GPIO_PIN_3;//analog mode
	GPIO_PORTE_DEN_R &= ~GPIO_PIN_3;//disable digital mode
	GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R &GPIO_PCTL_PE3_M) | GPIO_PCTL_PE3_AIN0;
	GPIO_PORTE_DIR_R &= ~GPIO_PIN_3;

	/* Configure ADC0 with SS3 sample sequencer*/
	ADC0_PC_R = ADC_PC_SR_125K;
	ADC0_SSPRI_R = ADC_SSPRI_SS3_M;
	ADC0_ACTSS_R &= ADC_ACTSS_ASEN3;// disable adc0 ss3 while initializing
	ADC0_EMUX_R = (ADC0_EMUX_R&~ADC_EMUX_EM3_M) | ADC_EMUX_EM3_PROCESSOR;//sw trigger
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R &~ ADC_SSMUX0_MUX0_M) | ADC_SSMUX0_MUX0_S;// Ain0
	ADC0_SSCTL3_R = ADC_SSCTL3_END0 | ADC_SSCTL3_IE0;//Set flag, capture only one time
	ADC0_IM_R |= ADC_IM_MASK3;//Flag create interrupt
	ADC0_ISC_R |= ADC_ISC_IN3;//Clear the flag
	IntPrioritySet(INT_ADC0SS3, 0x80);// priority 4
	IntEnable(INT_ADC0SS3);
	ADC0_ACTSS_R |= ADC_ACTSS_ASEN3;
}

void Adc0_SS3_sw_trigger_start(void){
	ADC0_PSSI_R|= ADC_PSSI_SS3;// enable software trigger on SS3
}

void Adc0SS3Handler(void){
	result = ADC0_SSFIFO3_R & 0xFF;
	ADC0_ISC_R |= ADC_ISC_IN3;// Acknowledge the interrupt
}

uint8_t Adc0SS3GetResult(void){
	return result;
}


