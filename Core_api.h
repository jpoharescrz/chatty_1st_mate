/////////////////////////////////////////////////////////////////////////
////                          Core_api.H                              ////
////         
//// This file is included in Core.c before any user includes                                                        ////
/////////////////////////////////////////////////////////////////////////
#define int8  char
#define uint8 unsigned char
#define byte unsigned char
#define int16 short
#define uint16 unsigned short
#define word unsigned short
#define int32 long
#define uint32 unsigned long

//enum true_false {
//    FALSE,
//   TRUE
//};
#define TRUE 1
#define FALSE 0

#define ROUND_UP TRUE
#define ROUND_DOWN FALSE

#include <string.h>
#include "Core24_hw_defs.h"
#include "Core_cnfg.h"

#define make_bit_mask(a) (1<<a)
#define bit_test( k,k_bit ) ( k & (1 << k_bit))

// Protocol
void RsrcFreeLock( int Resource );
int8 RsrcIsLockFree( int Resource );
int8 RsrcGetLockOwner( int Resource );
void RsrcInit();

int8 RsrcSetLockOwner( int Resource, int8 Owner );
void Delay_ms( uint16 dly_ms );
void Delay_us( uint16 dly_us );

int8 PutSchedTask(int8 *ptr, int8, int8, int8, int8);
void ClrTaskSignal(int8 task, int8 sigbitnum);
void SetTaskSignalByte(int8 task, int8 signal);
void SetTaskPeriod( int8 task, int8 TaskPeriod );
int8 IsTaskInSchedQue( int8 task );
void Put1MsSchedRtn(int8 TaskIndex, int8 period_cnt);
void Put10MsSchedRtn(int8 TaskIndex, int8 period_cnt);
void Put100MsSchedRtn(int8 TaskIndex, int8 period_cnt);
void Put100MsSchedRtn_wSignal(int8 TaskIndex, int8 period_cnt, int8 Signal);
void Put1SecSchedRtn(int8 TaskIndex, int8 period_cnt);
void Put1SecSchedRtn_wSignal(int8 TaskIndex, int8 period_cnt, int8 Signal);
void Put10SecSchedRtn(int8 TaskIndex, int8 period_cnt);
void DelTaskFrom1MsQue( int8 TaskIndex );
void DelTaskFrom10MsQue( int8 TaskIndex );
void DelTaskFrom100MsQue( int8 TaskIndex );
void DelTaskFrom1SecQue( int8 TaskIndex );
void DelTaskFrom10SecQue( int8 TaskIndex );

int8 GetTaskSignal(int8 task, int8 sigbitnum);
int8 GetTaskSignalByte(int8 task );
void EnableTask( int8 task );
void DisableTask( int8 task );
void SetTaskError( int8 task );
void ClrTaskError( int8 task );
int8 IsTaskInError( int8 task );
void SetTaskTruncate( int8 task );
void ClrTaskTruncate( int8 task );
int8 IsTaskTruncated( int8 task );
void SetTaskScratchBit( int8 task, int8 mask );
void ClrTaskScratchBit( int8 task, int8 mask );
int8 IsTaskScratchBitSet( int8 task, int8 mask );
void SetToneHi();
void SetToneWayHi();
void SetToneLo();
void SignalTask(int8 task, int8 sigbitnum);

int8 Core_SchedTask(CoreTimerQue Que, int8 TaskIndex, int8 period_cnt, int8 Signal, int8 period);
void Core_DelTaskFromSchedQue(enum SchedQue Que, int8 TaskIndex );

//void Core_SetProcToneOff();
//void ToneBeep(int8 tone_on_period);
char *int8_to_ascii10( unsigned int8 num, char *ptr);
int16 AddModulo( int16 initial_value, int16 add1, int16 modulo);
int16 SubModulo( int16 initial_value, int16 sub1, int16 modulo);
int8 MinInt8( int8 a, int8 b );
int8 MaxInt8( int8 a, int8 b );
int16 MinInt16( int16 a, int16 b );
int16 MaxInt16( int16 a, int16 b );
uint16 AbsInt16( int16 val );
int16 Core_get_modulo_abs_difference( int16 p0, int16 p1, int16 mod );
int16 Core_get_modulo_difference( int16 p0, int16 p1, int16 mod );
char *Core_IntToAsciiStr( long i, char *str, int8 min_digs );
char *Core_ValueToStr( int16 value, char *cptr, int8 min_digs );
int16 Core_StrToValue(char *cptr);
int16 Core_Round( int16 val, int8 up, int16 interval, int16 mod);
char *Core_GetStats( int8 Clear, char *buf );
char *Core_HiResValueToStr( int32 value, char *cptr, int8 min_int_digs, int8 min_dec_digs );
int32 Core_StrToHiResValue(char *cptr);

uint32 Core_milliseconds();
uint32 Core_seconds();

extern const char tenths_to_sixteenths[];

void write_flash_word( int32 flash_address, int32 data_32);
int32 read_flash_word( int32 flash_address );
void read_program_memory( uint32 bptr, uint8 *buffer, int bytecnt );
void erase_flash_memory_block( int32 flash_address );
void write_flash_buffer( uint32 flash_addr, uint8 *dptr, uint16 count);

int8 FatalError( int8 );
int8 Core_CodeError( int8 ErrorCode );
void reset_cpu();
void PutRunQueTask( int8 TaskIndex );
void _stat_append( unsigned long i, char *p, int8 append_delim, int8 width);
char *Core_GetVersion();

char *strhaschar( char *ptr, char c );
int8 IsTaskInRunQue( int8 _task );
void piezo_on( uint8 on );
void debug_pulse_on( uint8 on );

void UnLockPIO();
void LockPIO();

int CoreMain();
void CoreTmr1InterruptCallback(void);






