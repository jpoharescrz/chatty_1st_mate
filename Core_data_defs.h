/////////////////////////////////////////////////////////////////////////
////                          Core_data.H                            ////
////         
//// This file is included in Core.c before any user includes                                                        ////
/////////////////////////////////////////////////////////////////////////
#ifndef CORE_DATA_H
#define CORE_DATA_H

#ifdef CORE

#define RUNQUESIZE	TASKCNT

// Task control block structure
typedef struct T_Ctl_blk {
 int8 Signal;
 int8 Period;
 int8 Cnfg;
 int8 Scratch;
 int8 tag;
 int8 null0;
 int8 null1;
 int8 null2;
}TskCtlBlk;

#define cnfg Cnfg
#define scratch Scratch
#define period Period
#define signal Signal

#define TBLK_CNFG_ENABLE_BIT 7
#define TBLK_IN_SCHED_QUE_BIT 6
#define TBLK_TASK_ERROR_BIT 5
#define TBLK_TASK_TRUNCATE_BIT 4

#define TBLK_CNFG_ENABLE_MASK (1<<TBLK_CNFG_ENABLE_BIT)
#define TBLK_IN_SCHED_QUE_MASK (1<<TBLK_IN_SCHED_QUE_BIT)
#define TBLK_TASK_ERROR_MASK (1<<TBLK_TASK_ERROR_BIT)
#define TBLK_TASK_TRUNCATE_MASK (1<<TBLK_TASK_TRUNCATE_BIT)

#define TBLK_TAG_NULL 0
#define TBLK_TAG_WATCHDOG 1

// define the task control block array
TskCtlBlk TaskCtlBlk[ TASKCNT ];

// Scheduling queue header
typedef struct QHeader {
 int8 MaxOffset;
 int8 WT_OFF;
 int8 RD_OFF;
 int8 CNT;
}QueHdr;

// Scheduling queue record
typedef struct Qrec {
int8 count;
int8 taskId;
int8 signal;
int8 period;
}QueRecord;

#define QRECSIZE sizeof(QueRecord)

QueHdr Runque;                    // Always put the header
#define RunQue Runque
#define runque Runque
int8 RunqBuf[ (RUNQUESIZE  * QRECSIZE) ];       // over the buffer

union _1ms_utag {
  QueHdr _1msqHdr;
  int8 _1msqBuf[ (_1MSQUESIZE * QRECSIZE) + sizeof(QueHdr) ];     // 1 ms queue
  } _1msQ_u;
#define _1MsQ_u _1msQ_u
#define _1Msq_u _1msQ_u

union _10ms_utag {
  QueHdr _10msqHdr;
  int8 _10msqBuf[ (_10MSQUESIZE  * QRECSIZE) + sizeof(QueHdr) ];   // 10 ms queue
  } _10msQ_u;
#define _10MsQ_u _10msQ_u
#define _10Msq_u _10msQ_u

union _100ms_utag {
  QueHdr _100msqHdr;
  int8 _100msqBuf[ (_100MSQUESIZE  * QRECSIZE) + sizeof(QueHdr) ];  // 100 ms queue
  } _100msQ_u;
#define _100MsQ_u _100msQ_u
#define _100Msq_u _100msQ_u

union _1sec_utag {
  QueHdr _1secqHdr;
  int8 _1secqBuf[ (_1SECQUESIZE  * QRECSIZE) + sizeof(QueHdr) ];     // 1 Sec queue
  } _1secQ_u;
#define _1SecQ_u _1secQ_u 
#define _1Secq_u _1secQ_u  

union _10sec_utag {
  QueHdr _10secqHdr;
  int8 _10secqBuf[ (_10SECQUESIZE  * QRECSIZE) + sizeof(QueHdr) ];    // 10 Sec queue
  } _10secQ_u;
#define _10SecQ_u _10secQ_u
#define _10Secq_u _10secQ_u

int8 *SchedQueLookup[] = {  &_1msQ_u._1msqBuf[0], 
                            &_10msQ_u._10msqBuf[0],
                            &_100msQ_u._100msqBuf[0],
                            &_1secQ_u._1secqBuf[0],
                            &_10secQ_u._10secqBuf[0] };




int8 cnt_1ms, _1ms_slip, go_to_sleep;
uint16 _1ms_slips;
int8 *_1msTask_signal_ptr;

uint32 milliseconds, seconds;

#endif // CORE

enum SchedQue {
    _1MS_QUE,
    _10MS_QUE,
    _100MS_QUE,
    _1SEC_QUE,
    _10SEC_QUE
};

typedef enum SchedQue CoreTimerQue;

#ifdef CORE
#define MAX_SCHED_QUE (int)_10SEC_QUE
#endif

#endif // _CORE_DATA_DEFS
