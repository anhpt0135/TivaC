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
#include "Timer_Config.h"
#include "Adc_hal.h"
#include "UART0.h"
#include "Nokia5110.h"
#include "Switch_led.h"
#include "PLL.h"


uint32_t debounce_switch(uint32_t pin);

void Draw_bullet(unsigned char xpos, unsigned char ypos);

const unsigned char Enemy10Point1[] = { 0x42, 0x4D, 0xC6, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
		0x10, 0x00, 0x00,
		0x00,  // width is 16 pixels
		0x0A, 0x00, 0x00,
		0x00,  // height is 10 pixels
		0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00,
		0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00,
		0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00,
		0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00,
		0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00,
		0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,
		0x00,  // bottom row
		0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0xF0, 0x00,
		0x00, 0x0F, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00,
		0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xF0, 0xFF,
		0xFF, 0x0F, 0xFF, 0x00, 0x00, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00,
		0x00, 0xF0, 0x0F, 0x00, 0x00, 0xF0, 0x0F, 0x00, 0x00, 0x00, 0xF0, 0x00,
		0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // top row
		0xFF };

const unsigned char Missile0[] = { 0x42, 0x4D, 0x9A, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x04,
		0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
		0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80,
		0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00,
		0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF,
		0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF,
		0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0xF0,
		0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x0F,
		0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0xFF };


const unsigned char startR[] = { 0x42, 0x4D, 0xD6, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x0C, 0x00,
		0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x23, 0x0B, 0x00, 0x00, 0x23, 0x0B,
		0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80,
		0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80,
		0x00, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00,
		0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00,
		0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF,
		0xFF, 0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x00, 0x00, 0x4F, 0xFF,
		0xFF, 0xFF, 0xFF, 0x4F, 0x00, 0x00, 0x4F, 0x00, 0x00, 0x00, 0x0F, 0xFF,
		0x00, 0x00, 0x4F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4F, 0x00,
		0x00, 0x00, 0x07, 0x77, 0x00, 0x00, 0x4F, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x4F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4F, 0x00,
		0x00, 0x00, 0x07, 0x77, 0x00, 0x00, 0x4F, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x4F, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0x00, 0x00, 0x4F, 0xFF,
		0xFF, 0xFF, 0xFF, 0x4F, 0x00, 0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
		0x00, 0x00, 0xFF,

};


volatile int adc_flag = 0;
uint16_t result;


void User_func(void) {
	GPIO_PORTF_DATA_R ^= 0x00;
}

int main(void) {
	char buff[10];
	uint8_t a_flag = 0;
	volatile unsigned char x_bullet = 0;
	unsigned char Xpos = 0;
	unsigned char Ypos = 0;
	int val = 0;
	// Khai bao bien
	// Cau hinh clock he thong
	int touched_sw1, touched_sw2;
	SysCtlClockSet(
	SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);//400/5 with 5 = 4 + 1, and fill 4 into bit 22
	//Kich hoat port F
	UART0_Init();
	Clear_screen();
	Dbg_info("Timer0A_init()");
	//Timer0A_Init();
	Timer0A_Init_new(8000000);	// 100 ms ==> 10 Hz
	//Timer3A_Init(User_func, 40000000, 2);
	Dbg_info("Initialize adc0 ss3");
	Adc0_SS3_sw_trigger_init();
	led_init();
	switch_init();
	sysTick_init();
	Nokia5110_Init();
	Nokia5110_Clear();
	Dbg_info("Enable all interrupt");
	IntMasterEnable();	// Kich hoat ngat master
	while (1) {
		if (adc_flag == 1) {
			if (((val++) % 10) == 0) {
				printf("====ADC output value = %d========\r\n", result);
			}
			adc_flag = 0;
			snprintf(buff, 10, "%d", result);
			Nokia5110_Clear();
//			if(a_flag){
			Nokia5110_ClearBuffer();
//				Nokia5110_PrintBMP(Xpos, 47, Enemy10Point1, 0);
//				Xpos = ((Xpos + 5) < 84) ? (Xpos + 5) : 0;
			Ypos = (result * 48) >> 12;	// /2^12
			Ypos = (Ypos < 9) ? 9 : Ypos;
			Nokia5110_PrintBMP(0, Ypos, Enemy10Point1, 0);
			Nokia5110_PrintBMP(60, 10, Enemy10Point1, 0);
			Nokia5110_PrintBMP(60, 20, Enemy10Point1, 0);
			Nokia5110_PrintBMP(60, 30, Enemy10Point1, 0);
			Nokia5110_PrintBMP(60, 40, Enemy10Point1, 0);
			Nokia5110_PrintBMP(60, 50, Enemy10Point1, 0);
			touched_sw1 = touch(GPIO_PIN_0);
			//touched_sw2 = touch(GPIO_PIN_4);
			GPIO_PORTF_DATA_R = (touched_sw1) ? GPIO_PORTF_DATA_R | GPIO_PIN_1 : GPIO_PORTF_DATA_R & ~GPIO_PIN_1;
			//GPIO_PORTF_DATA_R = (touched_sw2) ? GPIO_PORTF_DATA_R | GPIO_PIN_2 : GPIO_PORTF_DATA_R & ~GPIO_PIN_2;

			if(touched_sw1){
				Draw_bullet(x_bullet, Ypos);
				//Nokia5110_PrintBMP(x_bullet, Ypos, Missile0, 0);
			}
			Nokia5110_DisplayBuffer();
			x_bullet = (x_bullet > MAX_X)? 0 : x_bullet + 2;
//			}else{
//				Nokia5110_SetCursor(5, 4);
//				Nokia5110_OutString(buff);
//			}
//			a_flag ^= 1;
		}

/*		GPIO_PORTF_DATA_R = (touch(GPIO_PIN_0)) ? 0x02 : 0x00;*/
	}
}

//Chuong trinh ngat
void Timer0IntHandler(void) {
	// Xoa het cac ngat timer
	GPIO_PORTF_DATA_R ^= 0x00;
	result = Adc0_SS3_trigger_and_get_value_without_acknowledgment();
	ADC0_ISC_R |= ADC_ISC_IN3;	//Acknownledge the conversion
	//printf("++ressult == %d\r\n", result);
	adc_flag = 1;
	//x_bullet = x_bullet + 2;
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	//x_bullet = (x_bullet > MAX_X)? 0 : x_bullet + 2;
	//printf("adc_flag == %d\r\n", adc_flag);
}


uint32_t debounce_switch(uint32_t pin){
	uint32_t current, previous, time = 1000;//Delay 10ms
	current = GPIOPinRead(GPIO_PORTF_BASE, pin);
	while(time){
		previous = current;
		delay_systic(800);//Delay 10us
		current = GPIOPinRead(GPIO_PORTF_BASE, pin);
		if(current == previous){
			time--;
		} else {
			time = 1000;
			//previous = current;
		}
	}
	return current;
}

void Draw_bullet(unsigned char xpos, unsigned char ypos){
	Nokia5110_PrintBMP(xpos, ypos, startR, 0);
}
