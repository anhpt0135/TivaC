/*#include <stdbool.h>
#include <stdint.h>

#include "../driverlib/can.h"
#include "../driverlib/gpio.h"
#include "../driverlib/pin_map.h"
#include "../driverlib/rom.h"
#include "../driverlib/sysctl.h"
#include "../driverlib/interrupt.h"
#include "../inc/hw_types.h"
#include "../inc/hw_gpio.h"
#include "../inc/hw_memmap.h"
#include "../inc/hw_sysctl.h"
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "Timer3A.h"

#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

#define GPIO_LOCK_KEY           0x4C4F434B  // Unlocks the GPIO_CR register
#define PF0       (*((volatile uint32_t *)0x40025004))
#define PF4       (*((volatile uint32_t *)0x40025040))
#define SWITCHES  (*((volatile uint32_t *)0x40025044))
#define SW1       0x10                      // on the left side of the Launchpad board
#define SW2       0x01                      // on the right side of the Launchpad board
#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control
#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08

void PortF_Init(void) {
	SYSCTL_RCGCGPIO_R |= 0x00000020;  		// 1) activate clock for Port F
	while((SYSCTL_RCGCGPIO_R & 0x20)==0){}; // allow time for clock to start
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   		// 2) unlock GPIO Port F
	GPIO_PORTF_CR_R = 0x1F;           		// allow changes to PF4-0
	// only PF0 needs to be unlocked, other bits can't be locked
	GPIO_PORTF_AMSEL_R = 0x00;        		// 3) disable analog on PF
	GPIO_PORTF_PCTL_R = 0x00000000;   		// 4) PCTL GPIO on PF4-0
	GPIO_PORTF_DIR_R = 0x0E;          		// 5) PF4,PF0 in, PF3-1 out
	GPIO_PORTF_AFSEL_R = 0x00;        		// 6) disable alt funct on PF7-0
	GPIO_PORTF_PUR_R = 0x11;          		// enable pull-up on PF0 and PF4
	GPIO_PORTF_DEN_R = 0x1F;          		// 7) enable digital I/O on PF4-0
}
uint32_t PortF_Input(void) {
	return (GPIO_PORTF_DATA_R & 0x11);  // read PF4,PF0 inputs
}

uint32_t PortF_Input_Debounce(void){
	return (GPIO_PORTF_DATA_R & 0x11);  // read PF4, PF0 inputs
}

void PortF_Output(uint32_t data) { // write PF3-PF1 outputs
	GPIO_PORTF_DATA_R = data;
}


void UserTask(void){
	GPIO_PORTF_DATA_R ^= RED;
}

int main1(void) {
	uint32_t status;
	PLL_Init(Bus80MHz);
	PortF_Init();              // initialize PF0 and PF4 and make them inputs
	Timer3A_Init(UserTask, 80000000, 2);// 2 Hz
	IntMasterEnable();
	while (1) {
		status = PortF_Input_Debounce();
		switch (status) {       // switches are negative logic on PF0 and PF4
		case 0x01:
			PortF_Output(BLUE);
			break;   			// SW1 pressed
		case 0x10:
			PortF_Output(RED);
			break;    			// SW2 pressed
		case 0x00:
			PortF_Output(GREEN);
			break;  			// both switches pressed
		case 0x11:
			PortF_Output(0);
			break;      		// neither switch pressed
		}
	}
}*/

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"


void Timer0A_Init(void){
	uint32_t ui32Period;
	//Kich hoat timer0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);	//Cau hinh timer

	ui32Period = (SysCtlClockGet() /10) /2;		//Thiet lap gia tri cua chu ky dua tren tan so he thong
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);

	//Kich hoat ngat tai timer 0
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER0_BASE, TIMER_A);	//Kich hoat timer0
}

void Timer0A_Init_new(uint32_t period){
	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0;
	while((SYSCTL_RCGCTIMER_R&SYSCTL_RCGCTIMER_R0) == 0){};
	TIMER0_CTL_R &= ~TIMER_CTL_TAEN;// Disable timer0A while initializing
	TIMER0_CFG_R = TIMER_CFG_32_BIT_TIMER; //32 bit mode
	TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
	TIMER0_TAPR_R = 0;
	TIMER0_TAILR_R = period -1;
	TIMER0_IMR_R |= TIMER_IMR_TATOIM;// timeout interrupt
	TIMER0_ICR_R |= TIMER_ICR_TATOCINT;
	NVIC_PRI4_R = (NVIC_PRI4_R & 0x00FFFFFF) | (2U << 29);
	NVIC_EN0_R = 0x00080000;//Enable interrupt 19 in Nvic
	TIMER0_CTL_R |= TIMER_CTL_TAEN;

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
	Timer0A_Init_new(80000000);
	IntMasterEnable();	// Kich hoat ngat master
	while(1)
	{}

	}

//Chuong trinh ngat
void Timer0IntHandler(void) {
	// Xoa het cac ngat timer
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	// Doc trang thai hien tai cua chan PF2 (LED BLUE) sau do thuc hien chop tat led voi tan so cao
/*	if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2)) {
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
	} else {
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
	}*/
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
