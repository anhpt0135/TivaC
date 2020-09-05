/*
 * Switch_led.h
 *
 *  Created on: Aug 31, 2020
 *      Author: anhpt0135
 */

#ifndef SRC_SWITCH_LED_H_
#define SRC_SWITCH_LED_H_

void led_init();
void switch_init();
int touch(uint32_t pin);
uint32_t debouncing_switch_without_interrupt(uint32_t pin);
//int debouncing_switch_with_interrupt();

#endif /* SRC_SWITCH_LED_H_ */
