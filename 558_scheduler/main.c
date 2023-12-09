/*
 * hello
 *
 * Copyright (C) 2022 Texas Instruments Incorporated
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/******************************************************************************
 *
 * The hello example project illustrates FreeRTOS's vTaskCreate, vTaskDelay and
 * vTaskDelete APIs.  A task is created to print the "Hello World!" message
 * five times on a terminal window at an one second interval.  After five
 * seconds, the task is then deleted.
 *
 * main() creates one task.  It then starts the scheduler.
 *
 * The Hello task creates a simple task that will output a 'Hello world!'
 * message over UART five times before self-terminating.
 
 * This example uses UARTprintf for output of UART messages.  UARTprintf is not
 * a thread-safe API and is only being used for simplicity of the demonstration
 * and in a controlled manner.
 *
 * Open a terminal with 115,200 8-N-1 to see the output for this demo.
 *
 */

/* Standard includes. */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Hardware includes. */
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "drivers/rtos_hw_drivers.h"
#include "utils/uartstdio.h"



#include "Timer.h"
#include "lcd.h"
#include "uart.h"

// Uncomment or add any include directives that are needed
#include "open_interface.h"
#include "movement.h"
//#include "button.h"
#include "resetSimulation.h"
#include "scan.h"
#include "cliff.h"
#include "adc.h"
/*----------------DEFINES----------------------*/

#define SCHED_POLICY_EDF 0
#define SCHED_POLICY_RMS 1
#define SCHED_POLICY SCHED_POLICY_RMS

/* Aperiodic server type
 * 0: no aperiodic server
 * 1: polling server
 * 2: deferrable server */
#define APERIODIC_SERVER_TYPE 2

/* The number of tasks, including the any server */
#define NUM_TASKS 4
/* To add a new task, increment NUM_TASKS, add its period below,
 * add its function implementation later in this file, add its
 * function prototype with the others, and add it to the list
 * of tasks defined in main(). For the index it gets added to
 * the list, add that with the other task indices. */
/*-----------------------------------------------------------*/

/* Task periods */

/* The period of the first task, specified in milliseconds, and
converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainTASK1_PERIOD_MS                 pdMS_TO_TICKS( 5000 )//5 seconds to move and scan

/* The period of the second task, specified in milliseconds, and
converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainTASK2_PERIOD_MS                 pdMS_TO_TICKS( 2500 )

/* The period of the third task, specified in milliseconds, and
converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainTASK3_PERIOD_MS                 pdMS_TO_TICKS( 1700 )

/* The period of the deferrable server, specified in milliseconds, and
converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainDEFERRABLE_SERVER_PERIOD_MS     pdMS_TO_TICKS( 1100 )

/* The period of the polling server, specified in milliseconds, and
converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainPOLLING_SERVER_PERIOD_MS     pdMS_TO_TICKS( 1100 )
/*-----------------------------------------------------------*/

/* Task indices */

/* The index of the first task in the list of tasks */
#define mainTASK1_INDEX 0

/* The index of the second task in the list of tasks */
#define mainTASK2_INDEX 1

/* The index of the third task in the list of tasks */
#define mainTASK3_INDEX 2

/* The index of the deferrable server in the list of tasks */
#define mainDEFERRABLE_SERVER_INDEX 3

/* The index of the polling server in the list of tasks */
#define mainPOLLING_SERVER_INDEX 3
/*-----------------------------------------------------------*/



typedef   int (*Function_Pointer)( void );

struct periodic_task
{
    char *name;             // task name
    TickType_t  p_i;        // period of task in ticks
    UBaseType_t priority;   // priority of task (set by scheduling policy)
    Function_Pointer func;  // pointer to the function which the task will run
    TaskHandle_t handle;    // handle to the task
    TickType_t start_time;  // time that the task became unblocked, i.e. entered it's next period
};

/*-----------------------------------------------------------*/

/*
 * TODO: Implement this function for any hardware specific clock configuration
 * that was not already performed before main() was called.
 */
static void prvSetupHardware( void );

/*
 * The three task function prototypes.
 */
static void prvTask1( void *pvParameters );
static void prvTask2( void *pvParameters );
static void prvTask3( void *pvParameters );

/* The aperiodic server function prototypes */
static void prvDeferrableServer( void *pvParameters );
static void prvPollingServer( void *pvParameters );

/* The EDF Scheduler function prototype */
static void prvEDFScheduler( void *pvParameters );

/* This function sets up UART0 to be used for a console to display information
 * as the example is running. */
//static void prvConfigureUART(void);


/*-----------------------------------------------------------*/

int main( void )
{
    /* Configure the system ready to run the demo.  The clock configuration
    can be done here if it was not done before main() was called. */
    prvSetupHardware();

    timer_init(); // Initialize Timer, needed before any LCD screen functions can be called
                     // and enables time functions (e.g. timer_waitMillis)
    lcd_init();
    int i, tasks_assigned;

    /* Create the list of tasks */
    struct periodic_task tasks[NUM_TASKS] = {
        {
            .name = "Task1",
            .p_i = mainTASK1_PERIOD_MS,
            .priority = -1,
            .func = prvTask1
        },
        {
            .name = "Task2",
            .p_i = mainTASK2_PERIOD_MS,
            .priority = -1,
            .func = prvTask2
        },
        {
            .name = "Task3",
            .p_i = mainTASK3_PERIOD_MS,
            .priority = -1,
            .func = prvTask3
        },
        { //TODO: add if statement to switch between this and polling server
            .name = "Deferrable Server",
            .p_i = mainDEFERRABLE_SERVER_PERIOD_MS,
            .priority = -1,
            .func = prvDeferrableServer
        }
    };

    /* Set task priorities based on scheduling policy */
    if ( SCHED_POLICY == SCHED_POLICY_RMS || SCHED_POLICY == SCHED_POLICY_EDF )
    {
        /* in RMS scheduling, the task with the shortest period always has the
         * highest priority. This is static and never changes. */
        TickType_t tempTaskPeriods[NUM_TASKS]; // temporary array to store task periods for sorting
        for ( i = 0; i < NUM_TASKS; i++)
        {
            tempTaskPeriods[i] = tasks[i].p_i;
        }
        for ( tasks_assigned = 0; tasks_assigned < NUM_TASKS; tasks_assigned++)
        {
            /* find the task with the shortest period */
            TickType_t shortest_period = tempTaskPeriods[0];
            int shortest_period_index = 0;
            for ( i = 1; i < NUM_TASKS; i++)
            {
                if (tempTaskPeriods[i] < shortest_period && tempTaskPeriods[i] != -1)
                { /* If the task period is shorter and available (not already used) */
                    shortest_period = tempTaskPeriods[i];
                    shortest_period_index = i;
                }
            }

            /* assign the task with the shortest period the highest priority */
            /* leave one upper priority level unassigned for EDF, for RMS it goes unused */
            tasks[shortest_period_index].priority = ( configMAX_PRIORITIES - 2 ) - tasks_assigned;

            /* remove the task with the shortest period from the list */
            tempTaskPeriods[shortest_period_index] = -1;
        }
    }
        else
    {
        /* invalid scheduling policy */
        while(1);
    }

    /* Create the tasks */
    for ( i = 0; i < NUM_TASKS; i++)//forgive me for the spaghetti code you're about to see but the func values don't seem to work unless you hard code them (prob c shenanigans)
    {
        if(tasks[i].name == "Task1" )
            xTaskCreate( prvTask1, tasks[i].name, configMINIMAL_STACK_SIZE, (void *) tasks, tasks[i].priority, &tasks[i].handle );
        else if(tasks[i].name == "Task2")
            xTaskCreate( prvTask2, tasks[i].name, configMINIMAL_STACK_SIZE, (void *) tasks, tasks[i].priority, &tasks[i].handle );
        else if(tasks[i].name == "Task3")
            xTaskCreate( prvTask3, tasks[i].name, configMINIMAL_STACK_SIZE, (void *) tasks, tasks[i].priority, &tasks[i].handle );
        else if(tasks[i].name == "Deferrable Server")
            xTaskCreate( prvDeferrableServer, tasks[i].name, configMINIMAL_STACK_SIZE, (void *) tasks, tasks[i].priority, &tasks[i].handle );
        else if(tasks[i].name == "Polling Server")
            xTaskCreate( prvPollingServer, tasks[i].name, configMINIMAL_STACK_SIZE, (void *) tasks, tasks[i].priority, &tasks[i].handle );
    }
//    xTaskCreate( prvTask3, "test", configMINIMAL_STACK_SIZE, NULL, 14, NULL );
//    xTaskCreate( prvTask2, "test2", configMINIMAL_STACK_SIZE, NULL, 15, NULL );
    lcd_putc('t');

    /* Start the tasks and timer running. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details.  */
    for( ;; );
}
/*-------------------TASKS---------------------*/

static void prvTask1( void *pvParameters )
{
    TickType_t xNextWakeTime;
    int this_task_index = mainTASK1_INDEX;
    TickType_t this_task_period = mainTASK1_PERIOD_MS;
    int i;
//    char str[50];

    /* All tasks will be passed the periodic task list in their parameters */
    struct periodic_task *tasks = (struct periodic_task *) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    /* set the initial start time for this task 
     * note that there is no need to call the EDF
     * scheduler because until the first task completes
     * the EDF and RMS schedules are identical */
    tasks[this_task_index].start_time = xNextWakeTime;

    for( ;; )
    {
        UARTprintf("Task 2 Start\n");
        for(i = 0; i < 70; i++)
        {
            UARTprintf("Task 2 currently at %d\n", i);
            timer_waitMillis(400);
        }
        UARTprintf("Task 2 done\n");
        timer_waitMillis(400);

        if ( SCHED_POLICY == SCHED_POLICY_EDF )
        {
            /* update the initial start time for this task */
            tasks[this_task_index].start_time = xNextWakeTime + this_task_period;

            /* Call the EDF Scheduler to re-arrange priorities based on this new start time.
            * We must temporarily raise our priority to the highest priority so that we can
            * ensure that the scheduling is uninterrupted. */
            vTaskPrioritySet( tasks[this_task_index].handle, ( configMAX_PRIORITIES - 1 ) );
            prvEDFScheduler( tasks, this_task_index );
            vTaskPrioritySet( tasks[this_task_index].handle, tasks[this_task_index].priority );
        }

        /* Place this task in the blocked state until it is time to run again.
        The block time is specified in ticks, the constant used converts ticks
        to ms.  The task will not consume any CPU time while it is in the
        Blocked state. */
        vTaskDelayUntil( &xNextWakeTime, this_task_period );
    }
}
/*-----------------------------------------------------------*/

static void prvTask2( void *pvParameters )
{
    TickType_t xNextWakeTime;
    int this_task_index = mainTASK2_INDEX;
    TickType_t this_task_period = mainTASK2_PERIOD_MS;
    int i;
//    char str[50];

    /* All tasks will be passed the periodic task list in their parameters */
    struct periodic_task *tasks = (struct periodic_task *) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    /* set the initial start time for this task 
     * note that there is no need to call the EDF
     * scheduler because until the first task completes
     * the EDF and RMS schedules are identical */
    tasks[this_task_index].start_time = xNextWakeTime;

    for( ;; )
    {
        UARTprintf("Task 2 Start\n");
        for(i = 0; i < 70; i++)
        {
            UARTprintf("Task 2 currently at %d\n", i);
            timer_waitMillis(400);
        }
        UARTprintf("Task 2 done\n");
        timer_waitMillis(400);
        
        if ( SCHED_POLICY == SCHED_POLICY_EDF )
        {
            /* update the initial start time for this task */
            tasks[this_task_index].start_time = xNextWakeTime + this_task_period;

            /* Call the EDF Scheduler to re-arrange priorities based on this new start time.
            * We must temporarily raise our priority to the highest priority so that we can
            * ensure that the scheduling is uninterrupted. */
            vTaskPrioritySet( tasks[this_task_index].handle, ( configMAX_PRIORITIES - 1 ) );
            prvEDFScheduler( tasks, this_task_index );
            vTaskPrioritySet( tasks[this_task_index].handle, tasks[this_task_index].priority );
        }

        /* Place this task in the blocked state until it is time to run again.
        The block time is specified in ticks, the constant used converts ticks
        to ms.  The task will not consume any CPU time while it is in the
        Blocked state. */
        vTaskDelayUntil( &xNextWakeTime, this_task_period );
    }
}
/*-----------------------------------------------------------*/

static void prvTask3( void *pvParameters )
{
    TickType_t xNextWakeTime;
    int this_task_index = mainTASK3_INDEX;
    TickType_t this_task_period = mainTASK3_PERIOD_MS;
    int i;
//    char str[50];

    /* All tasks will be passed the periodic task list in their parameters */
    struct periodic_task *tasks = (struct periodic_task *) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    /* set the initial start time for this task 
     * note that there is no need to call the EDF
     * scheduler because until the first task completes
     * the EDF and RMS schedules are identical */
    tasks[this_task_index].start_time = xNextWakeTime;

    for( ;; )
    {
        UARTprintf("Task 3 Start\n");
        for(i = 0; i < 50; i++)
        {
            UARTprintf("Task 3 currently at %d\n", i);
            timer_waitMillis(400);
        }
        UARTprintf("Task 3 done\n");
        timer_waitMillis(400);
        
        if ( SCHED_POLICY == SCHED_POLICY_EDF )
        {
            /* update the initial start time for this task */
            tasks[this_task_index].start_time = xNextWakeTime + this_task_period;

            /* Call the EDF Scheduler to re-arrange priorities based on this new start time.
            * We must temporarily raise our priority to the highest priority so that we can
            * ensure that the scheduling is uninterrupted. */
            vTaskPrioritySet( tasks[this_task_index].handle, ( configMAX_PRIORITIES - 1 ) );
            prvEDFScheduler( tasks, this_task_index );
            vTaskPrioritySet( tasks[this_task_index].handle, tasks[this_task_index].priority );
        }

        /* Place this task in the blocked state until it is time to run again.
        The block time is specified in ticks, the constant used converts ticks
        to ms.  The task will not consume any CPU time while it is in the
        Blocked state. */
        vTaskDelayUntil( &xNextWakeTime, this_task_period );
    }
}
/*-----------------------------------------------------------*/

static void prvDeferrableServer( void *pvParameters )
{
    TickType_t xNextWakeTime;
    int this_task_index = mainDEFERRABLE_SERVER_INDEX;
    TickType_t this_task_period = mainDEFERRABLE_SERVER_PERIOD_MS;
    int i;
//    char str[50];

    /* All tasks will be passed the periodic task list in their parameters */
    struct periodic_task *tasks = (struct periodic_task *) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    /* set the initial start time for this task 
     * note that there is no need to call the EDF
     * scheduler because until the first task completes
     * the EDF and RMS schedules are identical */
    tasks[this_task_index].start_time = xNextWakeTime;

    for( ;; )
    {
        UARTprintf("Task def Start\n");
        for(i = 0; i < 5; i++)
        {
            UARTprintf("Task def currently at %d\n", i);
            timer_waitMillis(400);
        }
        UARTprintf("Task def done\n");
        timer_waitMillis(400);
        // TODO: check for aperiodic tasks
        // run aperiodic tasks if they exist

        if ( SCHED_POLICY == SCHED_POLICY_EDF )
        {
            /* update the initial start time for this task */
            tasks[this_task_index].start_time = xNextWakeTime + this_task_period;

            /* Call the EDF Scheduler to re-arrange priorities based on this new start time.
            * We must temporarily raise our priority to the highest priority so that we can
            * ensure that the scheduling is uninterrupted. */
            vTaskPrioritySet( tasks[this_task_index].handle, ( configMAX_PRIORITIES - 1 ) );
            prvEDFScheduler( tasks, this_task_index );
            vTaskPrioritySet( tasks[this_task_index].handle, tasks[this_task_index].priority );
        }

        /* Place this task in the blocked state until it is time to run again.
        The block time is specified in ticks, the constant used converts ticks
        to ms.  The task will not consume any CPU time while it is in the
        Blocked state. */
        vTaskDelayUntil( &xNextWakeTime, this_task_period );
    }
}
/*-----------------------------------------------------------*/

static void prvPollingServer( void *pvParameters )
{
    TickType_t xNextWakeTime;
    int this_task_index = mainPOLLING_SERVER_INDEX;
    TickType_t this_task_period = mainPOLLING_SERVER_PERIOD_MS;
    int i;
//    char str[50];

/* All tasks will be passed the periodic task list in their parameters */
    struct periodic_task *tasks = (struct periodic_task *) pvParameters; 

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    /* set the initial start time for this task 
     * note that there is no need to call the EDF
     * scheduler because until the first task completes
     * the EDF and RMS schedules are identical */
    tasks[this_task_index].start_time = xNextWakeTime;


    for( ;; )
    {
        UARTprintf("Task pol Start\n");
        for(i = 0; i < 5; i++)
        {
            UARTprintf("Task pol currently at %d\n", i);
            timer_waitMillis(400);
        }
        UARTprintf("Task pol done\n");
        timer_waitMillis(400);
        // TODO: check for aperiodic tasks
        // run aperiodic tasks if they exist

        if ( SCHED_POLICY == SCHED_POLICY_EDF )
        {
            /* update the initial start time for this task */
            tasks[this_task_index].start_time = xNextWakeTime + this_task_period;

            /* Call the EDF Scheduler to re-arrange priorities based on this new start time.
            * We must temporarily raise our priority to the highest priority so that we can
            * ensure that the scheduling is uninterrupted. */
            vTaskPrioritySet( tasks[this_task_index].handle, ( configMAX_PRIORITIES - 1 ) );
            prvEDFScheduler( tasks, this_task_index );
            vTaskPrioritySet( tasks[this_task_index].handle, tasks[this_task_index].priority );
        }

        /* Place this task in the blocked state until it is time to run again.
        The block time is specified in ticks, the constant used converts ticks
        to ms.  The task will not consume any CPU time while it is in the
        Blocked state. */
        vTaskDelayUntil( &xNextWakeTime, this_task_period );
    }
}
/*-----------------------------------------------------------*/

static void prvEDFScheduler( struct periodic_task *tasks, int calling_task_index )
{
    /* The EDF Scheduler will be invoked by each task when they 
     * update their start times. The EDF Scheduler will then 
     * assign new priorities, where the highest priority goes
     * to the smallest start_time + p_i */

    int tasks_assigned[ NUM_TASKS ] = { 0 }; // array to keep track of which tasks have been assigned a priority

    for (int i = 0; i < NUM_TASKS; i++)
    {
        /* find the task with the earliest deadline */
        TickType_t earliest_deadline;
        int earliest_deadline_index;

        /* first find the first task that is unassigned */
        for (int j = 0; j < NUM_TASKS; j++)
        {
            if (!tasks_assigned[j])
            {
                earliest_deadline = tasks[j].start_time + tasks[j].p_i;
                earliest_deadline_index = j;
                break;
            }
        }

        /* now check if there are any other unassigned tasks that have an earlier deadline */
        for (int j = earliest_deadline_index + 1; j < NUM_TASKS; j++)
        {
            if (tasks[j].start_time + tasks[j].p_i < earliest_deadline && !tasks_assigned[j])
            {
                earliest_deadline = tasks[j].start_time + tasks[j].p_i;
                earliest_deadline_index = j;
            }
        }

        /* Assign the task with the earliest deadline the highest priority.
        * It is critical to use configMAX_PRIORITIES - 2 as we are temporarily 
        * assigning the current task to the highest priority so that this 
        * scheduler is not interrupted. */
        tasks[earliest_deadline_index].priority = ( configMAX_PRIORITIES - 2 ) - i;
        tasks_assigned[earliest_deadline_index] = 1;
    }

    /* set the priorities of the tasks */
    for (int i = 0; i < NUM_TASKS; i++)
    {
        if (i != calling_task_index)
        { /* DO NOT SET THE PRIORITY OF THE CALLING TASK, IT 
           * IS TERMPORARILY THE HIGHEST PRIORITY SO THAT 
           * SCHEDULING IS UNINTERRUPTED. IT MUST SET IT'S OWN
           * PRIORITY AFTER CALLING THIS FUNCTION */
            vTaskPrioritySet( tasks[i].handle, tasks[i].priority );
        }
    }

}
/*-----------------------------------------------------------*/










static void prvConfigureUART(void)
{
    /* Enable GPIO port A which is used for UART0 pins.
     * TODO: change this to whichever GPIO port you are using. */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    /* Configure the pin muxing for UART0 functions on port A0 and A1.
     * This step is not necessary if your part does not support pin muxing.
     * TODO: change this to select the port/pin you are using. */
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    /* Enable UART0 so that we can configure the clock. */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    /* Use the internal 16MHz oscillator as the UART clock source. */
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    /* Select the alternate (UART) function for these pins.
     * TODO: change this to select the port/pin you are using. */
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    /* Initialize the UART for console I/O. */
    UARTStdioConfig(0, 115200, 16000000);
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
    /* Run from the PLL at 80 MHz.  Any updates to the PLL rate here would
     * need to be reflected in FreeRTOSConfig.h by updating the value of
     * configCPU_CLOCK_HZ with the new system clock frequency. */
    MAP_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_INT |
                       SYSCTL_XTAL_16MHZ);

    /* Configure device pins. */
    PinoutSet(false);

    /* Configure UART0 to send messages to terminal. */
    prvConfigureUART();
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
    /* vApplicationMallocFailedHook() will only be called if
    configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
    function that will get called if a call to pvPortMalloc() fails.
    pvPortMalloc() is called internally by the kernel whenever a task, queue,
    timer or semaphore is created.  It is also called by various parts of the
    demo application.  If heap_1.c or heap_2.c are used, then the size of the
    heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
    FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
    to query the size of free heap space that remains (although it does not
    provide information on how the remaining heap might be fragmented). */
    IntMasterDisable();
    for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
    /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
    to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
    task.  It is essential that code added to this hook function never attempts
    to block in any way (for example, call xQueueReceive() with a block time
    specified, or call vTaskDelay()).  If the application makes use of the
    vTaskDelete() API function (as this demo application does) then it is also
    important that vApplicationIdleHook() is permitted to return to its calling
    function, because it is the responsibility of the idle task to clean up
    memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */
    IntMasterDisable();
    for( ;; );
}
/*-----------------------------------------------------------*/

//-------------Conflicts with malloc in the compiler files, commenting out to make it shut up

//void *malloc( size_t xSize )
//{
//    /* There should not be a heap defined, so trap any attempts to call
//    malloc. */
//    IntMasterDisable();
//    for( ;; );
//}
/*-----------------------------------------------------------*/

