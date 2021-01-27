/*
 * bsp.h
 *
 *  Created on: 31 Dec 2020
 *      Author: Danas
 */

#ifndef BSP_H_
#define BSP_H_

#define BSP_TICKS_PER_SEC 100U
#define SystemCoreClock (uint32_t) 3000000 /* Default Frequency for MSP432 */

//void Systick_Handler();
uint32_t BSP_tickCtr(void);
void BSP_delay(uint32_t ticks);

/*
 * Led control functions.
 */
void BSP_ledRedOn(void);
void BSP_ledRedOff(void);
void BSP_ledGreenOn(void);
void BSP_ledGreenOff(void);
void BSP_ledBlueOn(void);
void BSP_ledBlueOff(void);
/*
 * Init functions.
 */

void BSP_init(void);
void BSP_init_leds(void);
void BSP_init_timer(void);
void BSP_init_systick(void);
#endif /* BSP_H_ */
