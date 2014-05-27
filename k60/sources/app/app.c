/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                        Freescale Kinetis K60
*                                               on the
*
*                                        Freescale TWR-K60N512
*                                          Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <app_cfg.h>
#include  <includes.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
static  OS_TCB        App_TaskStartTCB;
static  OS_TCB        App_Task1_TCB;
static  OS_TCB        App_Task2_TCB;
static  OS_TCB        App_Task3_TCB;


static  CPU_STK_SIZE  App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE];
static  CPU_STK_SIZE  App_Task1Stk[APP_CFG_TASK_1_STK_SIZE];
static  CPU_STK_SIZE  App_Task2Stk[APP_CFG_TASK_2_STK_SIZE];
static  CPU_STK_SIZE  App_Task3Stk[APP_CFG_TASK_3_STK_SIZE];


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

//static  void  App_ObjCreate          (void);
//static  void  App_TaskCreate         (void);

static  void  App_TaskStart          (void       *p_arg);
static  void  App_Task1              (void       *p_arg);
static  void  App_Task2              (void       *p_arg);
static  void  App_Task3              (void       *p_arg);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This the main standard entry point.
*
* Note(s)     : none.
*********************************************************************************************************
*/

int  main (void)
{
	OS_ERR  err;

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
	CPU_ERR     cpu_err;
#endif
	
	CPU_Init();
	Mem_Init();                                                 /* Initialize the Memory Management Module              */
	Math_Init();                                                /* Initialize the Mathematical Module                   */
     
#if (CPU_CFG_NAME_EN == DEF_ENABLED)
	CPU_NameSet((CPU_CHAR *)"MK60N512ZVMD10",
			   (CPU_ERR  *)&cpu_err);
#endif
	
	BSP_IntDisAll();                                            /* Disable all interrupts until we are ready to accept them */

	OSInit(&err);                                               /* Initialize "uC/OS-III, The Real-Time Kernel"             */

	OSTaskCreate((OS_TCB     *)&App_TaskStartTCB,               /* Create the start task                                    */
                 (CPU_CHAR   *)"App Task Start",
                 (OS_TASK_PTR ) App_TaskStart,
                 (void       *) 0,
                 (OS_PRIO     ) APP_CFG_TASK_START_PRIO,
                 (CPU_STK    *)&App_TaskStartStk[0],
                 (CPU_STK     )(APP_CFG_TASK_START_STK_SIZE / 10u),
                 (CPU_STK_SIZE) APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  ) 0,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

      
#if (OS_TASK_NAME_EN > 0u)
	/*OSTaskNameSet(APP_CFG_TASK_START_PRIO, "Start", &err);*/
#endif

   	OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III).     */

	while(DEF_ON){												/* Should Never Get Here							        */
	};
}


/*********************************************************************************************************
*                                          App_TaskStart()
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Notes       : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  App_TaskStart (void *p_arg)
{
    OS_ERR  os_err;

    (void)p_arg;                                                /* See Note #1                                              */

    BSP_Init();                                  /* Start BSP and tick initialization                        */
    
	BSP_Tick_Init();                             /* Start Tick Initialization                                */
    
	OSTaskCreate((OS_TCB     *)&App_Task1_TCB,
				 (CPU_CHAR   *)"App Task1",
				 (OS_TASK_PTR ) App_Task1,
				 (void       *) 0,
				 (OS_PRIO     ) APP_CFG_TASK1_PRIO,
				 (CPU_STK    *)&App_Task1Stk[0],
				 (CPU_STK     )(APP_CFG_TASK_1_STK_SIZE / 10u),
				 (CPU_STK_SIZE) APP_CFG_TASK_1_STK_SIZE,
				 (OS_MSG_QTY  ) 0,
				 (OS_TICK     ) 0,
				 (void       *) 0,
				 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				 (OS_ERR     *)&os_err);
    
	OSTaskCreate((OS_TCB     *)&App_Task2_TCB,
				 (CPU_CHAR   *)"App Task2",
				 (OS_TASK_PTR ) App_Task2,
				 (void       *) 0,
				 (OS_PRIO     ) APP_CFG_TASK2_PRIO,
				 (CPU_STK    *)&App_Task2Stk[0],
				 (CPU_STK     )(APP_CFG_TASK_2_STK_SIZE / 10u),
				 (CPU_STK_SIZE) APP_CFG_TASK_2_STK_SIZE,
				 (OS_MSG_QTY  ) 0,
				 (OS_TICK     ) 0,
				 (void       *) 0,
				 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				 (OS_ERR     *)&os_err);

    OSTaskCreate((OS_TCB     *)&App_Task3_TCB,
				 (CPU_CHAR   *)"App Task3",
				 (OS_TASK_PTR ) App_Task3,
				 (void       *) 0,
				 (OS_PRIO     ) APP_CFG_TASK3_PRIO,
				 (CPU_STK    *)&App_Task3Stk[0],
				 (CPU_STK     )(APP_CFG_TASK_3_STK_SIZE / 10u),
				 (CPU_STK_SIZE) APP_CFG_TASK_3_STK_SIZE,
				 (OS_MSG_QTY  ) 0,
				 (OS_TICK     ) 0,
				 (void       *) 0,
				 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				 (OS_ERR     *)&os_err);
   
#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                               /* Determine CPU capacity                                   */
#endif
    

    APP_TRACE_INFO(("Creating Application Events...\n\r"));
    //App_ObjCreate();                                            /* Create Applicaiton kernel objects                        */

    APP_TRACE_INFO(("Creating Application Tasks...\n\r"));
    //App_TaskCreate();                                           /* Create Application tasks                                 */

    BSP_LED_Off(BSP_LED_ALL);

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.           */
        BSP_LED_Toggle(BSP_LED_BLUE);
        OSTimeDly((OS_TICK   )4000,
				  (OS_OPT    )OS_OPT_TIME_DLY,
				  (OS_ERR   *)&os_err);
    }
}

static void App_Task1 (void *p_arg)
{
	OS_ERR err;
    CPU_TS ts;

	(void)p_arg;
	p_arg = p_arg;
    BSP_LED_Off(BSP_LED_ALL);
	while(DEF_TRUE)
	{
        BSP_LED_Toggle(BSP_LED_GREEN);
		OSTimeDly((OS_TICK   )2000,
				  (OS_OPT    )OS_OPT_TIME_DLY,
				  (OS_ERR   *)&err);
	}
}

static void App_Task2 (void *p_arg)
{
	OS_ERR err;
    CPU_TS ts;

	(void)p_arg;
	p_arg = p_arg;
    BSP_LED_Off(BSP_LED_ALL);
	while(DEF_TRUE)
	{
        BSP_LED_Toggle(BSP_LED_YELLOW);
		OSTimeDly((OS_TICK   )1000,
				  (OS_OPT    )OS_OPT_TIME_DLY,
				  (OS_ERR   *)&err);
	}
}

static void App_Task3 (void *p_arg)
{
	OS_ERR err;
    CPU_TS ts;

	(void)p_arg;
	p_arg = p_arg;
    BSP_LED_Off(BSP_LED_ALL);
	while(DEF_TRUE)
	{
        BSP_LED_Toggle(BSP_LED_ORANGE);
		OSTimeDly((OS_TICK   )500,
				  (OS_OPT    )OS_OPT_TIME_DLY,
				  (OS_ERR   *)&err);
	}
}
