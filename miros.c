/*
 * miros.c
 *
 *  Created on: 9 Jan 2021
 *      Author: Danas
 */
#include <stdint.h>
#include "miros.h"
#include "msp.h"

OSThread * volatile OS_curr; /* Pointer to the current thread */
OSThread * volatile OS_next; /* Pointer to the next thread */

OSThread *OS_thread[32 + 1]; /* array of thread started */
uint8_t OS_threadNum;        /* Number of thread started */
uint8_t OS_currIdx;          /* Current thread started */

void OS_init(){
    NVIC_EnableIRQ(PendSV_IRQn);
    NVIC_SetPriority(PendSV_IRQn, 7);
    /* The priority of PendSV should be lower
     * than every other thread. (Higher number)
     */

}

void OS_run(){
      __enable_irq();
}
void OS_sched(void) {    
    /* OS_next */
    ++OS_currIdx;
    if (OS_currIdx == OS_threadNum) {
        OS_currIdx = 0;
    }
    OS_next = OS_thread[OS_currIdx];
    
    
    if (OS_next != OS_curr){
        /* Set PendSV*/
        *(uint32_t volatile *)0xE000ED04 = (1U << 28);
    }
}
void OSThread_start(
        OSThread *me,
        OSThreadHandler threadHandler,
        void *stkSto, uint32_t stkSize)
{
    uint32_t *sp = (uint32_t *)((((uint32_t)stkSto + stkSize) / 8) * 8);
    /* Division and multiplication by  8 is used to ensure that the stack
     * is alligned.
     */
    uint32_t *stk_limit;

    /* fabricate cortex-m stack frame for blnk_green */
    *(--sp) = (1U << 24); /*xPSR thumb bit*/
    *(--sp) = (uint32_t) threadHandler; /*PC*/
    *(--sp) = 0x000000EU; /* LR */
    *(--sp) = 0x000000CU; /* R12 */
    *(--sp) = 0x0000003U; /* R3 */
    *(--sp) = 0x0000002U; /* R2 */
    *(--sp) = 0x0000001U; /* R1 */
    *(--sp) = 0x0000000U; /* R0 */
    /* additionally, to backup registers R4-R11 */
    *(--sp) = 0x000000BU; /* R11 */
    *(--sp) = 0x000000AU; /* R10 */
    *(--sp) = 0x0000009U; /* R9 */
    *(--sp) = 0x0000008U; /* R8 */
    *(--sp) = 0x0000007U; /* R7 */
    *(--sp) = 0x0000006U; /* R6 */
    *(--sp) = 0x0000005U; /* R5 */
    *(--sp) = 0x0000004U; /* R4 */

    /*save the top of the stack in the thread attribute. */
    me->sp = sp;

    /* round up the bottom of the stack to the 8 byte boundary */
    stk_limit = (uint32_t*)(((((uint32_t)stkSto - 1U) / 8) + 1U) * 8);

    /* pre-fill unused part of the stack */
    for( sp = sp - 1U; sp >= stk_limit; --sp) {
        *sp = 0xDEADBEEFU;
    }
    
    
    /* register the thread with the OS */
    OS_thread[OS_threadNum] = me;
    ++OS_threadNum;
    
}


__stackless
void PendSV_Handler(){
 __asm volatile (
    /* __disable_irq(); */
    "  CPSID         I                 \n"

    /* if (OS_curr != (OSThread *)0) { */
    "  LDR           r1,=OS_curr       \n"
    "  LDR           r1,[r1,#0x00]     \n"
    "  CBZ           r1,PendSV_restore \n"

    /*     push registers r4-r11 on the stack */
    "  PUSH          {r4-r11}          \n"

    /*     OS_curr->sp = sp; */
    "  LDR           r1,=OS_curr        \n"
    "  LDR           r1,[r1,#0x00]      \n"
    "  STR           sp,[r1,#0x00]      \n"
    /* } */

    "PendSV_restore:                    \n"
    /* sp = OS_next->sp; */
    "  LDR           r1,=OS_next        \n"
    "  LDR           r1,[r1,#0x00]      \n"
    "  LDR           sp,[r1,#0x00]      \n"

    /* OS_curr = OS_next; */
    "  LDR           r1,=OS_next        \n"
    "  LDR           r1,[r1,#0x00]      \n"
    "  LDR           r2,=OS_curr        \n"
    "  STR           r1,[r2,#0x00]      \n"

    /* pop registers r4-r11 */
    "  POP           {r4-r11}           \n"

    /* __enable_irq(); */
    "  CPSIE         I                  \n"

    /* return to the next thread */
    "  BX            lr                 \n"
    );
}
