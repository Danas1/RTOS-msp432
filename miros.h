/*
 * miros.h
 *
 *  Created on: 9 Jan 2021
 *      Author: Danas
 */

#ifndef MIROS_H_
#define MIROS_H_

/*Thread control Block (TCB) */
typedef struct {
    void *sp; /* stack pointer */
    /* ... other attributes associated with a thread */
} OSThread;

typedef void (* OSThreadHandler)();
void OS_init(void);
void OS_run(void);
void OSThread_start(
        OSThread *me,
        OSThreadHandler threadHandler,
        void *stkSto, uint32_t stkSize);

/*OS_sched must be called with interrupts DISABLED */
void OS_sched(void);
#endif /* MIROS_H_ */
