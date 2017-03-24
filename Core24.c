/////////////////////////////////////////////////////////////////////////
//*Filename Core24.C                              ////
//
//*Author Paul O'Hare
//
//**Description: Main C file for the Adapted Steering Compass (ADSC)
//   This file along with Core24_hw_defs.h, Core_user_defs.h, Core_utils.h,
//   Core.h, Core_user_tasks.h and Core_data.h provide task processing,
//   timing services, task signalling functions for user specified tasks.
//   This is a non-preemptive kernal with a single run queue with no priorities.
//   Tasks must voluntarily give up the processor and shouldn't run more than 1 ms
//   at a stretch. Tasks needing more processing time may do so using the method
//   described below in The Process Signalling Task section. */


#include <xc.h>

#include <p24FJ128GB204.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CORE 1
#include "Core24_hw_defs.h"
#include "Core_api.h"
#include "rs232_api.h"
#include "button_data_defs.h"
#include "button_api.h"

//#list

/////////////////////////////////////////////////////////////////////////
// Core24.c
//
// This file along with Core_hw_defs.h, Core_user_defs.h, Core_utils.h,
// Core.h, Core_user_tasks.h and Core_data.h provide task processing,
// timing services, task signalling functions for user specified tasks.
// This is a non-preemptive kernal with a single run queue with no priorities.
// Tasks must voluntarily give up the processor and shouldn't run more than 1 ms
// at a stretch. Tasks needing more processing time may do so using the method
// described below in The Process Signalling Task section.
// 
// *************************************************
// Main Loop:
// The program has a main loop which runs after initialization and pulls tasks
// from the run queue and runs them. There is always a task in the run queue (ProcSigTaskQue).
// The main loop also allows a timer interrupt to occur which happens every
// 1 ms.
// *************************************************
// 1 Ms Interrupt:
// The 1 ms interrupt's only function is to signal the 1 ms schedule task (described
// below)
// *************************************************
// Task Control Block Array (TCB):
// Each task has an entry in the TCB array. It is structured as follows:
// 
// typedef struct T_Ctl_blk {
// int8 Signal;            // holds 8 possible signals for a task 
// int8 Period;            // holds the period used to reschedule a task in a given time queue
// int8 Cnfg;            // holds various configuration bits
// int8 Scratch;         // Scratch
// }TskCtlBlk;
//
// The TCB array is defined as follows in Core_reg.h
// 
// TskCtlBlk TaskCtlBlk[ TASKCNT ];
// **************************************************
// The Process Signalling Task (ProcSigTaskQue):
//
// int8 ProcSigTaskQue(int8 signal);   //
// 
// As mentioned above ProcSigTaskQue task is always in the run queue.
// It's job is to scan the TCB array and queue on the run queue which
// has a non-zero Signal in it's TCB and has it's enabled bit set in the Cnfg byte
// in it's TCB. After that, it re-queues itself into the run queue after all the 
// tasks it potentially just queued. 
//
// Incidentally, all tasks return an 8 bit value which is written to the Signal byte
// in the TCB. In this way, tasks may return non-zero values so that it will be queued to
// run the next time ProcSigTaskQue() runs. Also, all tasks are passed a copy of TCB.Signal.
//
// Tasks should not hold the processor for more than 1 ms. if more processing time is
// needed, the task may divide itself into states. The task would then give up the processor
// but stay in the run queue by returning a nonzero value which would be written to TCB.Signal.
// 
// ****************************************************
// The Scheduled Tasks:
// These tasks process lists of tasks (8 bit index) with associated counters (8 bit). 
// The tasks run a fixed intervals of 1 ms, 10 ms, 100 ms, 1 sec, and 10 sec. 
// There is a distinct list and task for each of these above mentioned time intervals.
//
// int8 Proc1MsQue(int8 signal);   // processes the 1 ms queue
// int8 Proc10MsQue(int8 signal);    // processes the 10 ms queue
// int8 Proc100MsQue(int8 signal);    // processes the 100 ms queue
// int8 Proc1SecQue(int8 signal);   // processes the 1 sec queue
// int8 Proc10SecQue(int8 signal);   // processes the 10 sec queue
//
// The following functions allow users to schedule task execution.
//
// void Put1MsSchedRtn(int8 TaskIndex, int8 period_cnt)      // schedule using 1 ms queue
// void Put10MsSchedRtn(int8 TaskIndex, int8 period_cnt)   // schedule using 10 ms queue
// void Put100MsSchedRtn(int8 TaskIndex, int8 period_cnt)   // schedule using 100 ms queue
// void Put1SecSchedRtn(int8 TaskIndex, int8 period_cnt)   // schedule using 1 sec queue
// void Put10SecSchedRtn(int8 TaskIndex, int8 period_cnt)   // schedule using 10 sec queue
// 
// The 1 ms task is signalled by the 1 ms interrupt. 
// The 10 ms task is initialized to run on the 1 ms queue every 10 ms.
// The 100 ms task is initialized to run on the 10 ms queue every 100 ms.
// The 1 sec task is initialized to run on the 100 ms queue every 1 sec.
// The 10 sec task is initialized to run on the 1 sec queue every 10 sec.
//
// Example:
// To schedule a given task with task index of Y to run in 12 ms would require;
// Put1MsSchedRtn( Y, 12);
// To schedule a given task with task index of Z to run in 820 ms would require;
// Put10MsSchedRtn( Z, 82);
//
// The task index byte passed to these scheduling functions uses bit 7 (if set) to specify
// whether a task is to be re-scheduled indefinitely at the specified period. Hence, the
// lower 7 bits of the task index byte is used for the actual task index.
//
// Kernel Services:
// The following is a list of services provided for the user:
//
// Prototype                                       Description
//
// void SignalTask(int8 task, int8 sigbitnum);               // Signals spec'd task with
//                                                // spec'd signal 
// void EnableTask( int8 task );                        // Enables spec'd task
// void DisableTask( int8 task );                        // Disables spec'd task
// void Put1MsSchedRtn(int8 TaskIndex, int8 period_cnt);      // Queues spec'd task to run in
//                                                // spec'd period * 1 ms
// void Put10MsSchedRtn(int8 TaskIndex, int8 period_cnt);      // Queues spec'd task to run in
//                                                // spec'd period * 10 ms
// void Put100MsSchedRtn(int8 TaskIndex, int8 period_cnt);      // Queues spec'd task to run in
//                                                // spec'd period * 100 ms
// void Put1SecSchedRtn(int8 TaskIndex, int8 period_cnt);      // Queues spec'd task to run in
//                                                // spec'd period * 1 sec
// void Put10SecSchedRtn(int8 TaskIndex, int8 period_cnt);      // Queues spec'd task to run in
//                                                // spec'd period * 10 sec
// int8 GetTaskSignal(int8 task, int8 sigbitnum);            // returns 0/1 if spec'd signal bit
//                                                // is clear/set
// void ClrTaskSignal(int8 task, int8 sigbitnum);            // Clears a specific signal bit
// void SetTaskSignalByte(int8 task, int8 signal);            // Sets all 8 signals to a value
// void SetTaskPeriod( int8 task, int8 TaskPeriod );         // Sets the task's period
// int8 IsTaskInSchedQue( int8 task );                     // Returns True if task is queued
// void SetTaskError( int8 task );                        // Sets the Error bit for a task
// void ClrTaskError( int8 task );                        // Clears an Error bit for a task
// int8 IsTaskInError( int8 task );                        // Returns an Error bit for a task
// void SetTaskTruncate( int8 task );                     // Sets the task truncate which forces
//                                                // the task to run the next time it's
//                                                // timer queue task runs
// void ClrTaskTruncate( int8 task );                     // Clears a task's truncate bit
// int8 IsTaskTruncated( int8 task );                     // Returns the state of the truncate bit
// void SetTaskScratchBit( int8 task, int8 mask );            // Sets a bit(s) in the task's Scratch byte   
// void ClrTaskScratchBit( int8 task, int8 mask );            // Clears bit(s) in tha task's Scratch byte
//                                                // a set bit in mask clears the bit
// int8 IsTaskScratchBitSet( int8 task, int8 mask );         // Returns Scratch bit(s)
// void SetToneHi();                                 // Sets the Peizo Tone volume to Hi
// void SetToneWayHi();                                 // Sets the Peizo Tone volume to way hi
// void SetToneLo();                                 // Sets the Peizo Tone volume to lo
// void SetToneOff();                                 // Turns off the tone.
//
// 
/////////////////////////////////////////////////////////////////////////
// InitQue
/////////////////////////////////////////////////////////////////////////
void InitQue(int8 *Qptr, int8 MaxOffset)
{
 
 *Qptr++ = MaxOffset;   // Init max offset
 *Qptr++ = MaxOffset;   // Init wt offset
 *Qptr++ = MaxOffset;   // Init rd offset
 *Qptr = 0;            // Clear the entry count
}

TskCtlBlk *_core_get_task_ctl_blk_ptr( task )
{
	if (task < TASKCNT)
		return( &TaskCtlBlk[task] );
	else
		Core_CodeError(CORE_FATAL_ERROR);
	return( &TaskCtlBlk[PROC_SIGQUE] );
}

void SignalTask(int8 _task, int8 sigbitnum)
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
 	tp->Signal |= (1 << sigbitnum); 
}

void ClrTaskSignal(int8 _task, int8 sigbitnum)
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
    tp->Signal &= 0xFF ^ (1 << sigbitnum); 
}

void SetTaskSignalByte(int8 _task, int8 _signal)
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
 	tp->Signal = _signal; 
}

void OrTaskSignalByte(int8 _task, int8 _signal)
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
 	tp->Signal |= _signal; 
}

int8 GetTaskSignalByte(int8 _task)
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
 	return( tp->Signal ); 
}

int8 GetTaskSignal(int8 _task, int8 sigbitnum)
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
 	return( tp->Signal & (1 << sigbitnum) ); 
}
 
void SetTaskPeriod( int8 _task, int8 _TaskPeriod )
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
    tp->Period = _TaskPeriod;
}

int8 *GetTaskSignalByteAddr(int8 _task)
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
 	return( &(tp->Signal) ); 
}

void EnableTask( int8 _task )
{
 TaskCtlBlk[ _task & 0x7F ].Cnfg |= TBLK_CNFG_ENABLE_MASK;
}

int8 IsTaskEnabled( int8 _task )
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
 	return( tp->Cnfg & TBLK_CNFG_ENABLE_MASK );
}

void DisableTask( int8 _task )
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
 	tp->Cnfg &= 0xFF ^ (TBLK_CNFG_ENABLE_MASK | TBLK_TASK_ERROR_MASK);
}

void SetTaskError( int8 _task )
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
 	tp->Cnfg |= TBLK_TASK_ERROR_MASK;
}

int8 IsTaskInError( int8 _task )
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
 	return( tp->Cnfg & TBLK_TASK_ERROR_MASK );
}

void ClrTaskError( int8 _task )
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
 	tp->Cnfg &= 0xFF ^ TBLK_TASK_ERROR_MASK;
}

void SetTaskTruncate( int8 _task )
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
 	tp->Cnfg |= TBLK_TASK_TRUNCATE_MASK;
}

int8 IsTaskTruncated( int8 _task )
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
 	return(tp->Cnfg & TBLK_TASK_TRUNCATE_MASK );
}

void ClrTaskTruncate( int8 _task )
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
 	tp->Cnfg &= 0xFF ^ TBLK_TASK_TRUNCATE_MASK;
}

void SetTaskScratchBit( int8 _task, int8 mask )
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
 	tp->scratch |= mask;
}

int8 IsTaskScratchBitSet( int8 _task, int8 mask )
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
 	return( tp->scratch & mask );
}

void ClrTaskScratchBit( int8 _task, int8 mask )
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
 	tp->scratch &= 0xFF ^ mask;
}

int8 IsTaskInSchedQue( int8 _task )
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
 	return ( tp->Cnfg & TBLK_IN_SCHED_QUE_MASK );
}

int8 IsTaskInRunQue( int8 _task )
{
	return( GetTaskSignalByte( _task ) );
}

void ClrTaskTag( int8 _task )
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
 	tp->tag = 0;
}

void SetTaskTag( int8 _task, int8 tag )
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
 	tp->tag = tag;
}

int8 GetTaskTag( int8 _task, int8 tag )
{
 TskCtlBlk *tp;
 
 	tp = _core_get_task_ctl_blk_ptr( _task & 0x7F );
 	return( tp->tag );
}

/////////////////////////////////////////////////////////////////////////
// PutRunQueTask
//
// Puts a task in the run queue
/////////////////////////////////////////////////////////////////////////
void PutRunQueTask( int8 TaskIndex )
{
  	RunQue.CNT++;
 	RunQue.WT_OFF -= 1;
 	if ((signed)RunQue.WT_OFF < 0) 
 	{
  		RunQue.WT_OFF = RUNQUESIZE-1;
  	}	
 
 	if (RunQue.WT_OFF == RunQue.RD_OFF)
 	{
   		FatalError( RUNQUE_OVERFLOW );
  	} 	

 	RunqBuf[(short)RunQue.WT_OFF] = TaskIndex;
}

/////////////////////////////////////////////////////////////////////////
// GetRunQueTask
//
// Gets the next task out of the run queue
/////////////////////////////////////////////////////////////////////////
int8 GetRunQueTask()
{
 	RunQue.CNT--;
 	RunQue.RD_OFF -= 1;
 	if ((signed)RunQue.RD_OFF < 0) 
 	{
   		RunQue.RD_OFF = RUNQUESIZE-1;
  	} 	
 	return( RunqBuf[(short)RunQue.RD_OFF] );
}


/////////////////////////////////////////////////////////////////////////
// PutSchedTask
//
// In the interest of speed, we declare the scheduled queues as a 
// monolithic byte buffer with an extra 4 bytes at the top for
// the header stuff (count, wr offset, rd offset, max offset)
//
// Scheduling queue header
//typedef struct QHeader {
// int8 MaxOffset;
// int8 WT_OFF;
// int8 RD_OFF;
// int8 CNT;
//}QueHdr;
//
// Scheduling queue record
//typedef struct Qrec {
//int8 count;
//int8 taskId;
//int8 signal;
//int8 period;
//}QueRecord;
//
//#define QRECSIZE sizeof(QueRecord)
/////////////////////////////////////////////////////////////////////////
int8 PutSchedTask(int8 *Qptr, int8 TaskIndex, int8 period_cnt, int8 Signal, int8 period)
{
 int8 temp;
 TskCtlBlk *tp;

 	temp = *Qptr++;     // get the max offset. Then point at the write offset

 	if ((*Qptr) == 0)      // if the write offset is zero
 	{
  		*Qptr = temp;         // set the write offset to the max offset
 	} 
 	else
 	{
  		*Qptr -= QRECSIZE;    // subtract QRECSIZE from the write offset
 	}
 	temp = *Qptr++;      // get the write offset, point QuePtr at read offset

 	if (temp == *Qptr++)   // check for overflow
 	{
        return(FALSE);
   		//FatalError( FE_10MSQUE_OVERFLOW );
 	}
 	(*Qptr)++;            // bump the entry count

 	Qptr += temp + 1;      // point at the record, we add 1 to the write offset
                  // to step over the counter
 
 	*Qptr++ = period_cnt;  
 	*Qptr++ = TaskIndex;
 	*Qptr++ = Signal;
 	*Qptr = period;	// non-zero if timer event

  	// Set queued flag
  	tp = _core_get_task_ctl_blk_ptr( TaskIndex & 0x7F );
  	tp->Cnfg |= TBLK_IN_SCHED_QUE_MASK;
    return(TRUE);
}

int8 Core_SchedTask(enum SchedQue Que, int8 TaskIndex, int8 period_cnt, int8 Signal, int8 period)
{
 int8 *Qptr, result;
 
#ifdef CORE_SCHEDTASK_FULL
 TskCtlBlk *tp;
 int8 temp;
#endif

    if ((int)Que > MAX_SCHED_QUE)
    {
        Core_CodeError( BAD_QUEUE_INDEX );
    }    
    Qptr = SchedQueLookup[ (int)Que ];
    
#ifndef CORE_SCHEDTASK_FULL
    result = PutSchedTask( Qptr, TaskIndex, period_cnt, Signal, period);
    return( result );
    
#else
 	temp = *Qptr++;     // get the max offset. Then point at the write offset

 	if ((*Qptr) == 0)      // if the write offset is zero
 	{
  		*Qptr = temp;         // set the write offset to the max offset
 	} 
 	else
 	{
  		*Qptr -= QRECSIZE;    // subtract QRECSIZE from the write offset
 	}
 	temp = *Qptr++;      // get the write offset, point QuePtr at read offset

 	if (temp == *Qptr++)   // check for overflow
 	{
   		//FatalError( FE_10MSQUE_OVERFLOW );
        return( FALSE );
 	}
 	(*Qptr)++;            // bump the entry count

 	Qptr += temp + 1;      // point at the record, we add 1 to the write offset
                  // to step over the counter
 
 	*Qptr++ = period_cnt;  
 	*Qptr++ = TaskIndex;
 	*Qptr++ = Signal;
 	*Qptr = period;	// non-zero if timer event

  	// Set queued flag
  	tp = _core_get_task_ctl_blk_ptr( TaskIndex & 0x7F );
  	tp->Cnfg |= TBLK_IN_SCHED_QUE_MASK;

    return( TRUE );
#endif
}
#ifdef USE_OLD_SCHEDULERS
void Put1MsSchedRtn(int8 TaskIndex, int8 period_cnt)
{
 	if (TaskIndex & 0x80)    // check for automatic reschedule
 	{
  		// Set the reschedule period count
  		PutSchedTask( &_1msQ_u._1msqBuf[0], TaskIndex, period_cnt, SGNL_TIMER_MASK, period_cnt);
 	}
 	else // single event in the future
 	{
  		PutSchedTask( &_1msQ_u._1msqBuf[0], TaskIndex, period_cnt, SGNL_TIMER_MASK, 0);
 	}
}

void Put10MsSchedRtn(int8 TaskIndex, int8 period_cnt)
{
 	if ( TaskIndex & 0x80 )    // check for automatic reschedule
 	{
  		// Set the reschedule period count
  		PutSchedTask( &_10msQ_u._10msqBuf[0], TaskIndex, period_cnt, SGNL_TIMER_MASK, period_cnt);
 	}
 	else // single event in the future
 	{
  		PutSchedTask( &_10msQ_u._10msqBuf[0], TaskIndex, period_cnt, SGNL_TIMER_MASK, 0);
 	}
}

void Put10MsSchedRtn_wSignal(int8 TaskIndex, int8 period_cnt, int8 signal)
{
 	if ( TaskIndex & 0x80 )    // check for automatic reschedule
 	{
  		// Set the reschedule period count
  		PutSchedTask( &_10msQ_u._10msqBuf[0], TaskIndex, period_cnt, signal, period_cnt);
 	}
 	else // single event in the future
 	{
  		PutSchedTask( &_10msQ_u._10msqBuf[0], TaskIndex, period_cnt, signal, 0);
 	}
}

void Put100MsSchedRtn(int8 TaskIndex, int8 period_cnt)
{
 	if ( TaskIndex & 0x80 )    // check for automatic reschedule
 	{
  		// Set the reschedule period count
  		PutSchedTask( &_100msQ_u._100msqBuf[0], TaskIndex, period_cnt, SGNL_TIMER_MASK, period_cnt);
 	}
 	else // single event in the future
 	{
  		PutSchedTask( &_100msQ_u._100msqBuf[0], TaskIndex, period_cnt, SGNL_TIMER_MASK, 0);
 	}
}

void Put100MsSchedRtn_wSignal(int8 TaskIndex, int8 period_cnt, int8 Signal)
{
 	if ( TaskIndex & 0x80 )    // check for automatic reschedule
 	{
  		// Set the reschedule period count
  		PutSchedTask( &_100msQ_u._100msqBuf[0], TaskIndex, period_cnt, Signal, period_cnt);
 	}
 	else // single event in the future
 	{
  		PutSchedTask( &_100msQ_u._100msqBuf[0], TaskIndex, period_cnt, Signal, 0);
 	}
}

void Put1SecSchedRtn_wSignal(int8 TaskIndex, int8 period_cnt, int8 Signal)
{
 	if ( TaskIndex & 0x80 )    // check for automatic reschedule
 	{
  		// Set the reschedule period count
  		PutSchedTask( &_1SecQ_u._1secqBuf[0], TaskIndex, period_cnt, Signal, period_cnt);
 	}
 	else // single event in the future
 	{
  		PutSchedTask( &_1SecQ_u._1secqBuf[0], TaskIndex, period_cnt, Signal, 0);
 	}
}

void Put1SecSchedRtn(int8 TaskIndex, int8 period_cnt)
{
 	if ( TaskIndex & 0x80 )    // check for automatic reschedule
 	{
  		// Set the reschedule period count
  		PutSchedTask( &_1SecQ_u._1secqBuf[0], TaskIndex, period_cnt, SGNL_TIMER_MASK, period_cnt);
 	}
 	else // single event in the future
 	{
  		PutSchedTask( &_1SecQ_u._1secqBuf[0], TaskIndex, period_cnt, SGNL_TIMER_MASK, 0);
 	}
}

void Put10SecSchedRtn(int8 TaskIndex, int8 period_cnt)
{
 	if ( TaskIndex & 0x80 )    // check for automatic reschedule
 	{
  		// Set the reschedule period count
  		PutSchedTask( &_10SecQ_u._10secqBuf[0], TaskIndex, period_cnt, SGNL_TIMER_MASK, period_cnt);
 	}
 	else // single event in the future
 	{
  		PutSchedTask( &_10SecQ_u._10secqBuf[0], TaskIndex, period_cnt, SGNL_TIMER_MASK, 0);
 	}
}
#endif
/////////////////////////////////////////////////////////////////////////
// GetSchedTask
//
// Returns the task index of the next scheduled task in the specified
// queue.
//
// In the interest of speed, we declare the scheduled queues as a 
// monolithic byte buffer with an extra 4 bytes at the top for
// the header stuff (count, wr offset, rd offset, max offset).
// Also, don't call this function without checking the task count of the
// queue.
//
// Scheduling queue header
//typedef struct QHeader {
// int8 MaxOffset;
// int8 WT_OFF;
// int8 RD_OFF;
// int8 CNT;
//}QueHdr;
//
// Scheduling queue record
//typedef struct Qrec {
//int8 count;
//int8 taskId;
//int8 signal;
//int8 period;
//}QueRecord;
//
//#define QRECSIZE sizeof(QueRecord)
/////////////////////////////////////////////////////////////////////////
int8 *GetSchedTask(int8 *Qptr)
{
 int8 temp;

 	temp = *Qptr;           // get the max offset
 	Qptr += 2;              // point at the read offset

 	if ((*Qptr) == 0)      // if the read offset is zero
 	{
  		*Qptr = temp;         // set the read offset to the max offset
 	} 
 	else
 	{
 		*Qptr -= QRECSIZE;             // subtract QRECSIZE from the read offset
 	}
 	temp = *Qptr++;         // get the read offset
 	(*Qptr)--;            // decrement the counter

 	Qptr += temp + 1;       // point at the record, we add 1 to the write offset
                  // to step over the counter

 	return( Qptr );          // return the pointer to the record
}

/////////////////////////////////////////////////////////////////////////
// ProcSchedQue
// 
// Processes the spec'd schedule queue
/////////////////////////////////////////////////////////////////////////
void ProcSchedQue(QueHdr *QuePtr )
{
 int8 count;
 int8 task_cnt;
 int8 task;
 int8 task_signal;
 int8 task_period;
 int8 *Qbuf_ptr;

 	// get a copy of the count 
 	count = QuePtr->CNT;

 	// while something in the queue
 	while(count--)
 	{
   		Qbuf_ptr = GetSchedTask( (int8 *) QuePtr );

  		// Get the period count
  		task_cnt = *Qbuf_ptr++;

  		// Get the task index            
  		task = *Qbuf_ptr++;

  		// Get the task signal
  		task_signal = *Qbuf_ptr++;

  		// Get the task period
  		task_period = *Qbuf_ptr++;

  		// if the period timer has expired or has been truncated for some reason, 
  		// signal the task to run
  		if ((task_cnt <= 1) || IsTaskTruncated( task ))
  		{
    		// signal the task
    		OrTaskSignalByte( task, task_signal );
    		// clear the truncate bit
    		ClrTaskTruncate( task );
 
    		if ( task & 0x80 )    // check for automatic reschedule
    		{
     			PutSchedTask( (int8 *)QuePtr, task, task_period, task_signal, task_period);  // reschedule the task
    		}
    		else
    		{
     			// Clear queued flag
    			TaskCtlBlk[ task & 0x7F ].Cnfg &= 0xff ^ TBLK_IN_SCHED_QUE_MASK;
    		}
   		}
   		else // It not time to signal the task to run
   		{
    		PutSchedTask( (int8 *)QuePtr, task, --task_cnt, task_signal, task_period);  // reschedule the task
   		}
 	}  // while
}

int8 Proc1MsQue( int8 signal )
{
	milliseconds++;
 	ProcSchedQue( &_1msQ_u._1msqHdr );
 	if (_1ms_slip)      // if we've missed any signals
 	{
  		_1ms_slip--;
  		return( SGNL_TIMER_BIT );      // recover lost pulses by getting requeued immediately
 	}
 	return(SGNL_NO_REQUEUE);      // No Requeue
}

int8 Proc10MsQue( int8 signal )
{
//	if (DEBUG_PULSE_PORT & (1 << DEBUG_PULSE_BIT))
//		DEBUG_PULSE_PORT &= (1 << DEBUG_PULSE_BIT)^0xFFFF;
//	else
//	 	DEBUG_PULSE_PORT |= (1 << DEBUG_PULSE_BIT);
	 	
 	ProcSchedQue( &_10msQ_u._10msqHdr );
 	return( SGNL_NO_REQUEUE );      // No Requeue
}

int8 Proc100MsQue( int8 signal )
{	 	
 	ProcSchedQue( &_100msQ_u._100msqHdr );
 	return( SGNL_NO_REQUEUE );      // No Requeue
}

int8 Proc1SecQue( int8 signal )
{
	seconds++;
 	ProcSchedQue( &_1SecQ_u._1secqHdr );
 	return( SGNL_NO_REQUEUE );      // No Requeue
}

int8 Proc10SecQue( int8 signal )
{
 	ProcSchedQue( &_10SecQ_u._10secqHdr );
 	return( SGNL_NO_REQUEUE );      // No Requeue
}


/////////////////////////////////////////////////////////////////////////
// Core_DelTaskFromSchedQue
// 
// Processes the spec'd schedule queue
/////////////////////////////////////////////////////////////////////////
void Core_DelTaskFromSchedQue(enum SchedQue Que, int8 TaskIndex )
{
 int8 count;
 int8 task_cnt;
 int8 task, task_signal, task_period;
 int8 *Qbuf_ptr;
 QueHdr *QuePtr;

    if ((int)Que > MAX_SCHED_QUE)
    {
        Core_CodeError( BAD_QUEUE_INDEX );
    }    
    QuePtr = (QueHdr *) SchedQueLookup[ (int)Que ];
 	// get a copy of the count 
 	count = QuePtr->CNT;

 	// while something in the queue
 	while(count--)
 	{
  		Qbuf_ptr = GetSchedTask(  (int8 *)QuePtr );

  		// Get the period count
  		task_cnt = *Qbuf_ptr++;

  		// Get the task index            
  		task = *Qbuf_ptr++;

  		// Get the task tag            
  		task_signal = *Qbuf_ptr++;

  		// Get the task period            
  		task_period = *Qbuf_ptr;

  		// if the period timer has expired or has been truncated for some reason, 
  		// signal the task to run
  		if ((task & 0x7F) != TaskIndex)
  		{
   			PutSchedTask( (int8 *)QuePtr, task, task_cnt, task_signal, task_period);  // reschedule the task
  		}
  		else
  		{
   			// Clear queued flag
   			TaskCtlBlk[ task & 0x7F ].Cnfg &= 0xff ^ TBLK_IN_SCHED_QUE_MASK;
   			// clear the truncate bit
   			ClrTaskTruncate( task );
  		}
 	}  // while
}

#ifdef OLD_DELTASK

/////////////////////////////////////////////////////////////////////////
// DelTaskFromSchedQue
// 
// Processes the spec'd schedule queue
/////////////////////////////////////////////////////////////////////////
void DelTaskFromSchedQue(QueHdr *QuePtr, int8 TaskIndex )
{
 int8 count;
 int8 task_cnt;
 int8 task, task_signal, task_period;
 int8 *Qbuf_ptr;

 	// get a copy of the count 
 	count = QuePtr->CNT;

 	// while something in the queue
 	while(count--)
 	{
  		Qbuf_ptr = GetSchedTask(  (int8 *)QuePtr );

  		// Get the period count
  		task_cnt = *Qbuf_ptr++;

  		// Get the task index            
  		task = *Qbuf_ptr++;

  		// Get the task tag            
  		task_signal = *Qbuf_ptr++;

  		// Get the task period            
  		task_period = *Qbuf_ptr;

  		// if the period timer has expired or has been truncated for some reason, 
  		// signal the task to run
  		if ((task & 0x7F) != TaskIndex)
  		{
   			PutSchedTask( (int8 *)QuePtr, task, task_cnt, task_signal, task_period);  // reschedule the task
  		}
  		else
  		{
   			// Clear queued flag
   			TaskCtlBlk[ task & 0x7F ].Cnfg &= 0xff ^ TBLK_IN_SCHED_QUE_MASK;
   			// clear the truncate bit
   			ClrTaskTruncate( task );
  		}
 	}  // while
}

void DelTaskFrom1MsQue( int8 TaskIndex )
{
 	DelTaskFromSchedQue( &_1msQ_u._1msqHdr, TaskIndex );
}

void DelTaskFrom10MsQue( int8 TaskIndex )
{
 	DelTaskFromSchedQue( &_10msQ_u._10msqHdr, TaskIndex );
}

void DelTaskFrom100MsQue( int8 TaskIndex )
{
 	DelTaskFromSchedQue( &_100msQ_u._100msqHdr, TaskIndex );
}

void DelTaskFrom1SecQue( int8 TaskIndex )
{
 	DelTaskFromSchedQue( &_1SecQ_u._1secqHdr, TaskIndex );
}

void DelTaskFrom10SecQue( int8 TaskIndex )
{
 	DelTaskFromSchedQue( &_10SecQ_u._10secqHdr, TaskIndex );
}
#endif
/////////////////////////////////////////////////////////////////////////
// ProcSigTaskQue
//
// Critical root task which is the first task placed in the 
// run queue at power up. 
//
// This task scans the task control block array and checks
// for signalled tasks. if a task is signalled it put in
// the run queue. After that, this task re-queues itself
// in the run queue task.
//  
/////////////////////////////////////////////////////////////////////////
int8 ProcSigTaskQue( int8 signal )
{
 signed int index;
 TskCtlBlk *tptr;
 
 	index = 1;
 	tptr = &(TaskCtlBlk[ index ]);
 	go_to_sleep = TRUE;
   	for (; index<TASKCNT; index++)
 	{
  		if ( tptr->Signal && (tptr->cnfg & TBLK_CNFG_ENABLE_MASK) )
  		{
   			PutRunQueTask( index );
  		}
  		tptr++;
 	}
 	if (RunQue.CNT)
 		go_to_sleep = FALSE;
 	else
 		go_to_sleep = TRUE;
 		
 	// Queue this process to run at the end of the line
 	PutRunQueTask( PROC_SIGQUE );
 	//bit_clear(PORTB,3); // diag

 	return( SGNL_GENERIC_MASK ); // always re-queue
}

///////////////////////////////////////////////////////////////////////////
// Heartbeat LED task
///////////////////////////////////////////////////////////////////////////
int8 ProcLedPulser( int8 signal )
{
    static int8 led_pulse_cnt, led_pulse_cnt_init = LED_PULSE_OK;

    switch ((uint8) signal)
    {
        case SGNL_LED_PULSE_INIT_MASK:
            
            led_pulse_cnt = led_pulse_cnt_init;
            MONITOR_LED = 0; // lights out!
            // Run Pulser in 180 ms
            //Put10MsSchedRtn_wSignal( PROC_LEDPULSER, 18, SGNL_LED_PULSE_ACTIVE_MASK );
            Core_SchedTask(_10MS_QUE, PROC_LEDPULSER, 18, SGNL_LED_PULSE_ACTIVE_MASK, 0);
             
            break;
            
        case SGNL_LED_PULSE_ACTIVE_MASK:
            
            if (--led_pulse_cnt)
            {   
                // flip the led
                MONITOR_LED = MONITOR_LED ^ 1;
                // back in 180 ms
                //Put10MsSchedRtn_wSignal( PROC_LEDPULSER, 18, SGNL_LED_PULSE_ACTIVE_MASK);
                Core_SchedTask(_10MS_QUE, PROC_LEDPULSER, 18, SGNL_LED_PULSE_ACTIVE_MASK, 0);
            }
            else
            {
                // back in 3 seconds
                //Put1SecSchedRtn_wSignal( PROC_LEDPULSER, 3, SGNL_LED_PULSE_INIT_MASK);
                Core_SchedTask(_1SEC_QUE, PROC_LEDPULSER, 3, SGNL_LED_PULSE_INIT_MASK, 0);
            }    
            break;
    }
 	return( 0 );      // No Requeue
}

/////////////////////////////////////////////////////////////////////////
// ExecTask
/////////////////////////////////////////////////////////////////////////
void ExecTask( uint8 TaskIndex )
{
 int8 TaskSignal;

 	TaskSignal = TaskCtlBlk[ (short) TaskIndex ].Signal;
    
    if (TaskIndex < TASKCNT)
    {
        TaskSignal = core_tasks[ TaskIndex ]( TaskSignal );
    }
    else
        FatalError( FE_UNKNOWN_TASK );

 TaskCtlBlk[ (short)TaskIndex ].Signal = TaskSignal;
}

/////////////////////////////////////////////////////////////////////////
// CoreMain
/////////////////////////////////////////////////////////////////////////
int CoreMain()   
{
 int8 intTask;
 TskCtlBlk *tptr;
 int current_cpu_ipl;

  	//Disable_Interrupts( INTR_GLOBAL );
    
  	SET_AND_SAVE_CPU_IPL(current_cpu_ipl, 7);  /* disable interrupts */
   	/* protected code here */
	
  	cnt_1ms = ISR_1MS_INIT;

  	_1ms_slip = 0; _1ms_slips = 0;

  	// init the tasks signal words
  	for( intTask = 0, tptr = &(TaskCtlBlk[ (short)intTask ]); intTask < TASKCNT; intTask++)
  	{
   		tptr->Signal = 0;
   		tptr->cnfg = TBLK_CNFG_ENABLE_MASK;
   		tptr++;
  	}

  	// Signal the root task
  	SignalTask( PROC_SIGQUE, SGNL_GENERIC_BIT );

  	InitQue( (int8 *)&RunQue, RUNQUESIZE-2 );
  	InitQue( (int8 *)&_1msQ_u._1msqHdr, ((_1MSQUESIZE-2)*QRECSIZE) );
  	InitQue( (int8 *)&_10msQ_u._10msqHdr, ((_10MSQUESIZE-2)*QRECSIZE) );
  	InitQue( (int8 *)&_100msQ_u._100msqHdr, ((_100MSQUESIZE-2)*QRECSIZE) );
  	InitQue( (int8 *)&_1secQ_u._1secqHdr, ((_1SECQUESIZE-2)*QRECSIZE) );
  	InitQue( (int8 *)&_10secQ_u._10secqHdr, ((_10SECQUESIZE-2)*QRECSIZE) );

  	// Schedule 10 ms task to run every 10ms
  	//Put1MsSchedRtn( (PROC_10MSQUE | 0x80), 10);
    Core_SchedTask(_1MS_QUE, PROC_10MSQUE | 0x80, 10, SGNL_TIMER_MASK, 10);
  	// Schedule 100 ms task to run every 100ms
  	//Put10MsSchedRtn( (PROC_100MSQUE | 0x80), 10 );
    Core_SchedTask(_10MS_QUE, PROC_100MSQUE | 0x80, 10, SGNL_TIMER_MASK, 10);
  	// Schedule 1 s task to run every 1 s
  	//Put100MsSchedRtn( (PROC_1SECQUE | 0x80), 10 );
    Core_SchedTask(_100MS_QUE, PROC_1SECQUE | 0x80, 10, SGNL_TIMER_MASK, 10);
  	// Schedule 10 s task to run every 10 s
  	//Put1SecSchedRtn( (PROC_10SECQUE | 0x80), 10 );
    Core_SchedTask(_1SEC_QUE, PROC_10SECQUE | 0x80, 10, SGNL_TIMER_MASK, 10);
  	// Schedule ledpulse task to run every 3 s
    //Put1SecSchedRtn_wSignal( PROC_LEDPULSER, 3, SGNL_LED_PULSE_INIT_MASK);
    Core_SchedTask(_1SEC_QUE, PROC_LEDPULSER, 3, SGNL_LED_PULSE_INIT_MASK, 0);

  	PutRunQueTask ( PROC_SIGQUE );  // Que the root signalling task checker
	
	//Initialize the sleep flag
	go_to_sleep = FALSE;
	
  	// Initialize the resource locks
  	RsrcInit();               
  	// initial user stuff defined in Core.h
  	Core_InitUser();               

  	// Set up a pointer to speed the timer isr
  	_1msTask_signal_ptr = GetTaskSignalByteAddr( PROC_1MSQUE );
 
  	RESTORE_CPU_IPL(current_cpu_ipl);
  	Core_EnableUserISRs();
    IEC0bits.T1IE = 1;   // enable timer1 interrupt

  	//enable_interrupts( INT_RDA );
  	//enable_interrupts( INT_RDA2 );

	/////////////////////////////////////////////////////////////////////////
	// Run queue processing loop
	/////////////////////////////////////////////////////////////////////////
  	while (RunQue.CNT)   // while there's something in the runque
  	{
   		SET_AND_SAVE_CPU_IPL(current_cpu_ipl, 7);  /* disable interrupts */

   		intTask = GetRunQueTask();   // Get the next task to run
   		ExecTask( intTask );         // run it
   
   		/* enable interrupts */
   		RESTORE_CPU_IPL(current_cpu_ipl);
   		
   		if (go_to_sleep)
   			//idle();
   			asm ("pwrsav #1");
  
   		asm ("nop");

  	}
  	// Something's wrong when there's nothing in the run que
  	FatalError( RUNQUE_EMPTYQUE );
  
  	return(TRUE);
}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//void Timer1Isr()
void CoreTmr1InterruptCallback(void)
{
  	IFS0bits.T1IF = 0;		 //  interrupt served

  	if (--cnt_1ms == 0)
  	{
	  	
   		cnt_1ms = ISR_1MS_INIT;   // re-init 1 ms counter

   		// if 1ms process han't run yet
   		if ( *_1msTask_signal_ptr & SGNL_TIMER_MASK )
   		{
    		_1ms_slip++; // count it
    		_1ms_slips++;
   		}
   		else
   		{
    		// signal the 1 ms task to run
    		*_1msTask_signal_ptr |= SGNL_TIMER_MASK;
   		}
  	}
}
  
int8 FatalError( int8 ErrorCode )
{
 int8 temp;
 
 	while(TRUE)
 	{
  		MONITOR_LED = 0;
  		Delay_ms(2000);      // delay 1 sec
  		MONITOR_LED = 1;
  		Delay_ms(2000);      // delay 1 sec
  		MONITOR_LED = 0;
  		Delay_ms(4000);      // delay 1 sec

  		temp = ErrorCode;
  		while (temp--)
  		{
   			MONITOR_LED = 1;
   			Delay_ms(500);      // delay 200 msec
   			MONITOR_LED = 0;
   			Delay_ms(500);      // delay 200 sec
  		}
 	}
 	return(FALSE);      // No Requeue
}

int8 Core_CodeError( int8 ErrorCode )
{
 int8 temp;
 
 	while(TRUE)
 	{
  		MONITOR_LED = 0;
  		Delay_ms(2000);      // delay 600 msec
  		MONITOR_LED = 1;
  		Delay_ms(2000);      // delay 600 msec
  		MONITOR_LED = 0;
  		Delay_ms(2000);      // delay 600 msec
  		MONITOR_LED = 1;
  		Delay_ms(2000);      // delay 600 msec
  		MONITOR_LED = 0;
  		Delay_ms(4000);      // delay 600 msec

  		temp = ErrorCode;
  		while (temp--)
  		{
   			MONITOR_LED = 1;
   			Delay_ms(500);      // delay 200 msec
   			MONITOR_LED = 0;
   			Delay_ms(500);      // delay 200 sec
  		}
 	}
 	return(FALSE);      // No Requeue
}



/////////////////////////////////////////////////////////////////////////
// char *Core_GetStats( int8 Clear, char *buf  )
//
// Returns a report of errors for the Core.
// Stats get cleared if Clear is non-zero.
// The report is composed of eight numbers coverted to
// ascii separated by commas.
//	int8 _1ms_slip;
//
// char *Core_IntToAsciiStr( int16 i, char *str, int8 min_digs )
/////////////////////////////////////////////////////////////////////////
char *Core_GetStats( int8 Clear, char *buf )
{
	_stat_append( (unsigned long)_1ms_slips, buf, FALSE,3 );

	if (Clear)
	{
		_1ms_slips = 0;
	}  	
 	return( buf );
}

uint32 Core_milliseconds()
{
	return( milliseconds );
}

uint32 Core_seconds()
{
	return( seconds );
}



