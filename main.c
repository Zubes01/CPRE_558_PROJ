/* Kernel includes. */
#include "FreeRTOS.h" /* Must come first. */
#include "task.h"     /* RTOS task related API prototypes. */
//#include "queue.h"    /* RTOS queue related API prototypes. */ //TODO: probably not required, but unsure so leaving it commented
//#include "timers.h"   /* Software timer related API prototypes. */ //TODO: probably not required, but unsure so leaving it commented
//#include "semphr.h"   /* Semaphore related API prototypes. */ //TODO: probably not required, but unsure so leaving it commented

/* TODO Add any manufacture supplied header files can be included
here. */
#include "hardware.h"

/* Scheduling policy 
 * 0: EDF         
 * 1: RMS           */
#define SCHED_POLICY_EDF 0
#define SCHED_POLICY_RMS 1
#define SCHED_POLICY SCHED_POLICY_RMS

/* The number of tasks */
#define NUM_TASKS 3
/* To add a new task, increment NUM_TASKS, add its period below,
 * add its function implementation later in this file, add its
 * function prototype with the others, and add it to the list
 * of tasks defined in main() */

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
typedef   int (*Function_Pointer)( void );

struct periodic_task 
{
    char *name;             // task name
    TickType_t  p_i;        // period of task in ticks
    UBaseType_t priority;   // priority of task (set by scheduling policy)
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

/*-----------------------------------------------------------*/

int main(void)
{
    /* Configure the system ready to run the demo.  The clock configuration
    can be done here if it was not done before main() was called. */
    prvSetupHardware();

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
        }
    };

    /* Set task priorities based on scheduling policy */
    if ( SCHED_POLICY == SCHED_POLICY_RMS )
    { 
        /* in RMS scheduling, the task with the shortest period always has the 
         * highest priority. This is static and never changes. */
        TickType_t tempTaskPeriods[NUM_TASKS]; // temporary array to store task periods for sorting
        for (int i = 0; i < NUM_TASKS; i++)
        {
            tempTaskPeriods[i] = tasks.p_i[i];
        }
        for (int tasks_assigned = 0; tasks_assigned < NUM_TASKS; tasks_assigned++)
        {
            /* find the task with the shortest period */
            TickType_t shortest_period = tempTaskPeriods[0];
            int shortest_period_index = 0;
            for (int i = 1; i < NUM_TASKS; i++)
            {
                if (tempTaskPeriods[i] < shortest_period && tempTaskPeriods[i] != -1)
                { /* If the task period is shorter and available (not already used) */
                    shortest_period = tempTaskPeriods[i];
                    shortest_period_index = i;
                }
            }

            /* assign the task with the shortest period the highest priority */
            tasks.priority[shortest_period_index] = ( configMAX_PRIORITIES - 1 ) - tasks_assigned;

            /* remove the task with the shortest period from the list */
            tempTaskPeriods[shortest_period_index] = -1;
        }
    }
    else if ( SCHED_POLICY == SCHED_POLICY_EDF )
    {
        /* in EDF scheduling, the task with the earliest deadline always has the 
         * highest priority. This is dynamic and changes as the tasks run. */
        //TODO: implement EDF scheduling
    }
    else
    {
        /* invalid scheduling policy */
        while(1);
    }

    /* Create the tasks */
    for (int i = 0; i < NUM_TASKS; i++)
    {
        xTaskCreate( tasks.func[i], tasks.name[i], configMINIMAL_STACK_SIZE, NULL, tasks.priority[i], NULL );
    }

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