// Timer3A.c
// Runs on LM4F120/TM4C123
// Use Timer3 in 32-bit periodic mode to request interrupts at a periodic rate
// Daniel Valvano
// Jan 1, 2020

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2020
  Program 7.5, example 7.6

 Copyright 2020 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
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

void (*PeriodicTask3)(void);   // user function

// ***************** Timer3A_Init ****************
// Activate Timer3 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
//          priority 0 (highest) to 7 (lowest)
// Outputs: none
void Timer3A_Init(void(*task)(void), uint32_t period, uint32_t priority){
  SYSCTL_RCGCTIMER_R |= 0x08;   // 0) activate TIMER3
  PeriodicTask3 = task;         // user function
  TIMER3_CTL_R = 0x00000000;    // 1) disable TIMER3A during setup
  TIMER3_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER3_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER3_TAILR_R = period-1;    // 4) reload value
  TIMER3_TAPR_R = 0;            // 5) bus clock resolution
  TIMER3_ICR_R = 0x00000001;    // 6) clear TIMER3A timeout flag
  TIMER3_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI8_R = (NVIC_PRI8_R&0x00FFFFFF)|(priority<<29); // priority  
// interrupts enabled in the main program after all devices initialized
// vector number 51, interrupt number 35
  NVIC_EN1_R = 1<<(35-32);      // 9) enable IRQ 35 in NVIC
  TIMER3_CTL_R = 0x00000001;    // 10) enable TIMER3A
}

void Timer3A_Handler(void){
  TIMER3_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER3A timeout
  (*PeriodicTask3)();                // execute user task
}

void Timer3A_Stop(void){
  NVIC_DIS1_R = 1<<(35-32);        // 9) disable interrupt 35in NVIC
  TIMER3_CTL_R = 0x00000000;       // 10) disable timer4A
}

void Timer0A_Init(void){
	uint32_t ui32Period;
	//Kich hoat timer0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);	//Cau hinh timer
	ui32Period = (SysCtlClockGet() /10) /2;		//Thiet lap gia tri cua chu ky dua tren tan so he thong
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);
	//Kich hoat ngat tai timer 0
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);//Enalbe Timeout Interrupt in IMR of Timer0a
	IntPrioritySet(INT_TIMER2A, 0x02);
	IntEnable(INT_TIMER0A);// Enable timer0a interrupt in NVIC
	TimerEnable(TIMER0_BASE, TIMER_A);	//Kich hoat timer0
}

void Timer2A_Init(uint32_t period){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
	TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet(TIMER2_BASE, TIMER_A, period -1);
	TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
	IntEnable(INT_TIMER2A);
	TimerEnable(TIMER2_BASE, TIMER_A);
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

