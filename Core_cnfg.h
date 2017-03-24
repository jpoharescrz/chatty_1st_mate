/////////////////////////////////////////////////////////////////////////
////                          Core_cnfg.h                            ////
////         
//// This file is where users may define new tasks                                                        ////
/////////////////////////////////////////////////////////////////////////
// The following functions allow users to schedule task execution.
//
// void Put1MsSchedRtn(int8 TaskIndex, int8 period_cnt)		// schedule using 1 ms queue
// void Put10MsSchedRtn(int8 TaskIndex, int8 period_cnt)	// schedule using 10 ms queue
// void Put100MsSchedRtn(int8 TaskIndex, int8 period_cnt)	// schedule using 100 ms queue
// void Put1SecSchedRtn(int8 TaskIndex, int8 period_cnt)	// schedule using 1 sec queue
// void Put10SecSchedRtn(int8 TaskIndex, int8 period_cnt)	// schedule using 10 sec queue
// 
// The task index byte passed to these scheduling functions uses bit 7 (if set) to specify
// whether a task is to be re-scheduled indefinitely at the specified period. Hence, the
// lower 7 bits of the task index byte is used for the actual task index.
//
// A user may schedule his task to run periodically by setting 
// bit 7 in the TaskIndex or schedule to run one time in a
// specified period by clearing bit 7.
//
// Example:
// To schedule a given task with task index of Y to run every 12 ms would require;
// Put1MsSchedRtn( Y | 0x80, 12);
// To schedule a given task with task index of Z to run once in 820 ms would require;
// Put10MsSchedRtn( Z, 82);
//
// Kernel Services:
// The following is a list of services provided for the user:
//
// Prototype													Description
//
// void SignalTask(int8 task, int8 sigbitnum);					// Signals spec'd task with
//																// spec'd signal 
// void EnableTask( int8 task );								// Enables spec'd task
// void DisableTask( int8 task );								// Disables spec'd task
// void Put1MsSchedRtn(int8 TaskIndex, int8 period_cnt);		// Queues spec'd task to run in
//																// spec'd period * 1 ms
// void Put10MsSchedRtn(int8 TaskIndex, int8 period_cnt);		// Queues spec'd task to run in
//																// spec'd period * 10 ms
// void Put100MsSchedRtn(int8 TaskIndex, int8 period_cnt);		// Queues spec'd task to run in
//																// spec'd period * 100 ms
// void Put1SecSchedRtn(int8 TaskIndex, int8 period_cnt);		// Queues spec'd task to run in
//																// spec'd period * 1 sec
// void Put10SecSchedRtn(int8 TaskIndex, int8 period_cnt);		// Queues spec'd task to run in
//																// spec'd period * 10 sec
// int8 GetTaskSignal(int8 task, int8 sigbitnum);				// returns 0/1 if spec'd signal bit
//																// is clear/set
// void ClrTaskSignal(int8 task, int8 sigbitnum);				// Clears a specific signal bit
// void SetTaskSignalByte(int8 task, int8 signal);				// Sets all 8 signals to a value
// void SetTaskPeriod( int8 task, int8 TaskPeriod );			// Sets the task's period
// int8 IsTaskInSchedQue( int8 task );							// Returns True if task is queued
// void SetTaskError( int8 task );								// Sets the Error bit for a task
// void ClrTaskError( int8 task );								// Clears an Error bit for a task
// int8 IsTaskInError( int8 task );								// Returns an Error bit for a task
// void SetTaskTruncate( int8 task );							// Sets the task truncate which forces
//																// the task to run the next time it's
//																// timer queue task runs
// void ClrTaskTruncate( int8 task );							// Clears a task's truncate bit
// int8 IsTaskTruncated( int8 task );							// Returns the state of the truncate bit
// void SetTaskScratchBit( int8 task, int8 mask );				// Sets a bit(s) in the task's Scratch byte	
// void ClrTaskScratchBit( int8 task, int8 mask );				// Clears bit(s) in tha task's Scratch byte
//																// a set bit in mask clears the bit
// int8 IsTaskScratchBitSet( int8 task, int8 mask );			// Returns Scratch bit(s)
// void SetToneHi();											// Sets the Peizo Tone volume to Hi
// void SetToneWayHi();											// Sets the Peizo Tone volume to way hi
// void SetToneLo();											// Sets the Peizo Tone volume to lo
// void SetToneOff();											// Turns off the tone.
//////////////////////////////////////////////////////////////////
//
// *********************************************************************
// * User Task definitions
// * 
// * functions which use scheduled task services must be defined here
// *********************************************************************
// * 
// The first #define should be of USERTASKX where X is the next
// task number after the last valid USERTASKX above. The new
// USERTASKX should be set to the label of the new task. 
//
// The second define sets an index for the task.
// The second #define should be of PROC_XXXXX where XXXXX is an
// appropriate label for the new task. The new PROC_XXXXX should
// be set to USER_TASK0_IDX+X where X matches the X in the
// USERTASKX immediately before it.
//
// The third line creates a prototype for the user task.
// 
/////////////////////////////////////////////////////////////////////
#ifndef CORE_CNFG_H
#define CORE_CNFG_H

#define Version "2.01"

#undef ONBOARD_GPS_MUDULE

// defines the entries capacity for each queue
#define _1MSQUESIZE		10
#define _10MSQUESIZE	16
#define _100MSQUESIZE	10
#define _1SECQUESIZE	7
#define _10SECQUESIZE	3

enum core_task_indices {
    PROC_SIGQUE,
    PROC_1MSQUE,
    PROC_10MSQUE,
    PROC_100MSQUE,
    PROC_1SECQUE,
    PROC_10SECQUE,
    PROC_LEDPULSER,
    CORE_TASKCNT
};

#define USER_TASK0_IDX CORE_TASKCNT

//////////////////////////////////////////////////////////////////
// Add next task here
// 1) add the new task's enum below in user_task_indices. (PROC_NEW_TASK)
// 2) add the new task's prototype below (int8 NewTask( int8 ))
// 3) add the task name (NewTask) to the bottom of core_tasks[]]
// 4) add any task initialization prototypes and then add it's
//      invocation to Core_InitUser()
//////////////////////////////////////////////////////////////////
enum user_task_indices {
 PROC_TTS_TASK	=	USER_TASK0_IDX,
 PROC_BUTTONTASK,		
 PROC_DSPLYTASK,
 PROC_NMEATASK,
 PROC_ADSCTASK,
 PROC_TTS_WD_TASK,
 PROC_ISD_TASK,
 PROC_ISDRAC_TASK,
 PROC_I2CTASK,
 PROC_FMT_TASK,
#ifdef ONBOARD_GPS_MODULE
 PROC_GPS_TASK,
#endif
 PROC_RS232_TASK,
 PROC_CMPS03_TASK,
 PROC_USB_USART_TASK,
 // PROC_NEW_TASK,  ******************
 TASKCNT
};

#include "Core_data_defs.h"

#ifdef CORE

/* System task prototypes */
int8 ProcSigTaskQue(int8 signal);
int8 Proc1MsQue(int8 signal);
int8 Proc10MsQue(int8 signal);
int8 Proc100MsQue(int8 signal);
int8 Proc1SecQue(int8 signal);
int8 Proc10SecQue(int8 signal);
int8 ProcLedPulser(int8 signal);

/* User task prototypes */
int8 Tts_ProcTask(int8 signal);
int8 Btn_ProcTask(int8 signal);
int8 Disp_ProcTask(int8 signal);
int8 Nmea_ProcTask(int8 signal);
int8 Adsc_ProcTask(int8 signal);
int8 Tts_ProcWdTask(int8 signal);
int8 Isd_ProcTask(int8 signal);
int8 IsdRac_ProcTask(int8 signal);
int8 I2C_ProcTask(int8 signal);
int8 FMT_ProcTask(int8 signal);
#ifdef ONBOARD_GPS_MODULE
int8 GPS_ProcTask(int8 signal);
#endif
int8 RS232_ProcTask(int8 signal);
int8 CMPS03_ProcTask(int8 signal);
int8 USB_USART_ProcTask(int8 signal);
// int8 NewTask( int8 );

int8 (*core_tasks[])(int8) = {
    ProcSigTaskQue,
    Proc1MsQue,
    Proc10MsQue,
    Proc100MsQue,
    Proc1SecQue,
    Proc10SecQue,
    ProcLedPulser,
    /* User tasks beyond */
    Tts_ProcTask,
    Btn_ProcTask,
    Disp_ProcTask,
    Nmea_ProcTask,
    Adsc_ProcTask,
    Tts_ProcWdTask,
    Isd_ProcTask,
    IsdRac_ProcTask,
    I2C_ProcTask,
    FMT_ProcTask,
#ifdef ONBOARD_GPS_MODULE
    GPS_ProcTask,
#endif
    RS232_ProcTask,
    CMPS03_ProcTask,
    USB_USART_ProcTask
//  ,NewTask
};

/* User task init prototypes */
void RS232_Init();
void Pots_Init();
void Disp_InitTask();
void TTS_InitTask();
void Adsc_InitTask();
void Nmea_InitTask();
void Btn_InitTask();
void I2C_InitTask();
void Isd_Init();
void FMT_Init();
#ifdef ONBOARD_GPS_MODULE
void GPS_Init();
#endif
void CMPS03_Init();
void RS232_InitTask();
void USB_USART_init();

//void 	Btn_report_hw_cnfg_status();


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Add User task initialization routines below and it's 
// prototype above
// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

void Core_InitUser() {
 //   Pots_Init();
    Btn_InitTask(); // Load the settings first
    RS232_Init();
    I2C_InitTask();
    Isd_Init();
    TTS_InitTask();
    FMT_Init();
#ifdef ONBOARD_GPS_MODULE
    GPS_Init();
#endif
    CMPS03_Init();
    Disp_InitTask();
    Nmea_InitTask();
    Adsc_InitTask();
    RS232_InitTask();
    USB_USART_init();

    //Btn_report_hw_cnfg_status();
}

// Define any user ISR enables

void Core_EnableUserISRs() {

}
#endif /* CORE */

// Define the signals the user tasks use

#define SGNL_GENERIC_BIT	0
#define SGNL_TIMER_BIT 		1
#define SGNL_USARTA_BIT 	2
#define SGNL_USARTB_BIT		3
#define SGNL_USART_USB_BIT	4
#define SGNL_NMEA_WATCHDOG_BIT	5
#define SGNL_I2C_BIT		5
#define SGNL_TTS_BIT		6
#define SGNL_ISD_BIT		7
#define SGNL_LED_PULSE_INIT_BIT 7
#define SGNL_LED_PULSE_ACTIVE_BIT 6

#define SGNL_NO_REQUEUE			0
#define	SGNL_GENERIC_MASK		(1<<SGNL_GENERIC_BIT)
#define SGNL_TIMER_MASK			(1<<SGNL_TIMER_BIT)
#define SGNL_USARTA_MASK		(1<<SGNL_USARTA_BIT)
#define SGNL_USARTB_MASK		(1<<SGNL_USARTB_BIT)
#define SGNL_USART_USB_MASK		(1<<SGNL_USART_USB_BIT)
#define SGNL_NMEA_WATCHDOG_MASK		(1<<SGNL_NMEA_WATCHDOG_BIT)
#define SGNL_I2C_MASK			(1<<SGNL_I2C_BIT)
#define SGNL_TTS_MASK			(1<<SGNL_TTS_BIT)
#define SGNL_ISD_MASK			(1<<SGNL_ISD_BIT)
#define SGNL_LED_PULSE_INIT_MASK    (1<<SGNL_LED_PULSE_INIT_BIT)
#define SGNL_LED_PULSE_ACTIVE_MASK  (1<<SGNL_LED_PULSE_ACTIVE_BIT)

#define	_SGNL_GENERIC_MASK		((1<<SGNL_GENERIC_BIT)^0xFF)
#define _SGNL_TIMER_MASK		((1<<SGNL_TIMER_BIT)^0xFF)
#define _SGNL_USARTA_MASK		((1<<SGNL_USARTA_BIT)^0xFF)
#define _SGNL_USARTB_MASK		((1<<SGNL_USARTB_BIT)^0xFF)
#define _SGNL_I2C_MASK			((1<<SGNL_I2C_BIT)^0xFF)
#define _SGNL_NMEA_WATCHDOG_MASK		((1<<SGNL_NMEA_WATCHDOG_BIT)^0xFF)
#define _SGNL_TTS_MASK			((1<<SGNL_TTS_BIT)^0xFF)
#define _SGNL_ISD_MASK			((1<<SGNL_ISD_BIT)^0xFF)
#define _SGNL_LED_PULSE_INIT_MASK    ((1<<SGNL_LED_PULSE_INIT_BIT)^0xFF)
#define _SGNL_LED_PULSE_ACTIVE_MASK  ((1<<SGNL_LED_PULSE_ACTIVE_BIT)^0xFF)

#define CORE_FATAL_ERROR 1
#define BTN_FATAL_ERROR 2
#define NMEA_FATAL_ERROR 3
#define TTS_ADDR_INDEX_ERROR 4
#define RS232_FATAL_ERROR 5
#define ADSC_FATAL_ERROR 6
#define BAD_QUEUE_INDEX 7

//////////////////////////////////////////////////////////////////////
//Resource lock/unlock utilities
//
// Defines structures, functions, and resources for locking/unlocking
// The resources requiring locks are defined in core_reg.h
//////////////////////////////////////////////////////////////////////
// Right now we only lock the ISD
enum resource_lock_enums {
 RSRC_ISD,
 RSRC_USARTA_BUF0,
 RSRC_USARTA_BUF1,
 RSRC_USARTB_BUF0,
 RSRC_USARTB_BUF1,
 RSRC_USARTUSB_BUF0,
 RSRC_USARTUSB_BUF1,
 RSRC_I2C
};	

typedef enum resource_lock_enums rsrc_lock_enum;

#define NUM_RESOURCES 8

#endif  // CORE_CNFG_H
