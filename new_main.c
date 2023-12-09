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
#define mainTASK1_PERIOD_MS                 pdMS_TO_TICKS( 200 )

/* The period of the second task, specified in milliseconds, and
converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainTASK2_PERIOD_MS                 pdMS_TO_TICKS( 1000 )

/* The period of the third task, specified in milliseconds, and
converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainTASK3_PERIOD_MS                 pdMS_TO_TICKS( 1000 )

/* The period of the deferrable server, specified in milliseconds, and
converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainDEFERRABLE_SERVER_PERIOD_MS     pdMS_TO_TICKS( 100 )

/* The period of the polling server, specified in milliseconds, and
converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainPOLLING_SERVER_PERIOD_MS     pdMS_TO_TICKS( 100 )
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

/*-----------------------------------------------------------*/

int main( void )
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
            /* leave one upper priority level unassigned for EDF, for RMS it goes unused */
            tasks.priority[shortest_period_index] = ( configMAX_PRIORITIES - 2 ) - tasks_assigned;

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
    for (int i = 0; i < NUM_TASKS; i++)
    {
        xTaskCreate( tasks.func[i], tasks.name[i], configMINIMAL_STACK_SIZE, NULL, tasks.priority[i], &tasks.handle[i] );
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
    int this_task_index = mainTASK1_INDEX;
    TickType_t this_task_period = mainTASK1_PERIOD_MS;

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
        // DO SOMETHING

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
        // DO SOMETHING

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
        // DO SOMETHING

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

static void prvSetupHardware( void )
{
    /* Ensure all priority bits are assigned as preemption priority bits
    if using a ARM Cortex-M microcontroller. */
    NVIC_SetPriorityGrouping( 0 );

    /* TODO: Setup the clocks, etc. here, if they were not configured before
    main() was called. */
}