#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "Timer_Config.h"


void User_func(void){
	GPIO_PORTF_DATA_R ^= 0x04;
}

int main(void)
{
	// Khai bao bien
	// Cau hinh clock he thong
	SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);//400/5 with 5 = 4 + 1, and fill 4 into bit 22
	//Kich hoat port F
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);	//Cau hinh 3 LED la output, o day chi su dung led 2 thoi
	//Timer0A_Init();
	//Timer0A_Init_new(80000000);
	Timer3A_Init(User_func, 40000000, 2);
	IntMasterEnable();	// Kich hoat ngat master
	while(1)
	{}

	}

//Chuong trinh ngat
void Timer0IntHandler(void) {
	// Xoa het cac ngat timer
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	GPIO_PORTF_DATA_R ^= 0x02;
}

/*int main1()
 {
 ROM_SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
 ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
 ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);

 for (;;) {
 // set the red LED pin high, others low
 ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_RED);
 ROM_SysCtlDelay(5000000);
 ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);
 ROM_SysCtlDelay(5000000);
 }
 return 0;
 }*/
