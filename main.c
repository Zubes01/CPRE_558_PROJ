/* Kernel includes. */
#include "FreeRTOS.h" /* Must come first. */
#include "task.h"     /* RTOS task related API prototypes. */
#include "queue.h"    /* RTOS queue related API prototypes. */
//#include "timers.h"   /* Software timer related API prototypes. */ //TODO: probably not required
//#include "semphr.h"   /* Semaphore related API prototypes. */ //TODO: probably not required

/* TODO Add any manufacture supplied header files can be included
here. */
#include "hardware.h"

/* Priorities at which the tasks are created.  The priorities are given 
 * in order of highest to least highest priority, so that task 1 has 
 * the highest priority, task 2 has the second highest priority, etc. */
#define mainTASK1_TASK_PRIORITY     ( configMAX_PRIORITIES - 1 )
#define mainTASK2_TASK_PRIORITY     ( configMAX_PRIORITIES - 2 )
#define mainTASK3_TASK_PRIORITY     ( configMAX_PRIORITIES - 3 )

/* The period of the first task, specified in milliseconds, and
converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainTASK1_PERIOD_MS                 pdMS_TO_TICKS( 200 )

/* The period of the second task, specified in milliseconds, and
converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainTASK2_PERIOD_MS                 pdMS_TO_TICKS( 1000 )

/* The period of the third task, specified in milliseconds, and
converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainTASK3_PERIOD_MS                 pdMS_TO_TICKS( 1000 )

/*-----------------------------------------------------------*/

/*
 * TODO: Implement this function for any hardware specific clock configuration
 * that was not already performed before main() was called.
 */
static void prvSetupHardware( void );

/*
 * The three task functions.
 */
static void prvTask1( void *pvParameters );
static void prvTask2( void *pvParameters );
static void prvTask3( void *pvParameters );

/*-----------------------------------------------------------*/

int main(void)
{
    /* Configure the system ready to run the demo.  The clock configuration
    can be done here if it was not done before main() was called. */
    prvSetupHardware();

    /* Create the task 1 */
    xTaskCreate(     /* The function that implements the task. */
                    prvTask1,
                    /* Text name for the task, just to help debugging. */
                    "Task1",
                    /* The size (in words) of the stack that should be created
                    for the task. */
                    configMINIMAL_STACK_SIZE,
                    /* A parameter that can be passed into the task.  Not used
                    in this simple demo. */
                    NULL,
                    /* The priority to assign to the task.  tskIDLE_PRIORITY
                    (which is 0) is the lowest priority.  configMAX_PRIORITIES - 1
                    is the highest priority. */
                    mainTASK1_TASK_PRIORITY,
                    /* Used to obtain a handle to the created task.  Not used in
                    this simple demo, so set to NULL. */
                    NULL );


    /* Create the task 2 */
    xTaskCreate(    prvTask2,
                    "Task2",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    mainTASK2_TASK_PRIORITY,
                    NULL );


    /* Create the task 3 */
    xTaskCreate(    prvTask3,
                    "Task3",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    mainTASK3_TASK_PRIORITY,
                    NULL );

    /* Start the tasks and timer running. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details.  */
    for( ;; );
}
/*-----------------------------------------------------------*/

static void prvTask1( void *pvParameters )
{
    TickType_t xNextWakeTime;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    for( ;; )
    {
        /* Place this task in the blocked state until it is time to run again.
        The block time is specified in ticks, the constant used converts ticks
        to ms.  The task will not consume any CPU time while it is in the
        Blocked state. */
        vTaskDelayUntil( &xNextWakeTime, mainTASK1_PERIOD_MS );
    }
}
/*-----------------------------------------------------------*/

static void prvTask2( void *pvParameters )
{
    TickType_t xNextWakeTime;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    for( ;; )
    {
        /* Place this task in the blocked state until it is time to run again.
        The block time is specified in ticks, the constant used converts ticks
        to ms.  The task will not consume any CPU time while it is in the
        Blocked state. */
        vTaskDelayUntil( &xNextWakeTime, mainTASK2_PERIOD_MS );
    }
}
/*-----------------------------------------------------------*/

static void prvTask3( void *pvParameters )
{
    TickType_t xNextWakeTime;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    for( ;; )
    {
        /* Place this task in the blocked state until it is time to run again.
        The block time is specified in ticks, the constant used converts ticks
        to ms.  The task will not consume any CPU time while it is in the
        Blocked state. */
        vTaskDelayUntil( &xNextWakeTime, mainTASK3_PERIOD_MS );
    }
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
    /* Ensure all priority bits are assigned as preemption priority bits
    if using a ARM Cortex-M microcontroller. */
    NVIC_SetPriorityGrouping( 0 );

    /* TODO: Setup the clocks, etc. here, if they were not configured before
    main() was called. */
}