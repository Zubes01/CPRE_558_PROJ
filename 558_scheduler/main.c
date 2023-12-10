
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

 * This example uses uart_sendStr_A for output of UART messages.  uart_sendStr_A is not
 * a thread-safe API and is only being used for simplicity of the demonstration
 * and in a controlled manner.
 *
 * Open a terminal with 115,200 8-N-1 to see the output for this demo.
 *
 */

/* CHANGES DEC 10: 
 * sample aperiodic task implemented
 * deferrable and periodic server aperiodic task handling partially implemented
 * task list now allocated using dynamic memory allocation
 * other small changes here and there */

/* Standard includes. */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

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
#define SCHED_POLICY SCHED_POLICY_EDF

/* Aperiodic server type
 * 0: no aperiodic server
 * 1: polling server
 * 2: deferrable server */
#define SERVER_TYPE_NO_SERVER 0
#define SERVER_TYPE_POLLING_SERVER 1
#define SERVER_TYPE_DEFERRABLE_SERVER 2
#define APERIODIC_SERVER_TYPE SERVER_TYPE_DEFERRABLE_SERVER

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
#define mainTASK1_PERIOD_MS                  240 //5 seconds to move and scan

/* The period of the second task, specified in milliseconds, and
converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainTASK2_PERIOD_MS                  480 

/* The period of the third task, specified in milliseconds, and
converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainTASK3_PERIOD_MS                  960 

/* The period of the deferrable server, specified in milliseconds, and
converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainDEFERRABLE_SERVER_PERIOD_MS     1920 

/* The period of the polling server, specified in milliseconds, and
converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainPOLLING_SERVER_PERIOD_MS        100 //shaved a zero off
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

struct aperiodic_task
{
    char *name;             // task name
    uint c_i;               // computation time of task in ms
    Function_Pointer func;  // pointer to the function which the task will run
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
static void prvEDFScheduler( struct periodic_task *tasks, int calling_task_index );

/* This function sets up UART0 to be used for a console to display information
 * as the example is running. */
//static void prvConfigureUART(void);

/*----------GLOBALS-----------*/
int count = 0;
int aperiodic_tasks_this_cycle = 0;
static QueueHandle_t aperiodic_task_queue;
struct aperiodic_task sample_aperiodic_task = {
    .name = "sample\0",
    .c_i = 100, // 1/10 of a second
    .func = sample_aperiodic_task_func
};

//struct periodic_task tasks[NUM_TASKS] = {
//    {
//        .name = "Task1\0",
//        .p_i = mainTASK1_PERIOD_MS,
//        .start_time = 0,
//        .priority = -1,
//        .func = prvTask1
//    },
//    {
//        .name = "Task2\0",
//        .p_i = mainTASK2_PERIOD_MS,
//        .start_time = 0,
//        .priority = -1,
//        .func = prvTask2
//    },
//    {
//        .name = "Task3\0",
//        .p_i = mainTASK3_PERIOD_MS,
//        .start_time = 0,
//        .priority = -1,
//        .func = prvTask3
//    },
//    { //TODO: add if statement to switch between this and polling server
//        .name = "d\0",
//        .p_i = mainDEFERRABLE_SERVER_PERIOD_MS,
//        .start_time = 0,
//        .priority = -1,
//        .func = prvDeferrableServer
//    }
//};
/*-----------------------------------------------------------*/

int main( void )
{
    /* Configure the system ready to run the demo.  The clock configuration
    can be done here if it was not done before main() was called. */
//    prvSetupHardware();

//    timer_init(); // Initialize Timer, needed before any LCD screen functions can be called
//                     // and enables time functions (e.g. //timer_waitMillis)
//    lcd_init();

//    uart_init();
    int i, tasks_assigned;

    /* Create the queue for aperiodic tasks if an aperiodic server is desired */
    if ( APERIODIC_SERVER_TYPE !=  0 )
    {
        aperiodic_task_queue = xQueueCreate( 10, sizeof( struct aperiodic_task ) );
    }

    /* Create the list of tasks */
    // struct periodic_task tasks[NUM_TASKS] = {
    //     {
    //         .name = "Task1\0",
    //         .p_i = mainTASK1_PERIOD_MS,
    //         .start_time = 0,
    //         .priority = -1,
    //         .func = prvTask1
    //     },
    //     {
    //         .name = "Task2\0",
    //         .p_i = mainTASK2_PERIOD_MS,
    //         .start_time = 0,
    //         .priority = -1,
    //         .func = prvTask2
    //     },
    //     {
    //         .name = "Task3\0",
    //         .p_i = mainTASK3_PERIOD_MS,
    //         .start_time = 0,
    //         .priority = -1,
    //         .func = prvTask3
    //     },
    //     { //TODO: add if statement to switch between this and polling server
    //         .name = "d\0",
    //         .p_i = mainDEFERRABLE_SERVER_PERIOD_MS,
    //         .start_time = 0,
    //         .priority = -1,
    //         .func = prvDeferrableServer
    //     }
    // };

    struct periodic_task *tasks;
    tasks = pvPortMalloc( NUM_TASKS * sizeof( periodic_task ) );

    tasks[0].name = pvPortMalloc( 12 * sizeof( char ) ); 
    tasks[0].name = "Task1\0";
    tasks[0].p_i = mainTASK1_PERIOD_MS;
    tasks[0].start_time = 0;
    tasks[0].priority = -1;
    tasks[0].func = prvTask1;

    tasks[1].name = pvPortMalloc( 12 * sizeof( char ) );
    tasks[1].name = "Task2\0";
    tasks[1].p_i = mainTASK2_PERIOD_MS;
    tasks[1].start_time = 0;
    tasks[1].priority = -1;
    tasks[1].func = prvTask2;

    tasks[2].name = pvPortMalloc( 12 * sizeof( char ) );
    tasks[2].name = "Task3\0";
    tasks[2].p_i = mainTASK3_PERIOD_MS;
    tasks[2].start_time = 0;
    tasks[2].priority = -1;
    tasks[2].func = prvTask3;

    if APERIODIC_SERVER_TYPE == SERVER_TYPE_POLLING_SERVER
    {
        tasks[3].name = pvPortMalloc( 12 * sizeof( char ) );
        tasks[3].name = "d\0";
        tasks[3].p_i = mainPOLLING_SERVER_PERIOD_MS;
        tasks[3].start_time = 0;
        tasks[3].priority = -1;
        tasks[3].func = prvPollingServer;
    }
    else if APERIODIC_SERVER_TYPE == SERVER_TYPE_DEFERRABLE_SERVER
    {
        tasks[3].name = pvPortMalloc( 12 * sizeof( char ) );
        tasks[3].name = "d\0";
        tasks[3].p_i = mainDEFERRABLE_SERVER_PERIOD_MS;
        tasks[3].start_time = 0;
        tasks[3].priority = -1;
        tasks[3].func = prvDeferrableServer;
    }
    else
    {
        while (1); // invalid server type
    }

//    struct periodic_task *tasks;
//    tasks = calloc( NUM_TASKS, sizeof(periodic_task) );
//    tasks[0].name = malloc()


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
        if(i == mainTASK1_INDEX )
        {
            xTaskCreate( prvTask1, tasks[i].name, configMINIMAL_STACK_SIZE, (void *) tasks, tasks[i].priority, &tasks[i].handle );
        }
        else if(i == mainTASK2_INDEX  ){
            xTaskCreate( prvTask2, tasks[i].name, configMINIMAL_STACK_SIZE, (void *) tasks, tasks[i].priority, &tasks[i].handle );
        }
        else if(i == mainTASK3_INDEX  ){
            xTaskCreate( prvTask3, tasks[i].name, configMINIMAL_STACK_SIZE, (void *) tasks, tasks[i].priority, &tasks[i].handle );
        }
        else if(i == mainDEFERRABLE_SERVER_INDEX && APERIODIC_SERVER_TYPE == SERVER_TYPE_DEFERRABLE_SERVER){
//            uart_sendStr_A("Defer deez nuts created\r\n");
            xTaskCreate( prvDeferrableServer, tasks[i].name, configMINIMAL_STACK_SIZE, (void *) tasks, tasks[i].priority, &tasks[i].handle );
        }
        else if(i == mainPOLLING_SERVER_INDEX && APERIODIC_SERVER_TYPE == SERVER_TYPE_POLLING_SERVER){
            xTaskCreate( prvPollingServer, tasks[i].name, configMINIMAL_STACK_SIZE, (void *) tasks, tasks[i].priority, &tasks[i].handle );
        }
        else
        {
            while(1); // invalid task index
//            lcd_putc('t');
        }
    }
//    xTaskCreate( prvTask3, "test", configMINIMAL_STACK_SIZE, NULL, 14, NULL );
//    xTaskCreate( prvTask2, "test2", configMINIMAL_STACK_SIZE, NULL, 15, NULL );
//    lcd_putc('t');

    /* Start the tasks and timer running. */
//    uart_sendStr_A("------------START-----------\r\n");
    static char cBuffer[ 80 ];
    vTaskList(cBuffer);
    printf(cBuffer);
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
    TickType_t test1, test2;


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
//        TickType_t xTimeNow;
//        TickType_t final;
//        xTimeNow = xTaskGetTickCount();
//        final = xTimeNow + pdMS_TO_TICKS( 672 );
//        while(final > xTimeNow)
//        {
//            xTimeNow = xTaskGetTickCount();
//        }

        /* given a 1/10 chance, put an aperiodic task onto the queue */
        if ( APERIODIC_SERVER_TYPE != SERVER_TYPE_NO_SERVER && rand() % 10 == 0 )
        {
            xQueueSend( aperiodic_task_queue, ( void * ) &sample_aperiodic_task, 0 );
            if ( APERIODIC_SERVER_TYPE == SERVER_TYPE_DEFERRABLE_SERVER && aperiodic_tasks_this_cycle == 0 )
            {
                /* if the aperiodic server type is a deferrable server, 
                 * and it is in the blocked state, and it has not yet
                 * executed an aperiodic task this cycle, we must wake
                 * the server so that it can execute the aperiodic task */
                TaskHandle_t deferrable_server_handle = xTaskGetHandle( "d\0" );
                xTaskAbortDelay( deferrable_server_handle );
                /* if it is not in the blocked state this shouldn't do anything */
            }
        }

        if ( SCHED_POLICY == SCHED_POLICY_EDF )
        {
            test1 = tasks[3].start_time;
            test2 = tasks[3].p_i;
            /* update the initial start time for this task */
            tasks[this_task_index].start_time = xNextWakeTime + this_task_period;

            /* Call the EDF Scheduler to re-arrange priorities based on this new start time.
            * We must temporarily raise our priority to the highest priority so that we can
            * ensure that the scheduling is uninterrupted. */
            test1 = tasks[3].start_time;
            test2 = tasks[3].p_i;
                  taskENTER_CRITICAL();
                  prvEDFScheduler( tasks, this_task_index );
                  taskEXIT_CRITICAL();



        }

        /* Place this task in the blocked state until it is time to run again.
        The block time is specified in ticks, the constant used converts ticks
        to ms.  The task will not consume any CPU time while it is in the
        Blocked state. */
//        lcd_printf("%d", count);
        count++;
        static char cBuffer[ 80 ];
        vTaskList(cBuffer);
        printf(cBuffer);
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
    int test = 0;

    test+= 1;

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
//        TickType_t xTimeNow;
//        TickType_t final;
//        xTimeNow = xTaskGetTickCount();
//        final = xTimeNow + pdMS_TO_TICKS( 500 );
//        while(final > xTimeNow)
//        {
//            xTimeNow = xTaskGetTickCount();
//        }

        if ( SCHED_POLICY == SCHED_POLICY_EDF )
        {
            /* update the initial start time for this task */
            tasks[this_task_index].start_time = xNextWakeTime + this_task_period;

            /* Call the EDF Scheduler to re-arrange priorities based on this new start time.
            * We must temporarily raise our priority to the highest priority so that we can
            * ensure that the scheduling is uninterrupted. */
                 taskENTER_CRITICAL();
                 prvEDFScheduler( tasks, this_task_index );
                 taskEXIT_CRITICAL();
        }

        /* Place this task in the blocked state until it is time to run again.
        The block time is specified in ticks, the constant used converts ticks
        to ms.  The task will not consume any CPU time while it is in the
        Blocked state. */
//        lcd_printf("%d", count);
        count++;
        static char cBuffer[ 100 ];
        vTaskList(cBuffer);
        printf(cBuffer);
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
    TickType_t test1, test2;
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
//        TickType_t xTimeNow;
//        TickType_t final;
//        xTimeNow = xTaskGetTickCount();
//        final = xTimeNow + pdMS_TO_TICKS( 500 );
//        while(final > xTimeNow)
//        {
//            xTimeNow = xTaskGetTickCount();
//        }

//        //timer_waitMillis(400);

        if ( SCHED_POLICY == SCHED_POLICY_EDF )
        {
            /* update the initial start time for this task */
            tasks[this_task_index].start_time = xNextWakeTime + this_task_period;

            /* Call the EDF Scheduler to re-arrange priorities based on this new start time.
            * We must temporarily raise our priority to the highest priority so that we can
            * ensure that the scheduling is uninterrupted. */
                 taskENTER_CRITICAL();
                 prvEDFScheduler( tasks, this_task_index );
                 taskEXIT_CRITICAL();
        }

        /* Place this task in the blocked state until it is time to run again.
        The block time is specified in ticks, the constant used converts ticks
        to ms.  The task will not consume any CPU time while it is in the
        Blocked state. */
//        lcd_printf("%d", count);
        count++;
        static char cBuffer[ 100 ];
        vTaskList(cBuffer);
        printf(cBuffer);
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
//        TickType_t xTimeNow;
//        TickType_t final;
//        xTimeNow = xTaskGetTickCount();
//        final = xTimeNow + pdMS_TO_TICKS( 500 );
//        while(final > xTimeNow)
//        {
//            xTimeNow = xTaskGetTickCount();
//        }

//        //timer_waitMillis(400);

        /* Check the queue for an aperiodic task */
        struct aperiodic_task received_task;
        aperiodic_tasks_this_cycle = 0;
        if ( xQueueReceive( aperiodic_task_queue, &received_task, 0 ) == pdPASS )
        {
            /* If an aperiodic task was received, run it */
            aperiodic_tasks_this_cycle++;
            received_task.func();
        }
        //TODO: implement using the c_i value of the aperiodic task?

        if ( SCHED_POLICY == SCHED_POLICY_EDF )
        {
            /* update the initial start time for this task */
            tasks[this_task_index].start_time = xNextWakeTime + this_task_period;

            /* Call the EDF Scheduler to re-arrange priorities based on this new start time.
            * We must temporarily raise our priority to the highest priority so that we can
            * ensure that the scheduling is uninterrupted. */
            taskENTER_CRITICAL();
            prvEDFScheduler( tasks, this_task_index );
            taskEXIT_CRITICAL();
        }

        /* Place this task in the blocked state until it is time to run again.
        The block time is specified in ticks, the constant used converts ticks
        to ms.  The task will not consume any CPU time while it is in the
        Blocked state. */
//        lcd_printf("%d", count);
        count++;
        static char cBuffer[ 100 ];
        vTaskList(cBuffer);
        printf(cBuffer);
        vTaskDelayUntil( &xNextWakeTime, this_task_period );
    }
}
/*-----------------------------------------------------------*/

static void prvPollingServer( void *pvParameters )
{
    TickType_t xNextWakeTime;
    int this_task_index = mainPOLLING_SERVER_INDEX;
    TickType_t this_task_period = mainPOLLING_SERVER_PERIOD_MS;

    /* All tasks will be passed the periodic task list in their parameters */
    struct periodic_task *tasks = (struct periodic_task *) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    /* set the initial start time for this task
     * note that there is no need to call the EDF
     * scheduler because until the first task completes
     * the EDF and RMS schedules are identical */
    tasks[this_task_index].start_time = xNextWakeTime;
    int i;


    for( ;; )
    {
        /* Check the queue for an aperiodic task */
        struct aperiodic_task received_task;
        if ( xQueueReceive( aperiodic_task_queue, &received_task, 0 ) == pdPASS )
        {
            /* If an aperiodic task was received, run it */
            received_task.func();
        }
        //TODO: implement using the c_i value of the aperiodic task?


        if ( SCHED_POLICY == SCHED_POLICY_EDF )
        {
            /* update the initial start time for this task */
            tasks[this_task_index].start_time = xNextWakeTime + this_task_period;

            /* Call the EDF Scheduler to re-arrange priorities based on this new start time.
            * We must temporarily raise our priority to the highest priority so that we can
            * ensure that the scheduling is uninterrupted. */
                  taskENTER_CRITICAL();
                  prvEDFScheduler( tasks, this_task_index );
                  taskEXIT_CRITICAL();
        }

        /* Place this task in the blocked state until it is time to run again.
        The block time is specified in ticks, the constant used converts ticks
        to ms.  The task will not consume any CPU time while it is in the
        Blocked state. */
//        lcd_printf("%d", count);
        count++;
        char* test;
        vTaskList(test);
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
    int i, j;
    TickType_t test1;
    TickType_t test2;
    test1 = tasks[3].start_time;
    test2 = tasks[3].p_i;
    int test = calling_task_index;

    int tasks_assigned[ NUM_TASKS ] = { 0 }; // array to keep track of which tasks have been assigned a priority

    for ( i = 0; i < NUM_TASKS; i++)
    {
        /* find the task with the earliest deadline */
        TickType_t earliest_deadline;
        int earliest_deadline_index;

        /* first find the first task that is unassigned */
        for ( j = 0; j < NUM_TASKS; j++)
        {
            if (!tasks_assigned[j])
            {
                test1 = tasks[j].start_time;
                test2 = tasks[j].p_i;
                earliest_deadline = tasks[j].start_time + tasks[j].p_i;
                earliest_deadline_index = j;
                break;
            }
        }

        /* now check if there are any other unassigned tasks that have an earlier deadline */
        for ( j = earliest_deadline_index + 1; j < NUM_TASKS; j++)
        {
            if (tasks[j].start_time + tasks[j].p_i < earliest_deadline && !tasks_assigned[j])
            {
                test1 = tasks[j].start_time;
                test2 = tasks[j].p_i;
                earliest_deadline = test1 + test2;
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
    for ( i = 0; i < NUM_TASKS; i++)
    {
        if (i != calling_task_index)
        {
            test2 = tasks[i].priority;
            vTaskPrioritySet( tasks[i].handle, tasks[i].priority );
            test1 = tasks[i].priority;
        }

    }

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
    for(;;);

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


void sample_aperiodic_task_func( void )
{
    /* This function is called by the aperiodic server when it receives an
     * aperiodic task. */
    uart_sendStr_A("Aperiodic task... RUNNING BABY!\r\n");

    /* Send funny ascii art */
    uart_sendStr_A("  _   _      _ _         __        __         _     _ \r\n");
    uart_sendStr_A(" | | | | ___| | | ___    \\ \\      / /__  _ __| | __| |\r\n");
    uart_sendStr_A(" | |_| |/ _ \\ | |/ _ \\    \\ \\ /\\ / / _ \\| '__| |/ _` |\r\n");
    uart_sendStr_A(" |  _  |  __/ | | (_) |    \\ V  V / (_) | |  | | (_| |\r\n");
    uart_sendStr_A(" |_| |_|\\___|_|_|\\___/      \\_/\\_/ \\___/|_|  |_|\\__,_|\r\n");
    uart_sendStr_A("                                                      \r\n");
}