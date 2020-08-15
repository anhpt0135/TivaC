#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "Timer_Config.h"
#include "Adc_hal.h"
#include "dbg.h"
#include "UART0.h"

volatile int adc_flag = 0;
uint16_t result;

void User_func(void){
	GPIO_PORTF_DATA_R ^= 0x04;
}

int main(void) {
	// Khai bao bien
	// Cau hinh clock he thong
	SysCtlClockSet(
			SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ
					| SYSCTL_OSC_MAIN);	//400/5 with 5 = 4 + 1, and fill 4 into bit 22
	//Kich hoat port F
	UART0_Init();
	Clear_screen();
	Dbg_info("Enable Port F");
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,
			GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);//Cau hinh 3 LED la output, o day chi su dung led 2 thoi
	Dbg_info("Timer0A_init()");
	//Timer0A_Init();
	Timer0A_Init_new(80000000);
	//Timer3A_Init(User_func, 40000000, 2);
	Dbg_info("Initialize adc0 ss3");
	Adc0_SS3_sw_trigger_init();
	Dbg_info("Enable all interrupt");
	IntMasterEnable();	// Kich hoat ngat master
	while (1) {
		if(adc_flag == 1){
			printf("====ADC output value = %d========\r\n", result);
			adc_flag = 0;
		}
	}
}

//Chuong trinh ngat
void Timer0IntHandler(void) {
	// Xoa het cac ngat timer
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	GPIO_PORTF_DATA_R ^= 0x02;
	result = Adc0_SS3_trigger_and_get_value_without_acknowledgment();
	printf("++ressult == %d\r\n", result);
	ADC0_ISC_R |= ADC_ISC_IN3;//Acknownledge the conversion
	adc_flag = 1;
	//printf("adc_flag == %d\r\n", adc_flag);
}
