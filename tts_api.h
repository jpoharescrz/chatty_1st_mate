
#include "Tts_CmdMap.h"
#include "tts_cnfg.h"
#include "tts_data_defs.h"


/////////////////////////////////////////////////////////////////
// void TtsSpellString( int16 pre_indices, char *ptr, int16 post_indices, int16 options)
//
// Spells a string.
// 
// Queues up to 4 things to say before the spelled-string and up to 4 things
// after. 
// pre_indices and post_indices are evaluated left to right
// upper byte to lower byte. Bytes equal to 0xFF within the indices
// are ignored.
// 
// The options word uses the following bits
//
// #define TTS_OPT_CLIP_MASK 0x200
//  If set, the phrase is clippable
// #define TTS_OPT_EXPL_DIGIT_MASK 0x100
//  If set, all digits are spoken including leading zeros
// #define TTS_OPT_HIGH_PRIORITY_QUEUE_MASK 0x80
//  If set, the high priority buffer is used
// #define TTS_OPT_FORCE_MASK 0x40
//  If set, the CmdDiscardCnt is set such that the command
//  is forced to the front of the buffer discarding all in front
// #define TTS_OPT_BLOCK_PERIOD_MASK 0x3F
//  If non-zero, then buffer is blocked for the specified period
//  after the msg.
//  0x01-0x0F: block for 1-16 ms
//  0x11-0x1F: block for 10-160 ms
//  0x21-0x2F: block for 100-1600 ms
//  0x31-0x3F: block for 1 sec to 16 sec
/////////////////////////////////////////////////////////////////
void TtsSpellString ( uint32 pre_index, char *ptr, uint32 post_index, int16 options);

/////////////////////////////////////////////////////////////////
// void TtsSayNumber( int32 pre_number_indices, char *ptr, int32 post_number_indices, int16 options)
//
// Says a number.
// 
// Queues up to 4 things to say before the number and up to 4 things
// after the number. 
// pre_number_indices and post_number_indices are evaluated left to right
// upper byte to lower byte. Bytes equal to 0xFF within the indices
// are ignored.
// 
// The options word uses the following bits
//
// #define TTS_OPT_CLIP_MASK 0x200
//  If set, the phrase is clippable
// #define TTS_OPT_EXPL_DIGIT_MASK 0x100
//  If set, all digits are spoken including leading zeros
// #define TTS_OPT_HIGH_PRIORITY_QUEUE_MASK 0x80
//  If set, the high priority buffer is used
// #define TTS_OPT_FORCE_MASK 0x40
//  If set, the CmdDiscardCnt is set such that the command
//  is forced to the front of the buffer discarding all in front
// #define TTS_OPT_BLOCK_PERIOD_MASK 0x3F
//  If non-zero, then buffer is blocked for the specified period
//  after the msg.
//  0x01-0x0F: block for 1-16 ms
//  0x11-0x1F: block for 10-160 ms
//  0x21-0x2F: block for 100-1600 ms
//  0x31-0x3F: block for 1 sec to 16 sec
/////////////////////////////////////////////////////////////////
uint8 TtsSayNumber( int32 pre_number_indices, char *ptr, int32 post_number_indices, int16 options);

/////////////////////////////////////////////////////////////////
// void TtsSaySomething( unsigned int16 p0, unsigned int16 p1, unsigned int16 p2, unsigned int16 p3, int16 options)
//
// Queues up to 8 things to say. p0-p3 are evaluated left to right
// upper byte to lower byte. Bytes equal to 0xFF within P0-P3 
// are ignored.
// 
// The options word uses the following bits
//
// #define TTS_OPT_CLIP_MASK 0x200
//  If set, the phrase is clippable
// #define TTS_OPT_HIGH_PRIORITY_QUEUE_MASK 0x80
//  If set, the high priority buffer is used
// #define TTS_OPT_FORCE_MASK 0x40
//  If set, the CmdDiscardCnt is set such that the command
//  is forced to the front of the buffer discarding all in front
// #define TTS_OPT_BLOCK_PERIOD_MASK 0x3F
//  If non-zero, then buffer is blocked for the specified period
//  after the msg.
//  0x01-0x0F: block for 1-16 ms
//  0x11-0x1F: block for 10-160 ms
//  0x21-0x2F: block for 100-1600 ms
//  0x31-0x3F: block for 1 sec to 16 sec
/////////////////////////////////////////////////////////////////
uint8 TtsSaySomething( unsigned int16 p0, unsigned int16 p1, unsigned int16 p2, unsigned int16 p3, int16 options);

/////////////////////////////////////////////////////////////////
// int8 TtsQueSpeech( int8 Cmd, char *argptr, int8 options )
//
// Queues a command for the TTS task to process. Typically,
// a speech command
// Cmd, and argptr[] are written directly to the TTS command
// buffer. The options byte uses the following bits
//
// #define TTS_OPT_HIGH_PRIORITY_QUEUE_MASK 0x80
//  If set, the high priority buffer is used
// #define TTS_OPT_FORCE_MASK 0x40
//  If set, the CmdDiscardCnt is set such that the command
//  is forced to the front of the buffer discarding all in front
// #define TTS_OPT_BLOCK_PERIOD_MASK 0x3F
//  If non-zero, then buffer is blocked for the specified period
//  after the msg.
//  0x01-0x0F: block for 1-16 ms
//  0x11-0x1F: block for 10-160 ms
//  0x21-0x2F: block for 100-1600 ms
//  0x31-0x3F: block for 1 sec to 16 sec
/////////////////////////////////////////////////////////////////
int8 TtsQueSpeech( int8 Cmd, char *argptr, int16 options );

/////////////////////////////////////////////////////////////////
// uint8 TtsQueSpeech2( uint8 Cmd, uint8 arg0, uint8 options )
//
// Queues a command for the TTS task to process. Typically,
// an admin command
// Cmd, and arg0 are written directly to the TTS command
// buffer. The options byte uses the following bits
//
// #define TTS_OPT_HIGH_PRIORITY_QUEUE_MASK 0x80
//  If set, the high priority buffer is used
/////////////////////////////////////////////////////////////////
int8 TtsQueSpeech2( uint8 Cmd, uint8 arg0, uint16 options );

/////////////////////////////////////////////////////////////////
// int8 Tts_IsMsgPlaying()
//
// Returns TRUE when any channel is playing a voice msg on the 
// TTS module. But only if it's not clippable.
/////////////////////////////////////////////////////////////////
int8 Tts_IsMsgPlaying();

//////////////////////////////////////////////////////////////////////
// void TTS_InitProcTask()
//
// Initializes the data structures fo rthe TTS task
/////////////////////////////////////////////////////////////////////
void TTS_InitTask();

///////////////////////////////////////////////////////////////////
// char TTS_IO_GetInMsgCnt( )
// 
// Returns a count of Messages in the incoming buffer
///////////////////////////////////////////////////////////////////
uint8 TTS_IO_GetInMsgCnt();

///////////////////////////////////////////////////////////////////
// char TTS_IO_GetOutMsgCnt( )
// 
// Returns a count of Messages in the outgoing buffer
///////////////////////////////////////////////////////////////////
uint8 TTS_IO_GetOutMsgCnt();

///////////////////////////////////////////////////////////////////
// char TTS_IO_GetInToken( int8 token )
// 
// Returns a pointer to the specified token in the incoming buffer
///////////////////////////////////////////////////////////////////
unsigned char *TTS_IO_GetInToken( int8 token );

///////////////////////////////////////////////////////////////////
// char *TTS_IO_GetOutToken( int8 token )
// 
// Returns a pointer to the specified token in the outgoing buffer
///////////////////////////////////////////////////////////////////
unsigned char *TTS_IO_GetOutToken( int8 token );

///////////////////////////////////////////////////////////////////
// int8 TTS_IO_GetInTokenCnt()
// 
// Returns the number of tokens in the incoming buffer
///////////////////////////////////////////////////////////////////
uint8 TTS_IO_GetInTokenCnt();

///////////////////////////////////////////////////////////////////
// int8 TTS_IO_GetOutTokenCnt()
// 
// Returns the number of tokens in the incoming buffer
///////////////////////////////////////////////////////////////////
uint8 TTS_IO_GetOutTokenCnt();

///////////////////////////////////////////////////////////////////
// void TTS_IO_Xmt_InMsg( char *MsgPtr )
// 
// Sends the msg to the slave in one atomic function
///////////////////////////////////////////////////////////////////
int TTS_IO_Xmt_InMsg( char *MsgPtr );

///////////////////////////////////////////////////////////////////
// void TTS_IO_Xmt_OutMsg( char *MsgPtr )
// 
// Sends the msg to the slave in one atomic function
///////////////////////////////////////////////////////////////////
int TTS_IO_Xmt_OutMsg( char *MsgPtr );

/////////////////////////////////////////////////////////////////
// void TTS_SayVersion()
//
// Announces ADSC and its version
// 
/////////////////////////////////////////////////////////////////
void TTS_SayVersion();

///////////////////////////////////////////////////////////////////
// char TTS_IO_NextOutMsgCnt( )
// 
// Sets the current message to the next one in the outgoing buffer
///////////////////////////////////////////////////////////////////
uint8 TTS_IO_NextOutMsg();

///////////////////////////////////////////////////////////////////
// char TTS_IO_NextInMsgCnt( )
// 
// Sets the current message to the next one in the outgoing buffer
///////////////////////////////////////////////////////////////////
uint8 TTS_IO_NextInMsg();

char *Tts_GetStats(  int8 Clear, char *buf );

void TTS_IO_InitTask();
void IsdSignalTask( uint8 CmdSrc );


//////////////////////////////////////////////////////////////////
// void TTS_IO_send()
//
// Increments the command count and signals the ISD Task
//////////////////////////////////////////////////////////////////
void TTS_IO_send( TTS_IO_CtrlBlkRec *p );

///////////////////////////////////////////////////////////////////
// int TTS_IO_put_OutChar( char c )
//
// Takes a character and writes it to the
// outgoing buffer. Both the general tx buffer count and
// command buffer count is incremented.
///////////////////////////////////////////////////////////////////
int TTS_IO_put_OutChar( char c );

///////////////////////////////////////////////////////////////////
// int TTS_IO_put_OutBuf( char *bptr )
//
// Takes a pointer to a character buffer an writes it to
// outgoing buffer. 
///////////////////////////////////////////////////////////////////
int TTS_IO_put_OutBuf( char *bptr );

///////////////////////////////////////////////////////////////////
// int TTS_IO_Xmt_OutMsg( char *MsgPtr )
// 
// Sends the msg to the slave in one atomic function
///////////////////////////////////////////////////////////////////
int TTS_IO_Xmt_OutMsg( char *MsgPtr );


///////////////////////////////////////////////////////////////////
// int TTS_IO_Xmt_InMsg( char *MsgPtr )
// 
// Sends the msg to the TTS Task in one atomic function
///////////////////////////////////////////////////////////////////
int TTS_IO_Xmt_InMsg( char *MsgPtr );


///////////////////////////////////////////////////////////////////
// int8 TTS_IO_GetOutFreeByteCnt()
// 
// Returns the number of free bytes in the outgoing buffer
///////////////////////////////////////////////////////////////////
int8 TTS_IO_GetOutFreeByteCnt();


///////////////////////////////////////////////////////////////////
// int8 TTS_IO_GetInFreeByteCnt()
// 
// Returns the number of free bytes in the incoming buffer
///////////////////////////////////////////////////////////////////
int8 TTS_IO_GetInFreeByteCnt();

///////////////////////////////////////////////////////////////////
// int8 TTS_IO_GetOutTokenCnt()
// 
// Returns the number of tokens in the incoming buffer
///////////////////////////////////////////////////////////////////
uint8 TTS_IO_GetOutTokenCnt();


///////////////////////////////////////////////////////////////////
// int8 TTS_IO_GetInTokenCnt()
// 
// Returns the number of tokens in the incoming buffer
///////////////////////////////////////////////////////////////////
uint8 TTS_IO_GetInTokenCnt();

///////////////////////////////////////////////////////////////////
// void TTS_IO_ClrInTokenCnt()
// 
// Clears the number of tokens in the incoming buffer
///////////////////////////////////////////////////////////////////
void TTS_IO_ClrInTokenCnt();

///////////////////////////////////////////////////////////////////
// char *TTS_IO_GetOutToken( int8 token )
// 
// Returns a pointer to the specified token in the outgoing buffer
///////////////////////////////////////////////////////////////////
uint8 *TTS_IO_GetOutToken( int8 token );


///////////////////////////////////////////////////////////////////
// char TTS_IO_GetInToken( int8 token )
// 
// Returns a pointer to the specified token in the incoming buffer
///////////////////////////////////////////////////////////////////
uint8 *TTS_IO_GetInToken( int8 token );


///////////////////////////////////////////////////////////////////
// char TTS_IO_GetOutMsgCnt( )
// 
// Returns a count of Messages in the outgoing buffer
///////////////////////////////////////////////////////////////////
uint8 TTS_IO_GetOutMsgCnt();


///////////////////////////////////////////////////////////////////
// char TTS_IO_GetInMsgCnt( )
// 
// Returns a count of Messages in the incoming buffer
///////////////////////////////////////////////////////////////////
uint8 TTS_IO_GetInMsgCnt();

void Tts_UnMute();
void Tts_Mute( int16 timeout );
