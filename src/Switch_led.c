/*
 * switch_led.c
 *
 *  Created on: Aug 31, 2020
 *      Author: anhpt0135
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "dbg.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "Switch_led.h"
#include "PLL.h"

#define DELAY10MS 800

void led_init() {
	Dbg_info("Enable LED 1, 2, 3");
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,
			GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); //Cau hinh 3 LED la output, o day chi su dung led 2 thoi
}

void switch_init(){
	Dbg_info("Enable switch 0 and switch 4");
	//Unlock for PF0
    //
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x1;
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable this if led_init() is not called first
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,
			GPIO_PIN_0 | GPIO_PIN_4); //PF0 and PF4 is input
	GPIOPadConfigSet(GPIO_PORTF_BASE ,GPIO_PIN_0 | GPIO_PIN_4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
}

//------------Switch_Debounce------------
// Read and return the status of the switch
// Input: none
// Output: 0x02 if PB1 is high
//         0x00 if PB1 is low
// debounces switch

uint32_t debouncing_switch_without_interrupt(uint32_t pin){
	uint32_t current, previous;
	int time = 1000;
	current = (uint32_t)GPIOPinRead(GPIO_PORTF_BASE, pin);
	while(time){
		previous = current;
		current = (uint32_t)GPIOPinRead(GPIO_PORTF_BASE, pin);
		delay_systic(DELAY10MS);//Delay 10 us
		if(current == previous){
			time --;
		} else {
			time = 1000;
			previous = current;
		}
	}
	//Dbg_info("current info ======%x ====", current);
	return current;// or previous
}

int touch(uint32_t pin){
	if ((debouncing_switch_without_interrupt(pin) & pin) == 0){
		return 1;
	} else {
		return 0;
	}
}

//int debouncing_switch_with_interrupt();


