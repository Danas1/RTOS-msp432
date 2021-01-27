#include <stdint.h>
#include "msp.h"
#include "bsp.h"
#include "miros.h"
/*
Main program to test the rtos for the msp432.
Includes how to create a thread for the rtos.
 */

uint32_t stack_blink_blue[40];
OSThread blink_blue;
void main_blink_blue(void){
    while(1){
        BSP_ledBlueOn();
        BSP_delay( BSP_TICKS_PER_SEC / 4U);
        BSP_ledBlueOff();
        BSP_delay( BSP_TICKS_PER_SEC * 3U / 4U);
    }
}


uint32_t stack_blink_green[40];
OSThread blink_green;
void main_blink_green(void){
    while(1){
        BSP_ledGreenOn();
        BSP_delay( BSP_TICKS_PER_SEC / 4U);
        BSP_ledGreenOff();
        BSP_delay( BSP_TICKS_PER_SEC * 3U / 4U);
    }
}

uint32_t stack_blink_red[40];
OSThread blink_red;
void main_blink_red(void){
    while(1){
        BSP_delay( BSP_TICKS_PER_SEC / 4U);
        BSP_ledRedOn();
        BSP_delay( BSP_TICKS_PER_SEC * 3U/ 4U);
        BSP_ledRedOff();
    }
}
void main(void){
    BSP_init();
    OS_init();

    OSThread_start(&blink_blue, &main_blink_blue, stack_blink_blue, sizeof(stack_blink_blue));
    OSThread_start(&blink_green, &main_blink_green, stack_blink_green, sizeof(stack_blink_green));
    OSThread_start(&blink_red, &main_blink_red, stack_blink_red, sizeof(stack_blink_red));

    while(1){

    }
}

