/////////////////////////////////////////////////////////////////////////
////                          Tts.c                              ////
////                                                                 ////

#ifndef TTS
#define TTS
#include <xc.h>
#include <p24FJ128GB204.h>
#include <string.h>
#include "Core_api.h"
#include "isd_api.h"
//#include "pots_api.h"
#include "button_api.h"
#include "rs232_api.h"
#include "fmt_api.h"
#include "tts_api.h"
#include "tts_data_defs.h"

// External prototypes
void Src_Targeting_Tone_Suspend();

// Internal prototypes
int8 TtsQueSpeech2( uint8 Cmd, uint8 arg0, uint16 options );
void TtsSendStopMsg( int8 );
void TTS_InitDigitalPots();
int8 _tts_proc_next_cmd( CmdBuffer *ptr);

//////////////////////////////////////////////////////////////////////
// void TTS_InitProcTask()
//
// Initializes the data structures fo rthe TTS task
/////////////////////////////////////////////////////////////////////
void TTS_InitTask()
{
#ifndef TTS_FORCE_HI_PRIORITY
 	tts_control.CmdBuffers[ LOW_PRIORITY_BUFFER ].CmdCnt = 0;
 	tts_control.CmdBuffers[ LOW_PRIORITY_BUFFER ].CmdByteCnt = 0;
 	tts_control.CmdBuffers[ LOW_PRIORITY_BUFFER ].CmdDiscardCnt = 0;
 	tts_control.CmdBuffers[ LOW_PRIORITY_BUFFER ].CmdRdIdx = 0;
 	tts_control.CmdBuffers[ LOW_PRIORITY_BUFFER ].CmdWtIdx = 0;
 	tts_control.CmdBuffers[ LOW_PRIORITY_BUFFER ].CmdBufferState = TTS_TASK_WAIT;
 	tts_control.CmdBuffers[ LOW_PRIORITY_BUFFER ].CmdBufferPriority = 0;
 	tts_control.CmdBuffers[ LOW_PRIORITY_BUFFER ].CmdEventTimeout = TTS_EVT_TIMEOUT;
 	tts_control.CmdBuffers[ LOW_PRIORITY_BUFFER ].CmdWaitingForEvent = TTS_EVT_PUP;
 	tts_control.CmdBuffers[ LOW_PRIORITY_BUFFER ].CmdErrsUnAcked = 0;
 	tts_control.CmdBuffers[ LOW_PRIORITY_BUFFER ].AliasErrs = 0;
#endif
 	tts_control.CmdBuffers[ HIGH_PRIORITY_BUFFER ].CmdCnt = 0;
 	tts_control.CmdBuffers[ HIGH_PRIORITY_BUFFER ].CmdByteCnt = 0;
 	tts_control.CmdBuffers[ HIGH_PRIORITY_BUFFER ].CmdDiscardCnt = 0;
 	tts_control.CmdBuffers[ HIGH_PRIORITY_BUFFER ].CmdRdIdx = 0;
 	tts_control.CmdBuffers[ HIGH_PRIORITY_BUFFER ].CmdWtIdx = 0;
 	tts_control.CmdBuffers[ HIGH_PRIORITY_BUFFER ].CmdBufferState = TTS_TASK_WAIT;
 	tts_control.CmdBuffers[ HIGH_PRIORITY_BUFFER ].CmdBufferPriority = HIGH_PRIORITY_BUFFER;
 	tts_control.CmdBuffers[ HIGH_PRIORITY_BUFFER ].CmdEventTimeout = 0;
 	tts_control.CmdBuffers[ HIGH_PRIORITY_BUFFER ].CmdErrsUnAcked = 0;
 	tts_control.CmdBuffers[ HIGH_PRIORITY_BUFFER ].AliasErrs = 0;

 	tts_control.TtsBufferPlayingMsg = 0xFF;
 	tts_control.TtsSpeechVolume = TTS_CNFG_SPEECH_VOL_HI;
 	tts_control.TtsErrors_WD = 0;
 	tts_control.TtsErrors_CME = 0;
 	tts_control.TtsErrors_BF = 0;
 	tts_control.TtsErrors_BadEvents = 0;
 	tts_control.TtsErrors_CodeErrs = 0;
	tts_control.TtsErrors_isd_BF = 0;
 	Tts_SpchSpeed = TTS_SPCH_SPEED_HIGH;
	tts_control.muted = FALSE;
	tts_control.mute_timeout = 0;
 	TTS_IO_InitTask();

 	// Get the ProcTtsWdTask() running every 1 sec
 	//Put1SecSchedRtn( PROC_TTS_WD_TASK + 0x80, 0 );
    Core_SchedTask(_1SEC_QUE, PROC_TTS_WD_TASK | 0x80, 1, SGNL_TIMER_MASK, 1);
    
#ifdef USE_SPEECH_VOLUME_CMD    
 	TtsQueSpeech2( TTS_CMD_SET_SPEECH_VOLUME, Btn_GetParm(BTN_PARM_SPEECH_VOL_IDX) + 1, TTS_OPT_HIGH_PRIORITY_QUEUE_MASK );
#endif
 
}

//////////////////////////////////////////////////////////////////////
// void TtsWdChk( int8 BufNum )
//
// Called once per second to check the status of the spec'd buffer
// If waiting for an event from the TTS module for too long it
// will reset the buffer and get it moving again
/////////////////////////////////////////////////////////////////////
void TtsWdChk( int8 BufNum )
{
 CmdBuffer *pCmdBuf;

 	pCmdBuf = &(tts_control.CmdBuffers[ (int)(BufNum) ]);

 	if ( pCmdBuf->CmdEventTimeout )
 	{
  		pCmdBuf->CmdEventTimeout--;
  		if ( !(pCmdBuf->CmdEventTimeout) )
  		{
   			pCmdBuf->CmdBufferState = TTS_TASK_PROC_QUE;
   			if (tts_control.TtsBufferPlayingMsg == BufNum)
     			tts_control.TtsBufferPlayingMsg = 0xFF;
   			SignalTask( PROC_TTS_TASK, SGNL_GENERIC_BIT ); 
   			pCmdBuf->CmdErrsUnAcked++;
  		}
 	}
}

/////////////////////////////////////////////////////////////////
// void Tts_ProcWdTask( int8 signal )
//
// Separate task.
// Runs every 1 sec to check if any cmds
// to the TTS module remain unacknowledged.  
/////////////////////////////////////////////////////////////////
int8 Tts_ProcWdTask( int8 signal )
{
#ifndef TTS_FORCE_HI_PRIORITY
 	TtsWdChk( LOW_PRIORITY_BUFFER );	// check buffer 0
#endif
 	TtsWdChk( HIGH_PRIORITY_BUFFER ); // check buffer 1

	if (tts_control.mute_timeout)
	{
	 	tts_control.mute_timeout--;
	 	if (!tts_control.mute_timeout)
	 	{
			Tts_UnMute();
		}
	}

 	return( SGNL_NO_REQUEUE );
}

/////////////////////////////////////////////////////////////////
// int _tts_xmit_char_to_isd( char c )
//
// Sends a character to the ISD through the TTS_IO_OUT queue
// logs an error if it returns false
/////////////////////////////////////////////////////////////////
void _tts_xmit_char_to_isd( char c )
{
	if (!TTS_IO_put_OutChar( c ))
	{
		tts_control.TtsErrors_isd_BF++;	
	}
	
} 

/////////////////////////////////////////////////////////////////
// int8 _tts_proc_buffer( CmdBuffer *ptr )
// 
// Returns TRUE if any commands were processed from the
// spec'd CmdBuffer
/////////////////////////////////////////////////////////////////
int8 _tts_proc_buffer( int8 buf )
{
 CmdBuffer *ptr;

 	ptr = &(tts_control.CmdBuffers[ (int)(buf) ]);

 	if (( ptr->CmdCnt )  && (TTS_IO_GetOutFreeByteCnt() > 30))
 	{
  		switch( ptr->CmdBufferState )
  		{
   			case TTS_TASK_WAIT:
    			// If the command is clippable and is speech
   				if ((ptr->CmdIsClippable) && !(ptr->CmdInProc & 0x80))
   				{
     				// Tell speech module to clip current phrase/word
     				TtsSendStopMsg( ptr->CmdBufferPriority );
     				// But the STOP command cannot be clippable
     				ptr->CmdIsClippable = 0;
    			}
    			break;

   			case TTS_TASK_PROC_QUE:
    			// Process the next command
    			ptr->CmdBufferState = _tts_proc_next_cmd(ptr);
    			// 
    			return( TRUE );

   			default:
    			break;
  		}
 	}
 	if ( ptr->CmdCnt )
   		return( TRUE );
 	else 
   		return ( FALSE );
}

/////////////////////////////////////////////////////////////////
// void ProcTtsPupEvt()
// 
// Called when a PUP (power-up) event is received unexpectedly
// from the TTS module.
// 
/////////////////////////////////////////////////////////////////
void ProcTtsPupEvt()
{
 	tts_control.CmdBuffers[0].CmdBufferState = TTS_TASK_PROC_QUE;
 	tts_control.CmdBuffers[1].CmdBufferState = TTS_TASK_PROC_QUE;
 	tts_control.CmdBuffers[0].CmdInProc = 0;
 	tts_control.CmdBuffers[1].CmdInProc = 0;
 	tts_control.TtsBufferPlayingMsg = 0xFF;
}

/////////////////////////////////////////////////////////////////
// Void _tts_proc_event( )
//
// Called when signalled by the ISD task using SGNL_ISD_BIT.
//
// ISD_Task() will fill the byte array TTS_IO_InCtrlBlk.Buf[] with the
// TTS module msg and char *TTS_IO_InCtrlBlk.Tokens[] will point to the tokens in
// TTS_IO_InCtrlBlk.Buf[]. TTS_IO_InCtrlBlk.TokenCnt will have the token count.
//
// This function processes a TTS module event. It only looks at
// the first token and sets NmeaTaskState to a new value based on
// the first token.
/////////////////////////////////////////////////////////////////
int8 _tts_proc_event( int8 Signal )
{
 unsigned int i,j,channel;
 unsigned int Event;
 CmdBuffer *bptr;
 char cmdstr[5];
 int8 RtnSignal;
 
 	RtnSignal = Signal;
  
  	// Any events from the TTS
  	j = TTS_IO_GetInMsgCnt();
  	i = TTS_IO_GetInTokenCnt();
  	if (i && j)
  	{
   		// ISD may send an error msg with the channel number
   		// in which case, there would be two tokens
   		switch (i)
   		{
	   		case 2:
	   		    j=1;  // set j so that we check second token for the command
    			channel = TTS_IO_GetInToken(0)[0] & 0x0F;
    			break;
    			
	   		case 1:
	   			j=0;  // check 1st token for the command
    			channel = 0;
    			break;
	   		
	   		default:
	   			tts_control.TtsErrors_BadEvents++;
     			TTS_IO_NextInMsg();
     			RtnSignal |= SGNL_GENERIC_MASK;
     			return( RtnSignal );
	   			break;
	   		    		
   		}

   		// Converts the 3 character event string into an integer
   		for( i=0; i<MAX_TTS_EVTS; i++ )
   		{
    		// Get a command into RAM
    		strcpy( cmdstr, TTS_IO_TtsEvts[i] );
    		if ( !strcmp( ((char *)TTS_IO_GetInToken(j)), cmdstr ) )
    		{
     			break;
    		}//if
   		}//for
#ifndef TTS_FORCE_HI_PRIORITY
   		if (channel)
     		bptr = &(tts_control.CmdBuffers[ HIGH_PRIORITY_BUFFER ]);
   		else
		{
     		bptr = &(tts_control.CmdBuffers[ LOW_PRIORITY_BUFFER ]);
			k++;
		}
#else
		bptr = &(tts_control.CmdBuffers[ HIGH_PRIORITY_BUFFER ]);
#endif

   		Event = bptr->CmdWaitingForEvent;
	
   		if ( bptr->CmdBufferState == TTS_TASK_WAIT )
   		{
    		if ( i == Event)
      			i = TTS_EVT_MATCHED;

    		switch(i)
    		{
	 			case TTS_EVT_PUP: // unexpected power-up from TTS
	   				ProcTtsPupEvt();
       				RtnSignal |= SGNL_GENERIC_MASK;
	   				break;

     			case TTS_EVT_CME:
	   				tts_control.TtsErrors_CME++;

	 			case TTS_EVT_MATCHED:
	   				bptr->CmdBufferState = TTS_TASK_PROC_QUE;
	   				bptr->CmdEventTimeout = 0;

       				if (tts_control.TtsBufferPlayingMsg == bptr->CmdBufferPriority)
         				tts_control.TtsBufferPlayingMsg = 0xFF;

       				bptr->CmdInProc = 0;
       				if ( Event == TTS_EVT_PUP ) 
         				tts_control.CmdBuffers[1].CmdBufferState = TTS_TASK_PROC_QUE;
       				break;

	 			case TTS_EVT_STP:
	 			case TTS_EVT_PPC:
	   				break;

	 			case TTS_EVT_UNKNOWN:
	 			default:
	   				tts_control.TtsErrors_BadEvents++;
       				break;
    		}
    		TTS_IO_NextInMsg();
    		
   		} // if ( bptr->CmdBufferState == TTS_TASK_WAIT )
   		else
   		{
    		if ( i == TTS_EVT_PUP ) // unexpected power-up from TTS
    		{
     			ProcTtsPupEvt();
     			RtnSignal |= SGNL_GENERIC_MASK;
     			TTS_IO_NextInMsg();
    		}
   		}	
  	} // if (TTS_IO_RxTokenCnt)
  	return( RtnSignal );
}

/////////////////////////////////////////////////////////////////
// int8 _tts_unblock_buffer( unsigned int8 buf )
//
// Unblocks the spec'd buffer if BLOCKED and returns TRUE
// If not BLOCKED returns FALSE 
/////////////////////////////////////////////////////////////////
int8 _tts_unblock_buffer( int8 buf )
{
 CmdBuffer *ptr;

  	ptr = &(tts_control.CmdBuffers[ (int)(buf) ]);

  	if (ptr->CmdBufferState == TTS_TASK_BLOCKED)
  	{
   		ptr->CmdBufferState = TTS_TASK_PROC_QUE; 
   		ptr->CmdEventTimeout = 0;
   		return( TRUE );    
  	}
  	return( FALSE );
}

/////////////////////////////////////////////////////////////////
// Void ProcTtsTask( int8 signal )
//
// The TTS task gets three signals:
// SGNL_ISD_BIT: set when an event from the ISD task
// SGNL_TIMER_BIT: set when a channel's BLOCKED state is done
// SGNL_GENERIC: set when new commands arrive to transmit to ISD
//				task
/////////////////////////////////////////////////////////////////
int8 Tts_ProcTask( int8 signal )
{
#ifndef TTS_FORCE_HI_PRIORITY
 unsigned int8 i=0;
#endif
 unsigned int8 RtnSignal;

 	RtnSignal = signal;

 	// if we have an event from the ISD task
 	if ( signal & SGNL_ISD_MASK )
 	{
  		RtnSignal = _tts_proc_event( RtnSignal );
  		if (!TTS_IO_GetInMsgCnt())
  			RtnSignal &= SGNL_ISD_MASK ^ 0xFF;
  			
  		return( RtnSignal );
 	} // if ( signal & SGNL_ISD_MASK )

 	// if we have a timer signal unblocking a channel
 	if (signal & SGNL_TIMER_MASK)
 	{
  		// No one should be playing a message after we finish blocking
  		tts_control.TtsBufferPlayingMsg = 0xFF;

  		if (_tts_unblock_buffer( HIGH_PRIORITY_BUFFER ))
  		{
   			RtnSignal |= SGNL_GENERIC_MASK;
  		}
#ifndef TTS_FORCE_HI_PRIORITY
   		if (_tts_unblock_buffer( LOW_PRIORITY_BUFFER ))
   		{
     		RtnSignal |= SGNL_GENERIC_MASK;
     	}	
#endif
  		
  		RtnSignal &= SGNL_TIMER_MASK ^ 0xFF;
  		return( RtnSignal );
 	} // if (signal & SGNL_TIMER_MASK)

 	// if we have a signal usually meaning we have new commands
 	if ((signal & SGNL_GENERIC_MASK))
 	{
  		// if we didn't process any high priority commands
  		if (!_tts_proc_buffer( HIGH_PRIORITY_BUFFER ))
  		{
	  		if (!Btn_DialogInProgress() )
	  		{
#ifndef TTS_FORCE_HI_PRIORITY 
    			if (!_tts_proc_buffer( LOW_PRIORITY_BUFFER ))
    			{
     				RtnSignal &= SGNL_GENERIC_MASK ^ 0xFF;
    			}
				else
				{
					i++;
				}
#else
				RtnSignal &= SGNL_GENERIC_MASK ^ 0xFF;
#endif
    		}	
  		}
  		return( RtnSignal );
 	} // if (signal & SGNL_GENERIC_MASK)
 	return( RtnSignal );
}
/////////////////////////////////////////////////////////////////
// int8 Tts_IsMsgPlaying()
//
// Returns TRUE when any channel is playing a voice msg in the 
// ISD speech task. But only if it's not clippable.
/////////////////////////////////////////////////////////////////
int8 Tts_IsMsgPlaying()
{
 unsigned int8 i,j;

 	i = tts_control.TtsBufferPlayingMsg;
 
 	// if some channel is playing a message
 	if (i != 0xFF)
 	{
	 	if (i < MAX_CMD_BUFFERS)
	 	{
  			// but if it's clippable
  			j = tts_control.CmdBuffers[(int)i].CmdIsClippable;
  			if (j == 0)
   				return( TRUE );
   		}		
 	}
 	return( FALSE );
}

/////////////////////////////////////////////////////////////////
// int8 IsTtsBlocked()
//
// Returns TRUE when the spec'd channel is BLOCKED
/////////////////////////////////////////////////////////////////
int8 IsTtsBlocked( CmdBuffer *ptr )
{
 	switch( ptr->CmdBufferState )
 	{
  		case TTS_TASK_BLOCKED:
 			return( 1 );
  		case TTS_TASK_WAIT:  
  		default:
			return( 0 );
 	}
}

/////////////////////////////////////////////////////////////////
// int8 GetFreeCmdBytes(CmdBuffer *ptr)
//
// Returns the number of bytes available for commands
// in the spec'd buffer
/////////////////////////////////////////////////////////////////
int8 GetFreeCmdBytes(CmdBuffer *ptr)
{
 	return( TTS_CMD_BUFSIZE - ptr->CmdByteCnt );
}

/////////////////////////////////////////////////////////////////
// int8 GetNextCmdByte(CmdBuffer *ptr)
//
// Returns the next available byte in
// the spec'd buffer
/////////////////////////////////////////////////////////////////
int8 GetNextCmdByte(CmdBuffer *ptr)
{
 int8 cmd = 0;

 	if ( ptr->CmdByteCnt )
 	{
  		ptr->CmdByteCnt--;
  		cmd = ptr->CmdBuf[ ptr->CmdRdIdx ];
  		ptr->CmdRdIdx++;	// compiler bug

  		if ( ptr->CmdRdIdx > (TTS_CMD_BUFSIZE-1))
   			ptr->CmdRdIdx = 0;
 	}

 	return( cmd );
}

/////////////////////////////////////////////////////////////////
// int8 UnGetNextCmdByte(CmdBuffer *ptr, int8 CmdByte)
//
// Un-gets CmdByte from the
// the spec'd buffer
// The next GetNextCmdByte(CmdBuffer *ptr) from this buffer
// will return CmdByte.
/////////////////////////////////////////////////////////////////
void UnGetNextCmdByte(CmdBuffer *ptr, int8 CmdByte)
{
 	if ( ptr->CmdRdIdx == 0)
   		ptr->CmdRdIdx = TTS_CMD_BUFSIZE;

 	ptr->CmdByteCnt++;
 	--(ptr->CmdRdIdx);
 		ptr->CmdBuf[ ptr->CmdRdIdx ] = CmdByte;
}

/////////////////////////////////////////////////////////////////
// int8 TTS_GetNumberIndex( int8 AsciiNum, int8 NumRange )
//
// Returns a TTS module msg index of a number
// AsciiNum should be 0-9, NumRange should be
// RANGE_0_9: for numbers 0-9
// RANGE_10_19: for numbers 10-19
// RANGE_20_90: for numbers 20,30,...90
// RANGE_100_900: for numbers 100,200,300,...900
/////////////////////////////////////////////////////////////////
int8 TTS_GetNumberIndex( uint8 AsciiNum, uint8 NumRange )
{
 unsigned int i;
	
	AsciiNum = AsciiNum & 0x0F;
    i = (unsigned int)(AsciiNum);

	switch( NumRange )
	{
	 case RANGE_0_9:
	  	return(TtsAddrIndex0_9[ i ]);
		break;
	 case RANGE_10_19:
	  	return( TtsAddrIndex10_19[ i ] );
		break;
	 case RANGE_20_90:
	  	return( TtsAddrIndex00_90[ i ] );
		break;
	 case RANGE_100_900:
	  	return( TtsAddrIndex00_900[ i ] );
		break;
	 default:
	 	tts_control.TtsErrors_CodeErrs++;
		return( TTS_ADDR_ERROR_INDEX );
		break;
    }
}

/////////////////////////////////////////////////////////////////
// void PutTtsCmd( uint8 CmdIdx, uint8 DelimFlag )
//
// Puts a three character command string in the 
// ISD Tx buffer. Optionally, follows it with a
// delimiter byte.
/////////////////////////////////////////////////////////////////
void PutTtsCmd( uint8 CmdIdx, uint8 DelimFlag )
{
 char cmdstr[MAX_TTS_CMD_LEN+1];

	if (CmdIdx < MAX_TTS_CMDS)
	{
  		strncpy( cmdstr, TtsCmds[ (unsigned int)CmdIdx ], MAX_TTS_CMD_LEN );
  		TTS_IO_put_OutBuf( cmdstr );
		if ( DelimFlag )
	  		_tts_xmit_char_to_isd( TTS_IO_PROTOCOL_DELIM );
	}
	else
	{
		tts_control.TtsErrors_CodeErrs++;
	}  
}

/////////////////////////////////////////////////////////////////
// void PutTtsSpchMsg( uint8 MsgIdx, uint8 DelimFlag )
//
// Puts a three digit acsii number string in the 
// ISD Tx buffer. This number is a msg address 
// in the TTS module.  
// Optionally, follows it with a delimiter byte.
/////////////////////////////////////////////////////////////////
void PutTtsSpchMsg( uint8 MsgIdx, uint8 DelimFlag )
{
 char cmdstr[4];

	if (MsgIdx <= TTS_ADDR_MAX_INDEX)
	{
		if ( DelimFlag & PREPEND_DELIMITER )
	  		_tts_xmit_char_to_isd( TTS_IO_PROTOCOL_DELIM );

    	int8_to_ascii10( MsgIdx, cmdstr );
    	TTS_IO_put_OutBuf( cmdstr );

		if ( DelimFlag & APPEND_DELIMITER )
	  		_tts_xmit_char_to_isd( TTS_IO_PROTOCOL_DELIM );
	}
	else
	{
		tts_control.TtsErrors_CodeErrs++;
	}  
}

/////////////////////////////////////////////////////////////////
// uint8 GetNextCmd( CmdBuffer *ptr )
//
// Gets the next command from the spec'd buffer
// Returns 0 if it can't find it or there are no bytes
// The CmdDiscardCnt member of the buffer structure
// holds a number of commands to discard before
// returning a command. CmdDiscardCnt gets set in
// TtsQueSpeech() when the (option & TTS_OPT_FORCE_MASK)
// is TRUE.
/////////////////////////////////////////////////////////////////
uint8 GetNextCmd( CmdBuffer *ptr )
{
 uint8 i;

 	if ( ptr->CmdCnt > 0 )
 	{
  		// if we have commands to discard
  		while ( ptr->CmdDiscardCnt > 0 )
  		{
   			// find the 'start of cmd' char
   			while ( ((uint8)GetNextCmdByte( ptr )) != TTS_CMD_PROTOCOL_START )
   			{ 
    			// If the read ptr equals the write pointer, leave
    			// This shouldn't happen
    			if ( ptr->CmdRdIdx == ptr->CmdWtIdx )
    			{ 
	    			tts_control.TtsErrors_CodeErrs++;
      				ptr->CmdDiscardCnt = 0;
      				ptr->CmdCnt = 0;
      				return( 0 );
    			}
   			} //while 
   			ptr->CmdDiscardCnt--;
   			ptr->CmdCnt--;
  		}
 
   		// find the 'start of cmd' char
   		while ( ((uint8)GetNextCmdByte( ptr )) != TTS_CMD_PROTOCOL_START )
   		{ 
    		// If the read ptr equals the write pointer, leave
    		// This shouldn't happen
    		if ( ptr->CmdRdIdx == ptr->CmdWtIdx ) 
    		{
	    		tts_control.TtsErrors_CodeErrs++;
      			ptr->CmdDiscardCnt = 0;
      			ptr->CmdCnt = 0;
      			return ( 0 );
      		}		
   		}

  		// get the cmd byte
  		i = GetNextCmdByte(ptr);
  		ptr->CmdInProc = i;
  		return( i );
 	}
 	else // no commands
  		return( 0 );
} 

/////////////////////////////////////////////////////////////////
// int8 _tts_proc_next_cmd()
//
// Processes the next command in the spec'd buffer
// There are two types; speech & admin
// The admin commands set volumes, tone periods
// and block the buffer for a period.
// The speech commands simply construct phrases 
// for the speech module to say.
/////////////////////////////////////////////////////////////////
int8 _tts_proc_next_cmd( CmdBuffer *ptr )
{
 
 unsigned int8 z,i;
 uint8 RtnState = TTS_TASK_PROC_QUE;
  
  	i = GetNextCmd( ptr );

	// If there are no commands, leave.
   	if (i == 0) 
   	{
    	return( RtnState );
   	}

   	if (i == TTS_CMD_BLOCK)
   	{
    	// get the schedule queue to wait in
    	z = GetNextCmdByte(ptr);

		switch( (z & 0x70) >> 4 )
 		{
			default:
  			case 0:	// wait in 1 ms queue
				//Put1MsSchedRtn( PROC_TTS_TASK, (z & 0x0F) );
                Core_SchedTask(_1MS_QUE, PROC_TTS_TASK, (z & 0x0F), SGNL_TIMER_MASK, 0);
				ptr->CmdEventTimeout = TTS_BLK_1MS_TIMOUT;
          		break;
 			case 1:	// wait in 10 ms queue
				//Put10MsSchedRtn( PROC_TTS_TASK, (z & 0x0F) );
                Core_SchedTask(_10MS_QUE, PROC_TTS_TASK, (z & 0x0F), SGNL_TIMER_MASK, 0);
				ptr->CmdEventTimeout = TTS_BLK_10MS_TIMOUT;
          		break;
			case 2:	// wait in 100 ms queue
				//Put100MsSchedRtn( PROC_TTS_TASK, (z & 0x0F) );
                Core_SchedTask(_100MS_QUE, PROC_TTS_TASK, (z & 0x0F), SGNL_TIMER_MASK, 0);
				ptr->CmdEventTimeout = TTS_BLK_100MS_TIMOUT;
          		break;
			case 3:	// wait in 1 s queue
				//Put1SecSchedRtn( PROC_TTS_TASK, (z & 0x0F) );
                Core_SchedTask(_1SEC_QUE, PROC_TTS_TASK, (z & 0x0F), SGNL_TIMER_MASK, 0);
				ptr->CmdEventTimeout = TTS_BLK_1SEC_TIMOUT;
          		break;
			case 4:	// wait in 10 s queue
				//Put10SecSchedRtn( PROC_TTS_TASK, (z & 0x0F) );
                Core_SchedTask(_10SEC_QUE, PROC_TTS_TASK, (z & 0x0F), SGNL_TIMER_MASK, 0);
				ptr->CmdEventTimeout = TTS_BLK_10SEC_TIMOUT;
          		break;
		} // switch
    	// get the period to wait
    	
		ptr->CmdCnt--;
        tts_control.TtsBufferPlayingMsg = ptr->CmdBufferPriority;
		return( TTS_TASK_BLOCKED );
   	}
   	else
   	{
    	if (i & 0x80)	// if it's a non-speech command
    	{
     		switch(i)
     		{
#ifdef USE_SPEECH_VOLUME_CMD                
	     		case TTS_CMD_SET_SPEECH_VOLUME:
	     		
	     			z = GetNextCmdByte(ptr);
	     			//Pots_UpdateSpeechLevel( z & 0x0F );
	     			
	     			if (z & 0x0F) 
					{
	     				FMT_Mute(FALSE);
						tts_control.muted = FALSE;
					}
	     			else
					{
	     				FMT_Mute(TRUE);
						tts_control.muted = TRUE;
					}	
	     			break;
#endif
      			default:
					tts_control.TtsErrors_BadCmds++;
					break;
     		} // switch

			ptr->CmdCnt--;
     		return( RtnState );
   		}
   		else // it's a speech command
   		{
     		// If the speech module of the TTS is free
     		if (tts_control.TtsBufferPlayingMsg == 0xFF)
     		{

      			_tts_xmit_char_to_isd( TTS_IO_PROTOCOL_START );
#ifndef TTS_NO_CHANNELS
      			_tts_xmit_char_to_isd( ptr->CmdBufferPriority | 0x30 );
      			_tts_xmit_char_to_isd( TTS_IO_PROTOCOL_DELIM );
#endif

	  			// Put the 'set phrase' cmd in I2C buffer
	  			PutTtsCmd( TTS_CMD_SPH, TRUE );

	  			// set the clippable flag
	  			ptr->CmdIsClippable = i & TTS_CMD_CLIP_MASK;
	  			i = i & TTS_CMD_MASK;

      			switch ( i )
      			{
	   				case TTS_SPCH_CMD_BUILD_A_PHRASE:
	   					
         				z = GetNextCmdByte(ptr); i = 0;
         				if (z != TTS_CMD_PROTOCOL_STOP )
		 				{
		  					while(i < 10)
		  					{
		   						i++;
		   						PutTtsSpchMsg( z, NO_DELIMITER ); 
		   						z = GetNextCmdByte(ptr);
		   						if (z != TTS_CMD_PROTOCOL_STOP )
		     						_tts_xmit_char_to_isd( TTS_IO_PROTOCOL_DELIM );
		   						else
		     						break;
		  					} // while
         				} // if
		 				break;
        			default:
		 				//return( TTS_TASK_PROC_QUE );
		 				break;
       			}  // end switch
       			_tts_xmit_char_to_isd( TTS_IO_PROTOCOL_STOP );
       			RtnState = TTS_TASK_WAIT;
       			ptr->CmdWaitingForEvent = TTS_EVT_PPC;
       			ptr->CmdEventTimeout = TTS_EVT_TIMEOUT;
       			tts_control.TtsBufferPlayingMsg = ptr->CmdBufferPriority;
      		}
      		else // // If the speech module of the TTS is not free
      		{
	   			UnGetNextCmdByte(ptr,i);
       			UnGetNextCmdByte(ptr,TTS_CMD_PROTOCOL_START);
       			ptr->CmdCnt++;
      		}
     		ptr->CmdCnt--;
   		} // it's a speech command
  	} // It's a TTS_CMD_BLOCKED
 	return( RtnState );
}

/////////////////////////////////////////////////////////////////
// int8 _tts_put_next_cmd_byte( int8 CmdByte, CmdBuffer *ptr )
//
// Puts CmdByte into the
// the spec'd buffer
/////////////////////////////////////////////////////////////////
void _tts_put_next_cmd_byte( int8 CmdByte, CmdBuffer *ptr )
{
 	ptr->CmdBuf[ ptr->CmdWtIdx ] = CmdByte;
 	ptr->CmdWtIdx++;

 	if ( ptr->CmdWtIdx > (TTS_CMD_BUFSIZE-1))
   		ptr->CmdWtIdx = 0;

 	ptr->CmdByteCnt++;
}

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
int8 TtsQueSpeech2( uint8 Cmd, uint8 arg0, uint16 options )
{
 CmdBuffer *ptr;

	// If we're muted
	if (tts_control.muted)
	{
		return( TRUE );
	}

#ifndef TTS_FORCE_HI_PRIORITY
 	//If it's a high priority command
 	if (options & TTS_OPT_HIGH_PRIORITY_QUEUE_MASK)
   		ptr = &(tts_control.CmdBuffers[ HIGH_PRIORITY_BUFFER ]);
 	else
   		ptr = &(tts_control.CmdBuffers[ LOW_PRIORITY_BUFFER ]);
#else
	ptr = &(tts_control.CmdBuffers[ HIGH_PRIORITY_BUFFER ]);
#endif

 	if (GetFreeCmdBytes(ptr) > (2+3))
 	{
  		_tts_put_next_cmd_byte( TTS_CMD_PROTOCOL_START, ptr );
  		_tts_put_next_cmd_byte( (Cmd & 0xBF), ptr );
  		if ((arg0 == TTS_CMD_PROTOCOL_START) || (arg0 == TTS_CMD_PROTOCOL_STOP))
  		{
   			ptr->AliasErrs++;
   			RS232_Xmt_Error_Msg( USARTA, CMD_ERR_TTS_ALIAS_ERR );
   			_tts_put_next_cmd_byte( (arg0 & 0xBF), ptr );
  		}
  		else
   			_tts_put_next_cmd_byte( (arg0), ptr );

  		_tts_put_next_cmd_byte( TTS_CMD_PROTOCOL_STOP, ptr );
  		ptr->CmdCnt++;

  		if ( !Tts_IsMsgPlaying() )
    		SignalTask( PROC_TTS_TASK, SGNL_GENERIC_BIT);
  
  		return( TRUE );
 	}
 	return(FALSE);
}

/////////////////////////////////////////////////////////////////
// int8 TtsQueSpeech( int8 Cmd, char *argptr, int16 options )
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
int8 TtsQueSpeech( int8 Cmd, char *argptr, int16 options )
{
 CmdBuffer *ptr;
 uint8 channel;
 uint8 i;

	// If we're muted
	if (tts_control.muted)
	{
		return( TRUE );
	}

#ifndef TTS_FORCE_HI_PRIORITY
 	// If it's a high priority command
 	if (options & TTS_OPT_HIGH_PRIORITY_QUEUE_MASK)
 	{
   		ptr = &(tts_control.CmdBuffers[ HIGH_PRIORITY_BUFFER ]);
   		channel = HIGH_PRIORITY_BUFFER;
 	}
 	else
 	{
   		ptr = &(tts_control.CmdBuffers[ LOW_PRIORITY_BUFFER ]);
   		channel = LOW_PRIORITY_BUFFER;
 	}
#else
	ptr = &(tts_control.CmdBuffers[ HIGH_PRIORITY_BUFFER ]);
   	channel = HIGH_PRIORITY_BUFFER;
#endif
 
 	if (options & TTS_OPT_CLIP_MASK)
 		Cmd |= TTS_CMD_CLIP_MASK;

 	i = GetFreeCmdBytes(ptr);

 	if ( i > (strlen(argptr)+3) )
 	{
  		_tts_put_next_cmd_byte( TTS_CMD_PROTOCOL_START, ptr );
  		_tts_put_next_cmd_byte( Cmd, ptr );

		// While not at the end of the arg list
  		while( ((uint8)*argptr) != 0xFF)
  		{
    		_tts_put_next_cmd_byte( *argptr, ptr );
    		argptr++;
  		}
  		_tts_put_next_cmd_byte( TTS_CMD_PROTOCOL_STOP, ptr );
  		ptr->CmdCnt++;

		// TTS_OPT_FORCE option:
		// forces a msg to the front of the queue
		// stops a msg that may be playing
		// truncates a blocking period
  		if ( options & (TTS_OPT_FORCE_MASK) )
  		{
   			if (ptr->CmdCnt > 1)
   			{
     			ptr->CmdDiscardCnt = ptr->CmdCnt-1;
   			}
   			else 
	 			ptr->CmdDiscardCnt = 0;
   
   			if (IsTtsBlocked( ptr ) && ptr->CmdIsClippable)
   			{
    			SetTaskTruncate( PROC_TTS_TASK );	// Force it out of queue
   			}
   			if (Tts_IsMsgPlaying())
   			{
    		 	ptr->CmdIsClippable = 1;	// Clip it 
   			}
  		}

		//
  		if (options & TTS_OPT_BLOCK_PERIOD_MASK)
  		{
   			TtsQueSpeech2( TTS_CMD_BLOCK, ((options & TTS_OPT_BLOCK_PERIOD_MASK) ), options);
  		}
  		SignalTask( PROC_TTS_TASK, SGNL_GENERIC_BIT);

  		return( TRUE );
 	}
 	else // Not enough room
 	{
   		tts_control.TtsErrors_BF++;
   		return( FALSE );
 	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//void _tts_load_speech_index_to_cmdstr( uint8 cmd_str_idx, uint8 index)
//
// writes to the static char array, _tts_cmd_str[] using cmd_str_idx with
// a speech index defined in index
//
/////////////////////////////////////////////////////////////////////////////////////////////////
#define TTS_CMD_STR_LEN 30
char _tts_cmd_str[TTS_CMD_STR_LEN];
int _tts_load_speech_index_to_cmdstr( uint8 cmd_str_idx, uint8 speech_index)
{
	if ((speech_index < TTS_ADDR_MAX_INDEX+1) &&
   		(cmd_str_idx < TTS_CMD_STR_LEN-1))
   		_tts_cmd_str[ cmd_str_idx++ ] = speech_index;
   	else
   		Core_CodeError( TTS_ADDR_INDEX_ERROR );
   	
   	return( cmd_str_idx );
}
/////////////////////////////////////////////////////////////////////////////////////////////////
// int _tts_load_speech_indices_to_cmd_str(uint8 cmd_str_idx, uint32 indices_mask)
//
// Load the static char array, _tts_cmd_str[] using cmd_str_idx with
// up to 4 speech indices defined in indices_mask
//
// returns updated cmd_str_idx
//
/////////////////////////////////////////////////////////////////////////////////////////////////
int _tts_load_speech_indices_to_cmd_str(uint8 cmd_str_idx, uint32 indices_mask)
{
 int i;
 uint8 speech_index;
 uint32 mask32;
 
	for (i=0, mask32 = 0xFF000000; i < 4; i++)
 	{
  		if (indices_mask & mask32)
  		{
   			speech_index = (char) ((indices_mask >> 24) & 0x000000FF);
   			cmd_str_idx = _tts_load_speech_index_to_cmdstr(cmd_str_idx, speech_index);
  		}
  		indices_mask = indices_mask << 8;
 	}
 	return( cmd_str_idx );
}
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

uint8 TtsSaySomething( unsigned int16 p0, unsigned int16 p1, unsigned int16 p2, unsigned int16 p3, int16 options)
{
 uint8 result = 1;
 int index = 0;

	if (!Btn_IsSpchMuted())
	{
 		index = _tts_load_speech_indices_to_cmd_str( index, (uint32)(((uint32)p0<<16)|p1));
 		index = _tts_load_speech_indices_to_cmd_str( index, (uint32)(((uint32)p2<<16)|p3));
 
		if (index < TTS_CMD_STR_LEN)
		{
 			_tts_cmd_str[index] = 0xFF;
 			result = TtsQueSpeech( TTS_SPCH_CMD_BUILD_A_PHRASE, _tts_cmd_str, options );
 		}	
 		else
 			Core_CodeError( TTS_ADDR_INDEX_ERROR );
	}
    return(result);
}

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
uint8 TtsSayNumber( int32 pre_number_indices, char *num_ptr, int32 post_number_indices, int16 options)
{
 uint8 result = 1;
 int index=0, i;
 
 int8 say_all=0, int_length, dec_length;
 
 char *cmdstr,*dec_str;

 	cmdstr = _tts_cmd_str;

 	if (options & TTS_OPT_EXPL_DIGIT_MASK )
		say_all = 1;

	index = _tts_load_speech_indices_to_cmd_str( index, (uint32)(pre_number_indices));

	if (num_ptr)	// if there's a number
 	{
 		// Check for a decimal point
 		dec_str = strchr( num_ptr, '.');
 		if (dec_str != NULL)
 		*dec_str = 0;	// Terminate integer string
 		dec_str++;		// point at decimal
 		
  		int_length = strlen( num_ptr );		// set the starting index
  		if (int_length > 3) int_length = 3;
  		i = 0;

 		// Check for a minus sign
 		if (num_ptr[0] == '-')
 		{
	 		index = _tts_load_speech_index_to_cmdstr( index, TTS_ADDR_MINUS_INDEX);
  			i++; int_length--;
 		}

		// This while handles the integers
  		while ( int_length > 0 )
  		{
   			switch( int_length )
   			{
	   			default:
    			case 3:
 	 				// Do the hundreds digit if not zero
 	 				if (( num_ptr[i] != '0' ) || say_all)
 	 				{
	 	 				index = _tts_load_speech_index_to_cmdstr( index, TTS_GetNumberIndex( num_ptr[i], RANGE_0_9 ) );
 	 				}
     			
     				break;
    			case 2:
 	 				// Do the tens digit as a 'teen' if the tens digit is 1
 	 				if (num_ptr[i] == '1')
     				{
	     				index = _tts_load_speech_index_to_cmdstr( index, TTS_GetNumberIndex( num_ptr[i+1], RANGE_10_19 ) );
	  					i++; int_length--;
     				}
 	 				else // non-teen tens
 	 				{
      					if (num_ptr[i] != '0')       
	  					{
  	   						// Do the tens digit as a non-teen
  	   						index = _tts_load_speech_index_to_cmdstr( index, TTS_GetNumberIndex( num_ptr[i], RANGE_20_90 ) );
      					}
      					else // tens is zero
      					{
        					// if there was a hundreds digit
        					if (( i && (num_ptr[i-1] != '0')) || say_all) // that was not zero
        					{
	       						index = _tts_load_speech_index_to_cmdstr( index, TTS_GetNumberIndex( num_ptr[i], RANGE_20_90 ) );
        					}
      					}
 	 				}
     				break;
     				
    			case 1: // ones digit
     				if (i)
     				{
      					if (( num_ptr[i] != '0') || (num_ptr[i-1] == '0'))
      					{
	     						index = _tts_load_speech_index_to_cmdstr( index, TTS_GetNumberIndex( num_ptr[i], RANGE_0_9 ) );
       					}	
     				}
	 				else
	 					index = _tts_load_speech_index_to_cmdstr( index, TTS_GetNumberIndex( num_ptr[i], RANGE_0_9 ) );
    				
	 				break;
	     		
   			} // switch
   			i++; int_length--;
   			
  		} // while

		dec_length = strlen( dec_str );		// set the starting index
		if (dec_length > 2) dec_length = 2;
		i = 0;
		
		// This while handles the decimals
		if ( dec_length )
		{
			index = _tts_load_speech_index_to_cmdstr( index, TTS_ADDR_POINT_INDEX );
		}
		
  		while (( dec_length > 0 ) && (i < 2))
  		{	 				
	 		index = _tts_load_speech_index_to_cmdstr( index, TTS_GetNumberIndex( dec_str[i], RANGE_0_9 ) );	
    		dec_length--; i++;
  		} // while


 	} // if (num_ptr)	

	index = _tts_load_speech_indices_to_cmd_str( index, (uint32)(post_number_indices));
  
  	if (index < TTS_CMD_STR_LEN)
	{
 		cmdstr[index] = 0xFF;
 		
		result = TtsQueSpeech( TTS_SPCH_CMD_BUILD_A_PHRASE, cmdstr, options );
 	}	
 	else
 		tts_control.TtsErrors_CodeErrs++;
    
    return( result );
}

/////////////////////////////////////////////////////////////////////////////////
// int8 TtsCharToIndex( char c )
//
// Returns a speech index for the number or letter
/////////////////////////////////////////////////////////////////////////////////
int8 TtsCharToIndex( char c )
{
 int8 i, rval;
 int j;

 	rval = TTS_ADDR_SPACE_INDEX ;
 	i = (c&0xF0) >> 4;	//Get the upper nybble

 	switch (i)
 	{
  		case 3:	// possible number
			j = c & 0x0F;		// Get the lower bits
			if ((c & 0x0F) < 0x0A)
    		{
	 			rval = TtsAddrIndexA_Z[ j ] ;
    		}
   			break;
  		case 4:	// possible upper case
  		case 5:
  		case 6:	// possible lower case
  		case 7:
			if (((c & 0x1F) < 0x1B) && ((c & 0x1F) > 0))
    		{
	 			c = c - 1;
	 			rval = TtsAddrIndexA_Z[ c & 0x1F ] ;
    		}
 			break;
  		default:
			break;
 	}
 	return(rval);
}

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
void TtsSpellString ( uint32 pre_index, char *ptr, uint32 post_index, int16 options)
{
 char *cmdstr;
 int index=0,i;

	cmdstr = _tts_cmd_str;
	
	// Used to optionally say "Heading", "Wind", etc.
	index = _tts_load_speech_indices_to_cmd_str( index, pre_index);
 

 	for (i=0; (i<strlen(ptr)) && (i<13); i++)
 	{
	 	index = _tts_load_speech_index_to_cmdstr( index, TtsCharToIndex(ptr[i]) );
 	}

	// Used to optionally say "Port"/"Starboard", "Waypoint", etc.
	index = _tts_load_speech_indices_to_cmd_str( index, post_index);
  
   	if (index < TTS_CMD_STR_LEN)
	{
 		cmdstr[index] = 0xFF;
 		TtsQueSpeech( TTS_SPCH_CMD_BUILD_A_PHRASE, cmdstr, options );
 	}	
 	else
 		tts_control.TtsErrors_CodeErrs++;
}

/////////////////////////////////////////////////////////////////
// void TtsSendStopMsg( uint8 channel )
//
// Sends a STOP command to the TTS module. This will terminate
// any speech msg being played.
/////////////////////////////////////////////////////////////////
void TtsSendStopMsg( int8 channel )
{
 char cmdstr[MAX_TTS_CMD_LEN+1];

	if (channel < MAX_CMD_BUFFERS)
	{
 		_tts_xmit_char_to_isd( TTS_IO_PROTOCOL_START );
#ifndef TTS_NO_CHANNELS
		_tts_xmit_char_to_isd( channel | 0x30 );
		_tts_xmit_char_to_isd( TTS_IO_PROTOCOL_DELIM );
#endif
  		strncpy( cmdstr, TtsCmds[ TTS_CMD_STO ], MAX_TTS_CMD_LEN );
    	TTS_IO_put_OutBuf( cmdstr );
    	_tts_xmit_char_to_isd( TTS_IO_PROTOCOL_STOP );
    	tts_control.CmdBuffers[ (int)(channel) ].CmdWaitingForEvent = TTS_EVT_STP;
		tts_control.CmdBuffers[ (int)(channel) ].CmdEventTimeout = TTS_EVT_TIMEOUT; 
	}
	else
		tts_control.TtsErrors_CodeErrs++;
}

/////////////////////////////////////////////////////////////////
// void TTS_SayVersion()
//
// Announces ADSC and its version
//  
/////////////////////////////////////////////////////////////////
void TTS_SayVersion()
{
 char *str;
 
 	// Make sure volume is on for this
 	//Pots_UpdateSpeechLevel( VOICE_VOL_MED );
 	str = Core_GetVersion();

 	TtsSayNumber( (TTS_ADDR_ADSC_INDEX << 8) | TTS_ADDR_VERSION_INDEX, str, 0L, TTS_OPT_HIGH_PRIORITY_QUEUE_MASK | TTS_OPT_EXPL_DIGIT_MASK | TTS_POSTDELAY_600MS);
}


/////////////////////////////////////////////////////////////////////////
// char *Tts_GetStats( int8 Clear, char *buf  )
//
// Returns a report of errors for the ISD.
// Stats get cleared if Clear is non-zero.
// The report is composed of eight numbers coverted to
// ascii separated by commas.
// int8 TtsErrors_WD;
// int8 TtsErrors_CME;
// int8 TtsErrors_BF;
// int8 TtsErrors_BadEvents;
// int8 TtsErrors_BadCmds;
//
//  int8 CmdBuffers[ MAX_CMD_BUFFERS ].CmdErrsUnAcked;
//
// char *Core_IntToAsciiStr( int16 i, char *str, int8 min_digs )
/////////////////////////////////////////////////////////////////////////
char *Tts_GetStats( int8 Clear, char *buf )
{
 struct tts_control_str *p;

	p = &tts_control;

	_stat_append( (unsigned long)p->TtsErrors_WD, buf, TRUE,3 );		//appends 4 chars
	_stat_append( (unsigned long)p->TtsErrors_CME, buf, TRUE,3  );
	_stat_append( (unsigned long)p->TtsErrors_BF, buf, TRUE,3  );
	_stat_append( (unsigned long)p->TtsErrors_BadEvents, buf, TRUE,3  );
	_stat_append( (unsigned long)p->TtsErrors_BadCmds, buf, TRUE,3  );
#ifndef TTS_FORCE_HI_PRIORITY
	_stat_append( (int16)p->CmdBuffers[LOW_PRIORITY_BUFFER].CmdErrsUnAcked, buf, TRUE,3  );
#endif
	_stat_append( (unsigned long)p->CmdBuffers[HIGH_PRIORITY_BUFFER].CmdErrsUnAcked, buf, TRUE,3 );
	
	_stat_append( (unsigned long)p->TtsErrors_CodeErrs, buf, TRUE,3  );
	_stat_append( (unsigned long)p->TtsErrors_isd_BF, buf, FALSE,3  );

	if (Clear)
	{
		p->TtsErrors_WD = 0;
		p->TtsErrors_CME = 0;
		p->TtsErrors_BF = 0;
		p->TtsErrors_BadEvents = 0;
		p->TtsErrors_BadCmds = 0;
#ifndef TTS_FORCE_HI_PRIORITY
		p->CmdBuffers[LOW_PRIORITY_BUFFER].CmdErrsUnAcked = 0;
#endif
		p->CmdBuffers[HIGH_PRIORITY_BUFFER].CmdErrsUnAcked = 0;
		p->TtsErrors_CodeErrs = 0;
		p->TtsErrors_isd_BF = 0;
	}  	
 	return( buf );
}

void Tts_UnMute()
{
	tts_control.mute_timeout = 0;
	tts_control.muted = FALSE;
	FMT_Mute(FALSE);
}

void Tts_Mute( int16 timeout )
{
	tts_control.mute_timeout = timeout;
	FMT_Mute(TRUE);
	tts_control.muted = TRUE;
}
#endif
