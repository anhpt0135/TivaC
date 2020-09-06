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
#include "dbg.h"
volatile uint16_t result = 0;

void Adc0_SS3_sw_trigger_init(void){
	Dbg_info("Enable clock for port E");
	SYSCTL_RCGCGPIO_R|=SYSCTL_RCGCGPIO_R4;// Enable clock for Port E
	SYSCTL_RCGCADC_R|=SYSCTL_RCGCADC_R0;
	while((SYSCTL_RCGCADC_R&SYSCTL_RCGCADC_R0)==0){
		Dbg_info("Waiting for ADC clock is set");
	};
	Dbg_info("Configure PE3 for Ain0");
	/* Configure PE3 for Ain0 */
	GPIO_PORTE_AFSEL_R |= GPIO_PIN_3;//PE3
	GPIO_PORTE_AMSEL_R |= GPIO_PIN_3;//analog mode
	Dbg_info("Disable digital mode");
	GPIO_PORTE_DEN_R &= ~GPIO_PIN_3;//disable digital mode
	GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R &GPIO_PCTL_PE3_M) | GPIO_PCTL_PE3_AIN0;
	Dbg_info("Configure PE3 to be input pin");
	GPIO_PORTE_DIR_R &= ~GPIO_PIN_3;

	/* Configure ADC0 with SS3 sample sequencer*/
	Dbg_info("Configure ADC0 with SS3 sample Sequencer");
	ADC0_PC_R = ADC_PC_SR_125K;
	ADC0_SSPRI_R = ADC_SSPRI_SS3_M;
	ADC0_ACTSS_R &= ADC_ACTSS_ASEN3;// disable adc0 ss3 while initializing
	ADC0_EMUX_R = (ADC0_EMUX_R&~ADC_EMUX_EM3_M) | ADC_EMUX_EM3_PROCESSOR;//sw trigger
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R &~ ADC_SSMUX0_MUX0_M) | ADC_SSMUX0_MUX0_S;// Ain0
	ADC0_SSCTL3_R = ADC_SSCTL3_END0 | ADC_SSCTL3_IE0;//Set flag, capture only one time
	//ADC0_IM_R |= ADC_IM_MASK3;//Flag create interrupt
	ADC0_ISC_R |= ADC_ISC_IN3;//Clear the flag
	//Dbg_info("Set priority 4 for INT_ADC0SS3");
	//IntPrioritySet(INT_ADC0SS3, 0x80);// priority 4
	//IntEnable(INT_ADC0SS3);
	ADC0_ACTSS_R |= ADC_ACTSS_ASEN3;
	Dbg_info("End of Adc0_SS3_sw_trigger_init");
}

void Adc0_SS3_sw_trigger_start(void){
	ADC0_PSSI_R|= ADC_PSSI_SS3;// enable software trigger on SS3
}

uint16_t Adc0_SS3_trigger_and_get_value_without_acknowledgment(void){
	ADC0_PSSI_R |= ADC_PSSI_SS3;
	while((ADC0_RIS_R & ADC_RIS_INR3) == 0){};
	Dbg_info("Get the ADC0 SSFIFO3 result");
	return (ADC0_SSFIFO3_R & 0x0FFF);
}

void Adc0SS3Handler(void){
	Dbg_info("Adc0SS3Handler\r\n");
	result = ADC0_SSFIFO3_R & 0xFFFF;
	Dbg_info("+++result = %d\n", result);
	ADC0_ISC_R |= ADC_ISC_IN3;// Acknowledge the interrupt
}

uint16_t Adc0SS3GetResult(void){
	return result;
}


