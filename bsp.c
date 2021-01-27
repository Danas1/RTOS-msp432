/*
 * bsp.c
 *
 *  Created on: 31 Dec 2020
 *      Author: Danas
 *
 * Board Support Package for MSP432p401r launchpad
 */

#include <stdint.h>


#include "msp.h"
#include "bsp.h"
#include "miros.h"

/* leds on-board (port 2 wired) */
#define LED_RED     (1U << 0)
#define LED_GREEN   (1U << 1)
#define LED_BLUE    (1U << 2)

/*
 *
 *
 */

static uint32_t volatile l_tickCtr;

/*
 * Systick Functions
 */


void SysTick_Handler(void){
    ++l_tickCtr;

    __disable_irq();
    OS_sched();
    __enable_irq();
}

uint32_t BSP_tickCtr(void){
    uint32_t tickCtr;

    __disable_irq();
    tickCtr = l_tickCtr;
    __enable_irq();

    return tickCtr;
}

void BSP_delay( uint32_t ticks){
    uint32_t start = BSP_tickCtr();
    while ( (BSP_tickCtr() - start) < ticks){
    }
}


/*
 * Led control functions.
 */
void BSP_ledRedOn(void){
    BITBAND_PERI(P2OUT,0) = 1;
}

void BSP_ledRedOff(){
    BITBAND_PERI(P2OUT,0) = 0;
}

void BSP_ledGreenOn(void){
    BITBAND_PERI(P2OUT,1) = 1;
}

void BSP_ledGreenOff(void){
    BITBAND_PERI(P2OUT,1) = 0;
}

void BSP_ledBlueOn(){
    BITBAND_PERI(P2OUT,2) = 1;
}

void BSP_ledBlueOff(){
    BITBAND_PERI(P2OUT,2) = 0;
}

/*
 * Init functions.
 */

void BSP_init(){
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;       // stop watchdog timer
    BSP_init_leds();
    BSP_init_systick();
    //NVIC_SetPriority (PendSV_IRQn, 1U << 1);
    //__enable_irq();
}


void BSP_init_leds(){
    /*Leds are wired to P2, bits 0 to 2 */
    P2DIR |= (BIT0 | BIT1 | BIT2);  /* Define the 3 first bits of port 2 (led) as output */
    P2OUT &= 0x00;                  /* Turn off led */
}

void BSP_init_systick(void){
    /*Variables configuration */
    l_tickCtr = 0;
    /*Systick Configuration */
    //SysTick_Config(SystemCoreClock / 1000); /* Fast config of systick and interrupt */

    SysTick->CTRL = 0;                              /* Systick disable */
    SysTick->LOAD = (SystemCoreClock / BSP_TICKS_PER_SEC) - 1U; /* Systick reload value */
    NVIC_SetPriority(SysTick_IRQn, 2);
    SysTick->VAL = 0;                               /* Reset Systick Value */
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |    // Source main clock (by default 3Mhz)
                         SysTick_CTRL_TICKINT_Msk | // Enable interrupt
                         SysTick_CTRL_ENABLE_Msk;   // Enable Systick.
}

void BSP_init_timer(){
    /*Timer A Configuration */
    TA0CTL = (TASSEL_2 | //SMCLK (3Mhz default, config system_msp432 to 46.875khz)
             ID_3 | //Divided by 8.(5859 hz)
             TAIE | // Enable interrupt of timer A.
             MC_1 ); //Count up to  TA0CCR0
    TA0CCR0 = 5894; // 0xB71B
    TA0CCTL0 = CCIE; // Enable capture interrupt.
    TA0IV = 0x02; // Interrupt Source
}


