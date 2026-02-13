/*
*********************************************************************************************************
*                                               uC/OS-II
*                                         The Real-Time Kernel
*
*                         (c) Copyright 1998-2003, Jean J. Labrosse, Weston, FL
*                                          All Rights Reserved
*
*                                          Sample code
*                                          MC9S12DP256B 
*                                       Wytec Dragon12 EVB
*
* File : app.c
* By   : Eric Shufro
*********************************************************************************************************
*/

#include    <includes.h>


/*
*********************************************************************************************************
*                                                DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                                CONSTANTS
*********************************************************************************************************
*/
   
   
/*
*********************************************************************************************************
*                                                VARIABLES
*********************************************************************************************************
*/

    OS_STK        AppStartTaskStk[APP_TASK_START_STK_SIZE];
    OS_STK        LCD_TestTaskStk[LCD_TASK_STK_SIZE];
    OS_STK        SevenSegTestTaskStk[SEVEN_SEG_TEST_TASK_STK_SIZE];
    OS_STK        KeypadRdTaskStk[KEYPAD_RD_TASK_STK_SIZE]; 
    OS_STK        MainTaskStk[MAIN_TASK_STK_SIZE];   
    			 
    OS_FLAG_GRP  *keypadEnFlagGrp;    


/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppStartTask(void *p_arg);
static  void  AppTaskCreate(void);
static  void  LCD_TestTask(void *p_arg);
static  void  SevenSegTestTask(void *p_arg);
static  void  KeypadRdTask(void *p_arg);
static  void  MainTask(void *p_arg);


#if (uC_PROBE_OS_PLUGIN > 0) || (uC_PROBE_COM_MODULE > 0)
extern  void  AppProbeInit(void);
#endif


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary 68HC12 and C initialization.
* Arguments   : none
*********************************************************************************************************
*/

void  main (void)
{
    INT8U   err;


    OSInit();                                                           /* Initialize "uC/OS-II, The Real-Time Kernel"              */

    OSTaskCreateExt(AppStartTask,
                    (void *)0,
                    (OS_STK *)&AppStartTaskStk[APP_TASK_START_STK_SIZE - 1],
                    APP_TASK_START_PRIO,
                    APP_TASK_START_PRIO,
                    (OS_STK *)&AppStartTaskStk[0],
                    APP_TASK_START_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskNameSet(APP_TASK_START_PRIO, "Start Task", &err);

    OSStart();                                                          /* Start multitasking (i.e. give control to uC/OS-II)       */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppStartTask()' by 'OSTaskCreate()'.
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*               2) Interrupts are enabled once the task start because the I-bit of the CCR register was
*                  set to 0 by 'OSTaskCreate()'.
*				3) After this created from main(), it runs and initializes additional application
*                  modules and tasks. Rather than deleting the task, it is simply suspended
*                  periodically. This tasks body could be used for additional work if desired.
*********************************************************************************************************
*/

static  void  AppStartTask (void *p_arg)
{		
   (void)p_arg;
   		  
    BSP_Init();                                                         /* Initialize the ticker, and other BSP related functions   */

#if OS_TASK_STAT_EN > 0
    OSStatInit();                                                       /* Start stats task                                         */
#endif

#if (uC_PROBE_OS_PLUGIN > 0) || (uC_PROBE_COM_MODULE > 0)
    AppProbeInit();                                                     /* Initialize uC/Probe modules                              */
#endif
    
    AppTaskCreate();                                                    /* Create additional tasks using this user defined function */
    
    while (TRUE) {                                                      /* Task body, always written as an infinite loop            */
        OSTimeDlyHMSM(0, 0, 5, 0);                                      /* Delay the task                                           */
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     CREATE APPLICATION TASKS
*
* Description : This function demonstrates how to create a new application task. 
* 
* Notes:        1) Each task should be a unique function prototypes as 
*                  static  void  mytaskname (void *p_arg). 
*               2) Additionally, each task should contain an infinite loop and call at least one
*                  OS resource on each pass of the loop. An OS resource may be a call to OSTimeDly(),
*                  OSTimeDlyHMSM(), or one of the message box, semaphore or other OS handled resource.
*               3) Each task must have its own stack. Be sure that the stack is declared large
*                  enough or the entire system may crash or experience erradic results if your stack
*                  grows and overwrites other variables in memory.
*               
* Arguments   : none
* Notes       : none
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
    INT8U  err;

    
    OSTaskCreateExt(LCD_TestTask,
                    (void *)0,
                    (OS_STK *)&LCD_TestTaskStk[LCD_TASK_STK_SIZE-1],
                    LCD_TEST_TASK_PRIO,
                    LCD_TEST_TASK_PRIO,
                    (OS_STK *)&LCD_TestTaskStk[0],
                    LCD_TASK_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    OSTaskNameSet(LCD_TEST_TASK_PRIO, "LCD Test Task", &err);    

    OSTaskCreateExt(SevenSegTestTask,
                    (void *)0,
                    (OS_STK *)&SevenSegTestTaskStk[SEVEN_SEG_TEST_TASK_STK_SIZE-1],
                    SEVEN_SEG_TEST_TASK_PRIO,
                    SEVEN_SEG_TEST_TASK_PRIO,
                    (OS_STK *)&SevenSegTestTaskStk[0],
                    SEVEN_SEG_TEST_TASK_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    OSTaskNameSet(SEVEN_SEG_TEST_TASK_PRIO, "SevenSegTest Task", &err);          

    OSTaskCreateExt(KeypadRdTask,
                    (void *)0,
                    (OS_STK *)&KeypadRdTaskStk[KEYPAD_RD_TASK_STK_SIZE-1],
                    KEYPAD_RD_TASK_PRIO,
                    KEYPAD_RD_TASK_PRIO,
                    (OS_STK *)&KeypadRdTaskStk[0],
                    KEYPAD_RD_TASK_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    OSTaskNameSet(KEYPAD_RD_TASK_PRIO, "KeypadRd Task", &err);
    
    OSTaskCreateExt(MainTask,
                    (void *)0,
                    (OS_STK *)&MainTaskStk[MAIN_TASK_STK_SIZE-1],
                    MAIN_TASK_PRIO,
                    MAIN_TASK_PRIO,
                    (OS_STK *)&MainTaskStk[0],
                    MAIN_TASK_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    OSTaskNameSet(MAIN_TASK_PRIO, "Main Task", &err);
                  
}

int DUTY;
int MIN_DUTY;
int MAX_DUTY;
int shift;
int sign;
int MIN_SHIFT;
int MAX_SHIFT;

int placeHolder = 0;

int BIT_MODE_16;
int changeDue;

static void MainTask(void) {
    
    //Initialize Ports for Debugging
    DDRB = 0xFF;
    DDRP = 0xFF;
    DDRJ = 0xFF;
    DDRH = 0x00;
    PTJ = 0x00;
    
    //Initialize PWM
    PWMCLK = 0x20;
    PWMPOL = 0x20;
    PWMCAE = 0x00;
    
    PWME = 0x30; 
    
    //Clockwise
    sign = 1;
    
    //Start with 16-bit mode
    BIT_MODE_16 = 1;
    changeDue = 1;
    
    while(1) {
        
        if(changeDue == 1) {
            
            if(BIT_MODE_16 == 1) {
                
                // Concat 4 & 5
                PWMCTL = 0x40;
                
                // 24MHz  / 2**1  = 12MHz
                // 12MHz  / 2*5   = 1.2MHz
                // 1.2MHz / 24000 = 50Hz
                PWMPRCLK = 0x01;
                PWMSCLA = 5;
                PWMPER45 = 24000;
                
                MIN_DUTY = 600;
                MAX_DUTY = 3000;
                
                DUTY = (MIN_DUTY + MAX_DUTY) / 2;
                
                shift = 100;
                MIN_SHIFT = 50;
                MAX_SHIFT = 500;
                
            } 
            else {
              
                // Disable concat
                PWMCTL = 0x00;
                
                // 24MHz / 2**3  = 3MHz
                // 3MHz  / 2*125 = 12KHz
                // 12KHz / 250   = 50Hz
                PWMPRCLK = 0x03;
                PWMSCLA = 125;
                PWMPER5 = 240;
                
                MIN_DUTY = 6;
                MAX_DUTY = 30;
                
                DUTY = (MIN_DUTY + MAX_DUTY) / 2;
                
                shift = 1;
                MIN_SHIFT = 1;
                MAX_SHIFT = 5;
              
            }
            changeDue = 0;
            
        }
        
        //Clamp PWM Duty Cycle
        if(DUTY > MAX_DUTY) {
            DUTY = MAX_DUTY;
        } 
        else if(DUTY < MIN_DUTY) {
            DUTY = MIN_DUTY;
        }
        
        //Clamp Shifts for PWM Duty
        if(shift > MAX_SHIFT) {
            shift = MAX_SHIFT;
        } 
        else if(shift < MIN_SHIFT) {
            shift = MIN_SHIFT;
        }
            
        if(BIT_MODE_16 == 1) {
            PWMDTY45 = DUTY;
        } 
        else {
            PWMDTY5 = DUTY;
        }
        
        
        OSTimeDlyHMSM(0,0,0,10);
    
    }
    
      
  
  
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                             SevenSegWriteTask
*
* Description: This task displays messages on the Dragon12 (16x2) LCD screen and is
*              responsible for initializing the LCD hardware. Care MUST be taken to
*              ensure that the LCD hardware is initialized before other tasks
*              attempt to write to it. If necessary the DispInit() function
*              may be called from the start task or bsp_init().
*********************************************************************************************************
*/

int showOnce = 1;

static  void  LCD_TestTask (void *p_arg)
{
       CPU_INT08S  i;
       CPU_INT08U  err;
                                                                        /* Keypad Enabled Message                                   */
const  CPU_INT08U  KeypadEnStr[18]    = {"Keypad Enabled"};                     

                                                                        /* Keypad Disabled Message                                  */                       
const  CPU_INT08U  KeypadDisStr[18]   = {"Keypad Disabled"};                    

                                                                        /* Power On Welcome Message. three seperate msgs / rows     */
const  CPU_INT08U  WelcomeStr[6][18]  = {"Project 4", "16-Bit mode  "}; 
    
                                                                        /* Define a message to scroll on the LCDs top line          */
const  CPU_INT08U  aboutStr[]         = {" "};
                                         
                                         
   CPU_INT08U  *aboutStrPtr;                                            /* Declare a pointer to aboutStr, used for scroll effect    */
    
                                   
                                  

   (void)p_arg;
                   
    DispInit(2, 16);                                                    /* Initialize uC/LCD for a 2 row by 16 column display       */

             
    while (DEF_TRUE) {                                                  /* All tasks bodies include an infinite loop                */           
        
        if(showOnce == 1) {
          
        DispClrScr();                                                   /* Start by clearing the screen                             */

        for (i = 0; i < 2; i+=2) {										/* With the Keypad task suspended, both LCD rows are avail. */
            DispStr(0, 0, WelcomeStr[i]);                               /* Display row 0 of Welcome Message i                       */
            DispStr(1, 0, WelcomeStr[i+1]);                             /* Display row 1 of Welcome Message i                       */        
            OSTimeDlyHMSM(0, 0, 1, 0);                                  /* Delay between updating the message                       */
        }        

        DispClrScr();
        
        showOnce = 0;
        }/* Clear LCD ROW 1 before unblocking the keypad task        */
                
        OSFlagPost(keypadEnFlagGrp, 0x01, OS_FLAG_SET, &err);           /* Set flag bit 0 of the keypad enable flag group           */
                                                                        /* This will unblock the keypad task which will use the     */
                                                                        /* bottom row of the LCD while not disabled                 */
                                                                            
        while (err != OS_NO_ERR) {                                      /* If a flag posting error occured,                         */
            OSTimeDlyHMSM(0, 0, 1, 0);                                  /* delay and try again until NO ERROR is returned           */
            OSFlagPost(keypadEnFlagGrp, 0x01, OS_FLAG_SET, &err);       /* Set flag bit 0 of the keypad enable flag group           */
        }


        

        aboutStrPtr = aboutStr;                                         /* Point to the start of the about message string           */

																		/* Start scrolling from right to left. This involves        */
        																/* starting from the last column and working toward the     */
                                                                        /* first column until the screen fills up. The remaining    */
                                                                        /* characters may start from position 0.                    */
        for (i = 15; i >= 0; i--) {										/* For the first 16 characters...                           */
            if (*aboutStrPtr != '\0') {						            /* If we have not reached the end of the string             */
                DispStr(0, i, aboutStrPtr);                             /* Display (col i - distance to end of lcd line) # of chars */
                OSTimeDlyHMSM(0, 0, 0, 100);                            /* Delay before updating the screen with the shifted msg    */             
            }
        }

        while ((aboutStr + sizeof(aboutStr) - aboutStrPtr) >  16) {     /* While there are greater than 16 chars left to display    */
            DispStr(0, 0, aboutStrPtr++);                               /* Display 16 chars and increment (shift) message by 1 char */
            OSTimeDlyHMSM(0, 0, 0, 100);                                /* Delay before displaying the shifted message              */
        }

        for (i = 15; i >= 0; i--) {										/* For the last 16 characters                               */
            if (*aboutStrPtr != '\0') {						            /* If we have not reached the end of the string             */
                DispStr(0, 0, aboutStrPtr++);                           /* Display (col i - distance to end of lcd line) # of chars */
                DispChar(0, i, ' ');                                    /* Leave spaces chars behind as the str scrolls off screen  */
                OSTimeDlyHMSM(0, 0, 0, 100);                            /* Delay before updating the screen with the shifted msg    */             
            }
        }
                
        DispClrLine(1);                                                 /* Clear LCD ROW 1 before blocking the keypad task          */
        
        //OSFlagPost(keypadEnFlagGrp, 0x01, OS_FLAG_CLR, &err);           /* Disable the keypad task by clearing its event flag 0     */

        while (err != OS_NO_ERR) {                                      /* If an error occured (perhaps the flag group is not       */
            OSTimeDlyHMSM(0, 0, 1, 0);                                  /* initialized yet, then delay and try again until no error */
            OSFlagPost(keypadEnFlagGrp, 0x01, OS_FLAG_CLR, &err);       /* Clear flag bit 0 of the keypad enable flag group         */
        }        
        
        //DispClrScr();                                                   /* Clear the screen                                         */
        //for (i = 0; i < 3; i++) {                                       /* Blink the Keypad Disabled String 3 times                 */
        //    DispStr(0, 0, KeypadDisStr);                                /* Display the Keypad Disabled message                      */     
        //    OSTimeDlyHMSM(0, 0, 0,500);                                 /* Show the message for a while                             */
        //    DispClrLine(0);                                             /* Next clear line 0 of the LCD                             */
        //    OSTimeDlyHMSM(0, 0, 0,500);                                 /* Lastly, wait a bit before repeating the message          */
        //}          
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                             SevenSegWriteTask
*
* Description: This task displays messages on the Dragon12 (16x2) LCD screen
*********************************************************************************************************
*/

static  void  SevenSegTestTask (void *p_arg)
{
    CPU_INT16U  num;


   (void)p_arg;
   
    SevenSegDisp_Init();	                                            /* Initialize the 7-Seg I/O and periodic refresh interrupt  */
    
    num = 0;
        
    while (DEF_TRUE) {                                                  /* All tasks bodies include an infinite loop                */   
        SevenSegWrite(placeHolder);                                             /* Output the value to the screen                           */        
                                              /* Increment the # being displayed, wrap after 9,999        */
        OSTimeDlyHMSM(0, 0, 0, 10);                                     /* Delay the task for 50ms and repeat the process           */
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                             Keypad Read Task
*
* Description: This task periodically reads the Wytec Dragon12 EVB keyapd and
*              displays the value on the bottom row of the LCD screen.
*********************************************************************************************************
*/

static  void  KeypadRdTask (void *p_arg)
{
    CPU_INT08U  key;
    CPU_INT08U  out_str[17];
    CPU_INT08U  key_map[] = {'1', '2', '3', 'A',
                             '4', '5', '6', 'B', 
                             '7', '8', '9', 'C',
                             '*', '0', '#', 'D'
                             };
    CPU_INT08U  err;
        

   (void)p_arg;
       
    KeypadInitPort();                                                   /* Initialize the keypad hardware                           */
    
    keypadEnFlagGrp = OSFlagCreate(0, &err);                            /* Create an event flag group. All flags initialized to 0   */
        
    while (err != OS_NO_ERR) {                                          /* If an error code was returned, loop until successful     */
        OSTimeDlyHMSM(0, 0, 1, 0);				                        /* Delay for 1 second, wait for resources to be freed       */
        keypadEnFlagGrp = OSFlagCreate(0, &err);                        /* Try to create the flag group again                       */
    }
    
    OSFlagPend(keypadEnFlagGrp, 0x01, OS_FLAG_WAIT_SET_ALL, 0, &err);   /* Wait until bit 1 of the flag group to become set         */
                                                                        /* The goal is to prevent this task from accessing the      */
                                                                        /* bottom row of the LCD until the LCD task has finished    */
                                                                        /* displaying its introduction message. This message will   */
                                                                        /* require both lines of the LCD, so we must wait to use it */
            
    DispClrLine(1);                                                     /* Clear the bottom line of the 2 line display              */
                
    while (DEF_TRUE) {                                                  /* All tasks bodies include an infinite loop                */   
        OSFlagPend(keypadEnFlagGrp, 0x01, OS_FLAG_WAIT_SET_ALL,0,&err); /* Suspend the task if flag 0 has been cleared              */
        key = KeypadReadPort();                                         /* Scan the keypad. Returns 0-15 or 0xFF if nothing pushed  */
        if (key == 0xFF) {            
            //err = sprintf(out_str, "Keypad is IDLE");
                 
        } else {
            
            DispClrLine(1);
            // Rotate Servo (Hold)
            if(key_map[key] == '1') {
                DUTY += sign * shift;
                err = sprintf(out_str, "Rotating...");
                placeHolder = DUTY;            
            }
            
            // Rotate to 0 degrees              
            else if(key_map[key] == '2') {
                DUTY = MIN_DUTY;
                err = sprintf(out_str, "Turning Left");
                placeHolder = DUTY;
            }
            
            // Rotate to 90 degrees
            else if(key_map[key] == '3') {
                DUTY = (MIN_DUTY + MAX_DUTY) / 2;
                err = sprintf(out_str, "Turning Neutral");
                placeHolder = DUTY;
            }
            
            // Rotate to 180 degrees
            else if(key_map[key] == 'A') {
                DUTY = MAX_DUTY;
                err = sprintf(out_str, "Turning Right");
                placeHolder = DUTY;
            }
            
            // Rotation speed slower
            else if(key_map[key] == '4') {
                if(BIT_MODE_16 == 1) {
                    shift -= 20;
                } else {
                    shift -= 1;
                }
                err = sprintf(out_str, "Rotation Slower");
                placeHolder = shift;
            }
            
            // Rotation speed faster
            else if(key_map[key] == '5') {
                if(BIT_MODE_16 == 1) {
                    shift += 20;
                } else {
                    shift += 1;
                }
                err = sprintf(out_str, "Rotation Faster");
                placeHolder = shift;
            }
            
            // 8-Bit mode 
            else if(key_map[key] == '6') {
                BIT_MODE_16 = 0;
                changeDue = 1;
                err = sprintf(out_str, "8-Bit mode");
            }
            
            // 16-Bit mode
            else if(key_map[key] == 'B') {
                BIT_MODE_16 = 1;
                changeDue = 1;
                err = sprintf(out_str, "16-Bit mode");
            }
            
            // 1) is now clockwise
            else if(key_map[key] == '8') {
                sign = 1;
                err = sprintf(out_str, "Clockwise");
                placeHolder = sign;
            }
            
            // 1) is now counter-clockwise
            else if(key_map[key] == '9') {
                sign = -1;
                err = sprintf(out_str, "Count Clockwise");
                placeHolder = sign;
            }
            
            //err = sprintf(out_str, "You Pressed: %c", key_map[key]);    /* Write "You Pressed" and the key number 'n' to a string   */      
        }
        
        DispStr(1, 0, out_str);
        //OSTimeDlyHMSM(0, 0, 0, 1);
    
    }
}



