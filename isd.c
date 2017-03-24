/////////////////////////////////////////////////////////////////////////
////                          isd.c                              ////
////       
// The RS232 & I2C configuration msgs for the TTS module are:
// !arg0,arg1,arg2...*
// Every message starts with a '!' character and ends with a '*'
// character.
// arg0 is always a single character digit specifying the channel 
// number.
// arg1 is always a three chracter command.
// arg2,arg3,etc. are command specific options]
// 
// !SAD,XXXX*, Set ISD Message Address, XXXX is the address
// !SPH,[,#,[0-9]]MSG_X,MSG_Y,MSG_Z,...*, Set ISD Message Phrase, MSG_X,MSG_Y,MSG_Z is the address
//		configured phrase may use up to 10 message indices
//  User may also define indexed phrases using "!SPH,#,[0-9],MSG_X,MSG_Y,MSG_Z,...*
// !PHR[,[0-9]]*, Start Playing Phrase msg, use previously configured phrase, the code 
//      sends a !PLC[,[0-9]]* msg when played phrase finishes
// User may also play previously defined indexed phrases using "!PHR,#,[0-9]*"
// !REC* or !REC,XXXX*, STart Recording, use previously configured (SAD) address if no arg2 or use
//		address spec'd in arg2.
// !PLA* or !PLA,MSG_X*, Start PLaying msg, use previously configued address if no arg2 or use
//		msg index spec'd in arg2. The code sends
//		a !PLC* msg (described below) when the played msg finishes
// !STO*, StoP Recording/Play. 
// !PUP*, Power up the Isd4004
// !PDN*, Power down the Isd4004
// !RST*, Reset the Isd4004
// !VUP,[0-3],[1-32]*, Volume Up, 0-left spkr, 1-right spkr, 2-lo tone
//				3-hi tone, 2nd arg is the step count
// !VDN,[0-3],[1-32]*, Volume down, 0-left spkr, 1-right spkr, 2-lo tone
//				3-hi tone, 2nd arg is the step count
// !VOL,[0-3],[00-31]*, Sets the volume for the spec'd pot. 1st arg is the
//				pot #, 0-left spkr, 1-right spkr, 2-lo tone, 3-hi tone. 2nd
//				arg is volume setting.
// !TPD,[001-255]*, Sets the hi tone frequency period, lo tone frequency
//				period is 2x of hi tone. The period will be 
//				(arg1_value+1)*4*128*(1/(12 mHz)). So for an arg1_value=255,
//				the period would be 10.9 ms for a frequency of 91 hz. An Arg1_value
//				of 8 gives a period of 0.341 Ms for a frequency of 2929 Hz.
// !TEF,[0-4],[0-4],[0-255],[0-255],[0-255]*, Sets up the Tone Element (TE) function. 
//				(1st arg): TE 0-left spkr vol, 1-right spkr vol, 2-lo tone vol, 
//				3-hi tone vol, 4-tone frequency period
//				(2nd arg): TE Function, 0-None, 1-Sine, 2-Square, 3-Ramp_dn, 4 ramp_up
//				(3rd arg): function period. range 0-255 (* 10ms)
//				(4th arg): function amplitude. range 0-31 for types 0-3. Range 0-255 for type 4.
//				(5th arg): Duty Cycle for square function. Mute period for ramp_up and ramp_dn
//				range 0-255 (* 10ms).
// !VBL,[0-31],[0-31]*, Sets the speaker volume and balance.
//				(1st arg):	Volume
//				(2nd arg):	Balance
//
// The response msgs are:
// !CMR*, Command Acknowledged, tells the PC that the last cmd was OK.
// !PLC*, msg play completed
// !PPC*, phrase play completed
// !RCC,XXXX*, msg record completed at address XXXX
// !CME,YYY*, Command Error, Specifies an error condition
// 		YYY: 000, no address specified
//                                                          ////
//
/////////////////////////////////////////////////////////////////////////
#define ISD

#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include <p24FJ128GB204.h>
#include <string.h>
#include "Core_api.h"
#include "rs232_api.h"
#include "Isd_data_defs.h"
#include "tts_api.h"
#include "Isd_api.h"
#include "Isd_utils.h"
//#include "pots_api.h"
#include "I2C_api.h"
#include "fmt_api.h"
#include "button_api.h"


///////////////////////////////////////////////////////////////
// Initialize the ISD
//
// We must send a POWERUP command to the ISD4004 and then wait
// at least 25 ms for the 8 KHz version. So, we'll send the 
// POWERUP cmd and then schedule the IsdTask to run in 40 ms.
// But before we do we'll set the IsdControl.IsdState flag to ISD_STT_RESET
// ( which will disable any other command processing ) and set 
// the IsdControl.IsdSerCmd
// to SER_POWERUP_DONE so that when it runs it will set the
// IsdControl.IsdState flag allowing normal processing.
////////////////////////////////////////////////////////////////
void Isd_Init()
{
 IsdControlType *ptr;
 PhraseCtlType *phtr;
 int i;
 
 	IsdTrace_Init();
 
  	ptr = &IsdControl;

  	ptr->IsdAddress = 0xFFFF;
  	ptr->IsdState = ISD_STT_RESET;
  	ptr->IsdSerCmd = 0;
  	ptr->IsdTmrEvt = 0;
  	ptr->IsdIntEvt = 0;
  	ptr->IsdEvtCollision = 0;
	ptr->IsdMsgTimeout = TTS_TIMEOUT;
	ptr->IsdErrMsgTimeout = 0;
	ptr->IsdRacTimeout = 0;
	ptr->IsdBadAddress = 0;
  	ptr->IsdSerCmdBadStt = 0;
	ptr->IsdBadRacs = 0;
  	ptr->IsdBadSignals = 0;
  	ptr->IsdBadMsgs = 0;
  	ptr->IsdCmdSrc = 0; 
  	ptr->IsdSerChannel = 0;
  	ptr->IsdSerRxTokenCnt = 0;
  	ptr->IsdRacWd = SER_RACWD_INIT;
  	ptr->IsdLastRac = 0;
  
  
  	for( i=0; i<MAX_PHRASE_INDEX; i++ )
  	{
   		phtr = &IsdControl.PhraseCtl[i];
   		phtr->IsdPhraseIdx=0;
   		phtr->IsdPhraseCnt=0;
  	}
  	ptr->CurrentPhraseIndex = 0;

  	SPI_SS = 1;
  	SPI_CLK = 0;
  	// Send the powerup command to the ISD
  	ISD_SPI_transact8( SPI_POWERUP );

  	// Schedule isdtask to run in 40 ms and set the powerup flag
  	ptr->IsdState = ISD_STT_RESET; 
  	// Next command is powerup
  	ptr->IsdTmrEvt = ISD_CMD_PUP;
  	// Command source is us
  	ptr->IsdCmdSrc = ISD_CMD_ISD_RCV;
  	// Come back in 40-50 ms
  	//Put10MsSchedRtn( PROC_ISD_TASK, 5 );
    Core_SchedTask(_10MS_QUE, (PROC_ISD_TASK), 5, SGNL_TIMER_MASK, 0);

  	// Schedule the auto-re-scheduling 20 ms RAC task
  	//Put10MsSchedRtn( PROC_ISDRAC_TASK | 0x80, 2 );
    Core_SchedTask(_10MS_QUE, (PROC_ISDRAC_TASK | 0x80), 2, SGNL_TIMER_MASK, 2);
  	// External interrupt on falling edge
  	_INT0EP = 1;
  	// Enable INT0 interrupt
  	_INT0IE = 1; 
}

///////////////////////////////////////////////////////////////
// void IsIsdReady( uint8 Owner )
//
// This function checks to see if the ISD is locked and
// if it is not then it locks it and returns TRUE. Otherwise, 
// it returns false 
///////////////////////////////////////////////////////////////
int IsIsdReady( uint8 Owner )
{
 	if ( RsrcSetLockOwner( RSRC_ISD, Owner ) ) 
 		return (TRUE);
 	else
 	{
		return( FALSE );
 	}
}

int Isd_UsartAClientReady() { return(IsIsdReady(SGNL_USARTA_MASK));}
int Isd_UsartBClientReady() { return(IsIsdReady(SGNL_USARTB_MASK));}
int Isd_UsartUsbClientReady() { return(IsIsdReady(SGNL_USART_USB_MASK));}

void Isd_Xmt_Error_Msg( int8 CmdSrc, int8 Channel, int8 ErrMsg )
{
 char evtstr[30];
 int i=0;
 
 	// Get a command into RAM
#ifndef TTS_NO_CHANNELS
 	evtstr[i++] = Channel | 0x30;
 	evtstr[i++] = PROTOCOL_DELIM;
#endif
 	strcpy( evtstr+(i), SerEvts[ ISD_EVT_CME_TKN ]);
 	i += 3;
 	evtstr[i++] = PROTOCOL_DELIM;
 	int8_to_ascii10( ErrMsg, evtstr+i );
 	Isd_Xmt_Msg( CmdSrc, evtstr );
}

///////////////////////////////////////////////////////////////
// int8 chk_isd_cmd( int8 State, int8 Cmd )
//
// Called internally from the ISD task to check whether
// a new command is compatible with the task state.
//
// This function uses the structured array IsdCmdMinState[]
// which is a lookup table.
///////////////////////////////////////////////////////////////
int8 chk_isd_cmd( int8 State, int Cmd )
{
	if ((Cmd & 0x7F) < MAX_ISD_CMD_MIN_STATE)
	{
 		switch( IsdCmdMinState[ Cmd & 0x7F ].MinStateOptions )
 		{
  			default:
  			case MIN_STT_OPT_EQ:
				if (State == IsdCmdMinState[ Cmd & 0x7F ].MinState) 
					return( TRUE );
				break;
			
  			case MIN_STT_OPT_GT:
    			if (State > IsdCmdMinState[ Cmd & 0x7F ].MinState) 
    				return( TRUE );
    			break;
    			
  			case MIN_STT_OPT_GTE:
    			if (State >= IsdCmdMinState[ Cmd & 0x7F ].MinState) 
    				return( TRUE );
    			break;
    		
  			case MIN_STT_OPT_LT:
    			if (State < IsdCmdMinState[ Cmd & 0x7F ].MinState) 
    				return( TRUE );
    			break;
    		
  			case MIN_STT_OPT_LTE:
    			if (State <= IsdCmdMinState[ Cmd & 0x7F ].MinState) 
    				return( TRUE );
    			break;
 		}
 	}	
 	return( FALSE );
}

int16 GetAddrFromCmd( int8 token )
{
 int index;
 IsdControlType *ptr;
 
 	ptr = &IsdControl;

 	index = atol( GetIsdRxTokens( ptr->IsdCmdSrc, token));
 	
 	if (index <= TTS_ADDR_MAX)
 		return( TtsAddrSpchTbl[ index ] );
 	else
 	{
	 	ptr->IsdBadMsgs++;
 		return(TTS_ADDR_ERROR);
 	}
}

void IsdSendRecEndAddress(int8, int8);
void IsdPlayPhrase( PhraseCtlType *phtr );
void IsdSendPlayMsgDone(int8, int8);
void IsdSendPlayPhraseDone(int8, int8);
void IsdSendStopDone(int8, int8);
void IsdSendCursor();

#ifndef TTS_NO_CHANNELS
#define ISD_CMD_STARTING_TKN_IDX 1
#define ISD_CMD_VOL_SIZE 4
#define ISD_CMD_VOL_AMT_IDX ISD_CMD_STARTING_TKN_IDX+2
#define ISD_CMD_SAD_SIZE 4
#define ISD_CMD_SAD_ADDR_IDX ISD_CMD_STARTING_TKN_IDX+1
#define ISD_CMD_SPH_MIN_CANNED_PHR_SIZE 5 
#define ISD_CMD_SPH_CANNED_PHR_IDX ISD_CMD_STARTING_TKN_IDX+2
#define ISD_CMD_REC_SIZE 3
#define ISD_CMD_PLA_SIZE 3
#define ISD_CMD_PLA_ADDR_IDX ISD_CMD_STARTING_TKN_IDX+1
#define ISD_CMD_PHR_SIZE 4
#else
#define ISD_CMD_STARTING_TKN_IDX 0
#define ISD_CMD_VOL_SIZE 3
#define ISD_CMD_VOL_AMT_IDX ISD_CMD_STARTING_TKN_IDX+2
#define ISD_CMD_SAD_SIZE 2
#define ISD_CMD_SAD_ADDR_IDX ISD_CMD_STARTING_TKN_IDX+1
#define ISD_CMD_SPH_MIN_CANNED_PHR_SIZE 4 
#define ISD_CMD_SPH_CANNED_PHR_IDX ISD_CMD_STARTING_TKN_IDX+2
#define ISD_CMD_REC_SIZE 2
#define ISD_CMD_PLA_SIZE 2
#define ISD_CMD_PLA_ADDR_IDX ISD_CMD_STARTING_TKN_IDX+1
#define ISD_CMD_PHR_SIZE 3
#endif

void ProcIsdCmd( int8 CmdSrc )
{
 int i,j,k;
 char *cptr;
 IsdControlType *ptr;
 PhraseCtlType *phtr;
 
 		ptr = &IsdControl;
		ptr->IsdSerRxTokenCnt = GetIsdRxTokenCnt( CmdSrc );
		ptr->IsdSerCmd = Isd_GetCmd( CmdSrc );
    	ptr->IsdSerChannel = GetIsdChannel( CmdSrc );
    	ptr->IsdSerPort = GetIsdSerPort( CmdSrc );

		ptr->IsdCmdSrc = CmdSrc;

   		switch (ptr->IsdSerCmd & 0x7F)
   		{
#ifdef USE_SPEECH_VOLUME            
	   		case  ISD_CMD_VOL:	// Set the volume
   
     			if (ptr->IsdSerRxTokenCnt != ISD_CMD_VOL_SIZE)
     			{
     	 			Isd_Xmt_Error_Msg( CmdSrc, ptr->IsdSerChannel, CMD_ERR_UNKNOWN_CMD );
     	 			break;
     			}
     			i = atol( GetIsdRxTokens( CmdSrc, ISD_CMD_VOL_AMT_IDX));
     			// SetDgtlPot( DGTL_POT_VOICE, (int8)i );
     			
     			IsdSendCmdDone( CmdSrc, ptr->IsdSerChannel );
     			RsrcFreeLock( RSRC_ISD );
     			break;
#endif     			
    		case  ISD_CMD_SAD:	// Set the address, nothing to do
   
     			if (ptr->IsdSerRxTokenCnt != ISD_CMD_SAD_SIZE)
     			{
     	 			Isd_Xmt_Error_Msg( CmdSrc, ptr->IsdSerChannel, CMD_ERR_UNKNOWN_CMD );
     	 			break;
     			}
     			
     			ptr->IsdAddress = atol( GetIsdRxTokens( CmdSrc, ISD_CMD_SAD_ADDR_IDX));
     			if (ptr->IsdAddress > ISD_MAX_ROW_ADDR)
     			{
        			ptr->IsdAddress = ISD_MAX_ROW_ADDR;
        		}		
     			IsdSendCmdDone( CmdSrc, ptr->IsdSerChannel );
     			RsrcFreeLock( RSRC_ISD );
     			break;

    		case  ISD_CMD_SPH: // Set the phrase

				//if (!Btn_IsSpchMuted())
				//	FMT_Mute(FALSE);
					
        		i = GetIsdRxTokenCnt( CmdSrc );
        		cptr = GetIsdRxTokens( CmdSrc, ISD_CMD_STARTING_TKN_IDX+1);
        			
        		// Check if we're setting a canned phrase
        		if (( cptr[0] == '#' ) && ( i > ISD_CMD_SPH_MIN_CANNED_PHR_SIZE-1 ))
        		{
         			// Get the phrase index
         			j = atol( GetIsdRxTokens( CmdSrc, ISD_CMD_SPH_CANNED_PHR_IDX));
         			if ( j > MAX_PHRASE_INDEX )
         			{
          				break;
         			}
         			i = ISD_CMD_STARTING_TKN_IDX+3;
         			k = ISD_CMD_STARTING_TKN_IDX+3;
        		}
        		else  // We set phrase #0 by default
        		{
         			j = 0;
         			i = ISD_CMD_STARTING_TKN_IDX+1;
         			k = ISD_CMD_STARTING_TKN_IDX+1;
        		}
        		phtr = &IsdControl.PhraseCtl[j];
     			for(phtr->IsdPhraseCnt=0; i<=ptr->IsdSerRxTokenCnt; i++, phtr->IsdPhraseCnt++)
     			{
         			phtr->IsdPhrase[ i-k ] = GetAddrFromCmd( i );
     			}
     			phtr->IsdPhraseCnt--;
     			ptr->IsdState = ISD_STT_PLAYPHRASE;
     			ptr->IsdRacWd = SER_RACWD_INIT*2;
				ptr->IsdMsgTimeout = TTS_TIMEOUT;
        		ptr->CurrentPhraseIndex = j;
     			IsdPlayPhrase( phtr );
     			break;

    		case  ISD_CMD_REC: // Start recording at address

     			if ( ptr->IsdSerRxTokenCnt == ISD_CMD_REC_SIZE )
     			{
         			ptr->IsdAddress = atol( GetIsdRxTokens( CmdSrc, ISD_CMD_STARTING_TKN_IDX+1));
     			}
     			
     			if (ptr->IsdAddress < ISD_MAX_ROW_ADDR)
     			{
     				ptr->IsdState = ISD_STT_REC;
     				ptr->IsdRacWd = SER_RACWD_INIT;
     				ISD_SPI_transact24( SPI_SETREC, ptr->IsdAddress );
     			}
     			break;

    		case  ISD_CMD_STO: // Stop recording

	 			switch( ptr->IsdState )
	 			{
	  	 			case ISD_STT_REC:
      	  				ISD_SPI_transact24( SPI_STOPREC, 0 );
		  				IsdSendRecEndAddress( CmdSrc, ptr->IsdSerChannel );
	      				break;
      	 			case ISD_STT_PLAYMSG:
      	 				//FMT_Mute(TRUE);
     	  				ISD_SPI_transact24( SPI_STOPPLAY, 0 );
          				IsdSendStopDone( CmdSrc, ptr->IsdSerChannel);
	      				break;
	  	 			case ISD_STT_PLAYPHRASE:
	  	 				//FMT_Mute(TRUE);
     	  				ISD_SPI_transact24( SPI_STOPPLAY, 0 );
          				IsdSendStopDone( CmdSrc, ptr->IsdSerChannel);
		  				break;
        			default: 
					case ISD_STT_READY:
          				IsdSendStopDone( CmdSrc, ptr->IsdSerChannel);
		  				break;
	  			}
        
      			ptr->IsdState = ISD_STT_READY;
      			// Tolerate 2 RAC pulses after this
      			ptr->IsdRacWd = 2;
      			// Free the lock
	  			RsrcFreeLock( RSRC_ISD );
      			break;

    		case  ISD_CMD_PLA: // Start playing msg at address spec'd in SAD

				//if (!Btn_IsSpchMuted())
				//	FMT_Mute(FALSE);
				
     			if ( ptr->IsdSerRxTokenCnt == ISD_CMD_PLA_SIZE )
     			{
         			ptr->IsdAddress = GetAddrFromCmd(ISD_CMD_PLA_ADDR_IDX);
     			}
	 			ptr->IsdState = ISD_STT_PLAYMSG;
     			ptr->IsdRacWd = SER_RACWD_INIT*2;
				ptr->IsdMsgTimeout = TTS_TIMEOUT;
     			ISD_SPI_transact24( SPI_SETPLAY, ptr->IsdAddress );
     			break;

    		case  ISD_CMD_PHR: // Start playing phrase spec'd in SPH

				//if (!Btn_IsSpchMuted())
				//	FMT_Mute(FALSE);
				
     			ptr->IsdState = ISD_STT_PLAYPHRASE;
     			ptr->IsdRacWd = SER_RACWD_INIT*2;

        		i = GetIsdRxTokenCnt(ptr->IsdCmdSrc);
        		cptr = GetIsdRxTokens( CmdSrc, ISD_CMD_STARTING_TKN_IDX+1);
        		
        		// Check for alternate format specifying phrase index
        		//$0,PHR,#,[phrase index]*
        		if (( cptr[0] == '#' ) && ( i > ISD_CMD_PHR_SIZE-1 ))
        		{
        			// Get the phrase index
         			j = atol( GetIsdRxTokens( CmdSrc, ISD_CMD_STARTING_TKN_IDX+2));
         			if ( j > MAX_PHRASE_INDEX )
         			{
          				break;
         			}
        		}
        		else
        		{
         			j = 0;
        		}
        		ptr->CurrentPhraseIndex = j;
        		phtr = &IsdControl.PhraseCtl[j];
     			IsdPlayPhrase( phtr );
     			break;

    		case  ISD_CMD_PUP: // Explicitly powerup the ISD

	 			// Send the powerup command to the ISD
  	 			ISD_SPI_transact8( SPI_POWERUP );

  	 			// Schedule isdtask to run in 80 ms and set the powerup flag
  	 			ptr->IsdState = ISD_STT_RESET; 
  	 			ptr->IsdTmrEvt = ISD_EVT_POWERUP_DONE;
  	 			//Put10MsSchedRtn( PROC_ISD_TASK, 9 );
                Core_SchedTask(_10MS_QUE, (PROC_ISD_TASK), 9, SGNL_TIMER_MASK, 0);
     	
     			break;

    		case  ISD_CMD_PDN: // Explicitly powerdown the ISD

 	 			// Send the powerdown command to the ISD
  	 			ISD_SPI_transact8( SPI_POWERDN );

  	 			// Put the ISD in the RESET State
  	 			ptr->IsdState = ISD_STT_RESET; 
        		IsdSendCmdDone( CmdSrc, RsrcGetLockOwner( RSRC_ISD ));
     			break;

    		case  ISD_CMD_RST: // Explicitly powerdown the ISD
				   // Then bring it back up
 	 			// Send the powerdown command to the ISD
  	 			ISD_SPI_transact8( SPI_POWERDN );

  	 			// Schedule isdtask to run in 80 ms and set the powerup flag
  	 			ptr->IsdState = ISD_STT_RESET; 
  	 			ptr->IsdTmrEvt = ISD_CMD_PUP;
	 
  	 			//Put10MsSchedRtn( PROC_ISD_TASK, 9 );
                Core_SchedTask(_10MS_QUE, (PROC_ISD_TASK), 9, SGNL_TIMER_MASK, 0);
     			break;
     			
     		default:
     			Isd_Xmt_Error_Msg( CmdSrc, ptr->IsdSerChannel,  ISD_ERR_BADCMD );
     			// Free the lock
	  			RsrcFreeLock( RSRC_ISD );
	  			break;
		}// switch
		
}

///////////////////////////////////////////////////////////////
// int8 Isd_ProcTask( int8 signal )
//
// This task manages the ISD4004 voice recorder chip.
// The task receives signals from the serial ports, the external
// interrupt from the ISD, and from itself.
///////////////////////////////////////////////////////////////
int8 Isd_ProcTask( int8 signal )
{
 int8 RtnSignal, CmdSrc;
 IsdControlType *ptr;
 
 	ptr = &IsdControl;

 	// We're done after this by default
 	//RtnSignal = SGNL_NO_REQUEUE;
 	RtnSignal = signal;

 	// if we have a serial command
 	if (signal & (SGNL_USARTA_MASK | SGNL_USARTB_MASK | SGNL_USART_USB_MASK | SGNL_TTS_MASK))
 	{
	  	if ( signal & SGNL_USARTA_MASK )
	  	{
		  	CmdSrc = ISD_CMD_SRC_RS232_A;
		  	RtnSignal &= (SGNL_USARTA_MASK) ^ 0xFF;
		}
		else
		if ( signal & SGNL_USARTB_MASK )
		{
		 	CmdSrc = ISD_CMD_SRC_RS232_B;
			RtnSignal &= (SGNL_USARTB_MASK) ^ 0xFF;
		}
		else
		if ( signal & SGNL_USART_USB_MASK )
		{
		 	CmdSrc = ISD_CMD_SRC_RS232_USB;
			RtnSignal &= (SGNL_USART_USB_MASK) ^ 0xFF;
		}
		else            
		if ( signal & SGNL_TTS_MASK )
		{
            CmdSrc = ISD_CMD_SRC_TTS;
			RtnSignal &= (SGNL_TTS_MASK) ^ 0xFF;
		}

		if ( RsrcGetLockOwner( RSRC_ISD ) != CmdSrc )
		{
			ptr->IsdBadSignals++;
			return( RtnSignal ); // then clear this signal
		}
		
		// if we are in the right state for the command
  		if (!chk_isd_cmd( ptr->IsdState, Isd_GetCmd( CmdSrc ) ))
  		{
	  		//Isd_Xmt_Error_Msg( CmdSrc, GetIsdChannel( CmdSrc ), ISD_ERR_BADSTT );
 	  		ptr->IsdSerCmdBadStt++;
			switch ( ptr->IsdState )
  			{
   				case ISD_STT_PLAYMSG:
	    			ptr->IsdIntEvt = ISD_EVT_PLAYMSG_DONE;
					ISD_SPI_transact24( SPI_STOPPLAY, 0 );
					SignalTask( PROC_ISD_TASK, SGNL_ISD_BIT );
        			break;
   				case ISD_STT_PLAYPHRASE:
   	     			ptr->IsdIntEvt = ISD_EVT_PLAYPHRASE_DONE;
					ISD_SPI_transact24( SPI_STOPPLAY, 0 );
					SignalTask( PROC_ISD_TASK, SGNL_ISD_BIT );
					break;
				default:
        			RsrcFreeLock( RSRC_ISD );
					break;
        		
  			}//switch
            
            _isd_free_port_buffer( CmdSrc );
   			
            return( RtnSignal ); // then clear this signal
	    }
				
		ProcIsdCmd( CmdSrc );
        
        _isd_free_port_buffer( CmdSrc );
       
		IsdTraceChK( CmdSrc );
		
		// if no more messages...
		if (!Isd_Msg_Done( CmdSrc ))
   			return( RtnSignal ); // then clear this signal
   		else
   			return( signal );    // else don't clear it
   			
  	}// if (signal & (SGNL_RS232_MASK | SGNL_I2C_MASK | SGNL_TTS_MASK))

 	// if we have a scheduled event
 	if (signal & (SGNL_TIMER_MASK ))
 	{
  		// if we in the right state for the command
  		if (chk_isd_cmd( ptr->IsdState, ptr->IsdTmrEvt ))
  		{
   			switch (ptr->IsdTmrEvt & 0x7F)
   			{
     			case  ISD_EVT_POWERUP_DONE: // Set the powerup flag to enable normal operations 
         			ptr->IsdState = ISD_STT_READY;
         			ptr->IsdLastRac = SPI_RAC;
         			_INT0IE = 1; // enable ext interrupt
         			// Free the lock
         			RsrcFreeLock( RSRC_ISD );
         			// Tell the TTS task we've powered up
         			IsdSendTtsEvent(0, SER_EVT_PUP_TKN);
         			break;

    			case  ISD_CMD_PUP: // Explicitly powerup the ISD

	 				// Send the powerup command to the ISD
  	 				ISD_SPI_transact8( SPI_POWERUP );

  	 				// Schedule isdtask to run in 80 ms and set the powerup flag
  	 				ptr->IsdState = ISD_STT_RESET; 
  	 				ptr->IsdTmrEvt = ISD_EVT_POWERUP_DONE;
  	 				//Put10MsSchedRtn( PROC_ISD_TASK, 9 );
                    Core_SchedTask(_10MS_QUE, (PROC_ISD_TASK), 9, SGNL_TIMER_MASK, 0);
 
     				break;

			}// switch
   		}// if (chk_isd_cmd( ptr->IsdState, ptr->IsdTmrEvt ))
   		RtnSignal &= (SGNL_TIMER_MASK) ^ 0xFF;
   		return( RtnSignal );
  	}// if (signal & (SGNL_TIMER_MASK))

 	// if we have an internal ISD event
 	if (signal & (SGNL_ISD_MASK ))
 	{
  		// if we in the right state for the command
  		if (chk_isd_cmd( ptr->IsdState, ptr->IsdIntEvt ))
  		{
   			switch (ptr->IsdIntEvt & 0x7F)
   			{
     			case  ISD_EVT_PLAYMSG_DONE:
     				//FMT_Mute(TRUE);
         			IsdSendPlayMsgDone( ptr->IsdCmdSrc, ptr->IsdSerChannel);
         			ptr->IsdState = ISD_STT_READY;
         			// Free the lock
   					RsrcFreeLock( RSRC_ISD );
         			break;

     			case  ISD_EVT_PLAYPHRASE_DONE:
     				//FMT_Mute(TRUE);
         			IsdSendPlayPhraseDone( ptr->IsdCmdSrc, ptr->IsdSerChannel);
         			ptr->IsdState = ISD_STT_READY;
         			// Free the lock
   					RsrcFreeLock( RSRC_ISD );         			
         			break;

     			case  ISD_EVT_OVERFLOW:
     				//FMT_Mute(TRUE);
         			Isd_Xmt_Error_Msg( ptr->IsdCmdSrc, ptr->IsdSerChannel,  ISD_ERR_OVERFLOW );
         			ptr->IsdState = ISD_STT_READY;
         			// Free the lock
   					RsrcFreeLock( RSRC_ISD );         			
         			break;

	 			case  ISD_EVT_BAD_EOM:
	 				//FMT_Mute(TRUE);
         			Isd_Xmt_Error_Msg( ptr->IsdCmdSrc, ptr->IsdSerChannel,  ISD_ERR_BADEOM );
         			ptr->IsdState = ISD_STT_READY;
         			// Free the lock
   					RsrcFreeLock( RSRC_ISD );         			
         			break;

     			case  ISD_CMD_RST: 	// Explicitly powerdown the ISD
					   				// Then bring it back up
 	 	 			// Send the powerdown command to the ISD
  	 	 			ISD_SPI_transact8( SPI_POWERDN );

  	 	 			// Schedule isdtask to run in 80 ms and set the powerup flag
  	 	 			ptr->IsdState = ISD_STT_RESET; ptr->IsdTmrEvt = ISD_CMD_PUP;
	 
  	 	 			//Put10MsSchedRtn( PROC_ISD_TASK, 9 );
                    Core_SchedTask(_10MS_QUE, (PROC_ISD_TASK), 9, SGNL_TIMER_MASK, 0);
     	 			break;

			}// switch
   		}// if (chk_isd_cmd( ptr->IsdState, ptr->IsdIntEvt ))

   		RtnSignal &= (SGNL_ISD_MASK) ^ 0xFF;

   		return( RtnSignal );
  	}// if (signal & (SGNL_ISD_MASK))
  	
	return( 0 );
}

//**************************************************************
// Sends an RCC event with the last address when recording
// was stopped
//**************************************************************  
void IsdSendRecEndAddress( int8 CmdSrc, int8 Channel )
{
 char evtstr[12];

 	// Get a command into RAM
#ifndef TTS_NO_CHANNELS
 	evtstr[0] = Channel | 0x30;
 	evtstr[1] = PROTOCOL_DELIM;
 	strcpy( &(evtstr[2]), SerEvts[ SER_EVT_RCC_TKN ] );
#else
	strcpy( &(evtstr[0]), SerEvts[ SER_EVT_RCC_TKN ] );
#endif
 	evtstr[3] = PROTOCOL_DELIM;
 	Core_IntToAsciiStr( ( long)(IsdControl.IsdSpiAddress & 0x0FFF), &(evtstr[4]), 4 );
 	Isd_Xmt_Msg( CmdSrc, evtstr );
}

//**************************************************************
// Sends an PLC (Play Message Complete) event
//**************************************************************  
void IsdSendPlayMsgDone(int8 CmdSrc, int8 Channel)
{
 	IsdSendEvent( CmdSrc, Channel, SER_EVT_PLC_TKN );
}

//**************************************************************
// Sends an PPC (Play Phrase Complete) event
//************************************************************** 
void IsdSendPlayPhraseDone( int8 CmdSrc, int8 Channel )
{
 	IsdSendEvent( CmdSrc, Channel, SER_EVT_PPC_TKN );
}

//**************************************************************
// Sends an STP (Stop Complete) event
//************************************************************** 
void IsdSendStopDone( int8 CmdSrc, int8 Channel)
{
 	IsdSendEvent( CmdSrc, Channel, SER_EVT_STP_TKN );
}

//**************************************************************
// Plays the next phrase in the phrase list phtr->IsdPhrase[]
//************************************************************** 
void IsdPlayPhrase( PhraseCtlType *phtr )
{
 	phtr->IsdPhraseIdx = 0;
 	if ( phtr->IsdPhraseIdx < phtr->IsdPhraseCnt )
	{
   		ISD_SPI_transact24( SPI_SETPLAY, phtr->IsdPhrase[ (unsigned int)phtr->IsdPhraseIdx++ ] );
		IsdControl.IsdMsgTimeout = TTS_TIMEOUT;
	}
}

//**************************************************************
// External Interrupt from the ISD4004
//
// An interrupt occurs when and End of Message (EOM) or
// a memory overflow (OVF) condition happens
//
// An EOM is expected when playing a message.
//
// An EOM generate an error message/event when not playing
// message. OVF condition will always generate an error
// message.
//************************************************************** 
void isd_INT0_Isr(void)
{
 IsdControlType *ptr;
 PhraseCtlType *phtr;
 int i;
 
 	ptr = &IsdControl;
 	i = ptr->CurrentPhraseIndex;
 	phtr = &IsdControl.PhraseCtl[i];

 	// Get the SPI Status
 	if (ptr->IsdState == ISD_STT_REC)
   		ISD_SPI_transact24( SPI_STOPREC, 0 );
 	else
   		ISD_SPI_transact24( SPI_STOPPLAY, 0 );

 	// If the EOM bit from the ISD4004 is set 
 	if (SPI_STATUS_EOM)
 	{
  		switch ( ptr->IsdState )
  		{
   			case ISD_STT_READY:
				_INT0IF = 0; // clear external int
				return;
				break;
   			case ISD_STT_REC: 
	   			ptr->IsdIntEvt = ISD_EVT_BAD_EOM;    	
        		break;
   			case ISD_STT_PLAYMSG:
	    		ptr->IsdIntEvt = ISD_EVT_PLAYMSG_DONE;
        		break;
   			case ISD_STT_PLAYPHRASE:
        		if (( phtr->IsdPhraseIdx < phtr->IsdPhraseCnt ) &&
            		( phtr->IsdPhraseIdx < SER_MAX_PHRASE_MSGS+1 ))
				{
         			ISD_SPI_transact24( SPI_SETPLAY, phtr->IsdPhrase[ (unsigned int)phtr->IsdPhraseIdx++ ] );
					ptr->IsdMsgTimeout = TTS_TIMEOUT;
		 			_INT0IF = 0; // clear external int
		 			return;
				}
	    		else
        		{
   	     			ptr->IsdIntEvt = ISD_EVT_PLAYPHRASE_DONE;
        		}
        		break;
  		}//switch
 	}//if
 	else 
 	{ 
  		if (SPI_STATUS_OVF) // assume OVERFLOW bit set
  		{
   			if ( ptr->IsdState != ISD_STT_RESET )
   			{
    			ptr->IsdIntEvt = ISD_EVT_OVERFLOW;
   			}
   			else // ptr->IsdState == ISD_STT_RESET
   			{
    			_INT0IF = 0; // clear external int
    			return;
   			}
  		}
 	}
 	SignalTask( PROC_ISD_TASK, SGNL_ISD_BIT );
 	_INT0IF = 0; // clear external int
}

////////////////////////////////////////////////////////////////
// int8 IsdRac_ProcTask( int8 signal )
//
// ISD RAC pin monitor task
// This task runs every 20 ms
// The signal bit set will be SGNL_TIMER_BIT since the signal
// comes from the scheduler.
//
// The ISD4004 has 8 minutes of storage
// The chip uses 2400 rows of memory storage
// Hence, each row has 200 ms of storage
//
// The RAC pin goes low every 200 ms for 25 ms
// Since this task runs every 20 ms, it'll catch
// every low pulse of the RAC pin.
//
// This task monitors the RAC pin to make sure the ISD4004
// is behaving as it should. When recording or playing back
// a message the RAC pin should be pulsing, and otherwise
// shouldn't be pulsing.
/////////////////////////////////////////////////////////////
int8 IsdRac_ProcTask( int8 signal )
{
 int8 RtnSignal, isd_event;
 IsdControlType *ptr;
 //PhraseCtlType *phtr;
 
 	ptr = &IsdControl;
 	//phtr = &IsdControl.PhraseCtl;
	isd_event = ISD_EVT_PLAYMSG_DONE;

	// IsdWd should never get to zero
    if ( !SPI_RAC ) 
    	ptr->IsdRacWd = SER_RACWD_INIT*2;
    else
	{
		if (ptr->IsdRacWd)
        	ptr->IsdRacWd--;
	}

 	RtnSignal = 0; // removed from run queue after this

 	switch( ptr->IsdState )
 	{       		
  		// In these states, the RAC pin should be going low every 200 ms
  		case ISD_STT_PLAYPHRASE:
			isd_event = ISD_EVT_PLAYPHRASE_DONE;

  		case ISD_STT_PLAYMSG:
			
			// Check the msg timeout here
			if (!(SPI_RAC))
			{
				// detect a negative edge, RAC is low but was previously high
				if (ptr->IsdLastRac)
				{
					if (ptr->IsdMsgTimeout)
						ptr->IsdMsgTimeout--;

					// if the message timed out
					if (!ptr->IsdMsgTimeout)
					{
						// stop the message, log an error
						ptr->IsdErrMsgTimeout++;
						ptr->IsdIntEvt = isd_event;
						ISD_SPI_transact24( SPI_STOPPLAY, 0 );
						SignalTask( PROC_ISD_TASK, SGNL_ISD_BIT );
					}
				}
			}
			else // SPI_RAC is set
			{
				// if RAC stayed HIGH too long while playing a msg or phrase
       			if ( !ptr->IsdRacWd )
       			{
					// stop the message, log an error
					ptr->IsdRacTimeout++;
					ptr->IsdIntEvt = isd_event;
					ISD_SPI_transact24( SPI_STOPPLAY, 0 );		
					SignalTask( PROC_ISD_TASK, SGNL_ISD_BIT );
       			}
			}
			break; 

		case ISD_STT_REC:

       		if ( !ptr->IsdRacWd )
       		{
        		Isd_Xmt_Error_Msg( ptr->IsdCmdSrc, ptr->IsdSerChannel,  ISD_ERR_RACWD );
       		}
       
    		break;
    		
  		default:
       		// SPI_RAC should not pulse in these states
       		// check for High to low transition
       		if ( !SPI_RAC && ( ptr->IsdLastRac ))
       		{
				//ISD_SPI_transact24( SPI_STOPPLAY, 0 );
				ptr->IsdBadRacs++;
         		//Isd_Xmt_Error_Msg( ptr->IsdCmdSrc, ptr->IsdSerChannel,  ISD_ERR_BADRAC );
         		//ptr->IsdIntEvt = ISD_CMD_RST;
         		//SignalTask( PROC_ISD_TASK, SGNL_ISD_BIT );
        	}
       		break;
 	}//switch

	// remember last RAC
    if ( !SPI_RAC ) 
    	ptr->IsdLastRac = 0;
    else
		ptr->IsdLastRac = 1;

 	return( RtnSignal );
}

/////////////////////////////////////////////////////////////
// ISD_SPI_transact24( int8 Cmd, int16 Address )
//
// Takes Cmd and Address and shifts it into the ISD.
// ISD returns 24 bits and gets stored in IsdStatus_1_2
// and IsdStatus_3
/////////////////////////////////////////////////////////////
void ISD_SPI_transact24( int8 Cmd, int16 Address )
{
 int8 i;
 int16 mask;
 IsdControlType *ptr;
 
	ptr = &IsdControl;
	if (Address > ISD_MAX_ROW_ADDR-1)
	{
		Isd_Xmt_Error_Msg( ptr->IsdCmdSrc, ptr->IsdSerChannel,  ISD_ERR_BADADR );
		ptr->IsdBadAddress++;
		Address = TTS_ADDR_ERROR;
	}
 	
    // bring the clock low
  	SPI_CLK = 0;
    Delay_us(1);
    
 	// bring SS low to start the transaction
 	SPI_SS = 0;
    Delay_us(1);

 	i=0; 
 	mask=0x01; 
 	ptr->IsdSpiAddress = 0; 
 	ptr->IsdSpiStatus = 0;

 	// First, we send the 1st 2 bits of address LSB first
 	// and get the EOM, OVF bits into the 2 LSBs of IsdControl.IsdSpiStatus
 	for( i=0; i<2; i++)
 	{
  		// bring the clock low
  		SPI_CLK = 0;

  		// Set a bit to the ISD
  		if ( Address & mask ) 
    		SPI_OUT = 1;
  		else
    		SPI_OUT = 0;

  		// shift the bit from ISD
  		ptr->IsdSpiStatus = ptr->IsdSpiStatus << 1; 
  		// Get a bit from the ISD
  		if (SPI_IN) 
    		ptr->IsdSpiStatus |= 0x0001;

        Delay_us(1);
  		// Clock in the bit to the ISD
  		SPI_CLK = 1;
        Delay_us(1);
  		// shift the mask for the next bit
  		mask = mask << 1;  
 	}

 	// Now, we shift the rest of the Address and get 14 bits of the
 	// row address
 	for( ; i<16; i++)
 	{
  		// bring the clock low
  		SPI_CLK = 0;

  		// Set a bit to the ISD
  		if ( Address & mask )
    		SPI_OUT = 1;
  		else
    		SPI_OUT = 0;

  		// shift the bit from ISD
  		ptr->IsdSpiAddress = ptr->IsdSpiAddress >> 1; 
  		// Get a bit from the ISD
  		if (SPI_IN) 
    		ptr->IsdSpiAddress |= 0x8000;

        Delay_us(1);
  		// Clock in the bit to the ISD
  		SPI_CLK = 1;
        Delay_us(1);
  		// shift the mask for the next bit
  		mask = mask << 1;  
 	}

	// Then, we send 1st 2 bits of the cmd LSB first
	// and get the 2 msbs of the row address
 	for( i=0, mask=0x01; i<2; i++)
 	{
  		// bring the clock low
  		SPI_CLK = 0;

  		// Set a bit to the ISD
  		if ( Cmd & mask )
    		SPI_OUT = 1;
  		else
    		SPI_OUT = 0;

 		// shift the bit from ISD
  		ptr->IsdSpiAddress = ptr->IsdSpiAddress >> 1; 
  		// Get a bit from the ISD
  		if (SPI_IN)
    		ptr->IsdSpiAddress |= 0x8000;

        Delay_us(1);
  		// Clock in the bit to the ISD
  		SPI_CLK = 1;
        Delay_us(1);
  		// shift the mask for the next bit
  		mask = mask << 1;  
 	}

 	// Finally, we shift the last 6 bits of the cmd LSB
 	for( ; i<8; i++)
 	{
  		// bring the clock low
  		SPI_CLK = 0;

        Delay_us(1);
  		// Set a bit to the ISD
  		if ( Cmd & mask ) 
    		SPI_OUT = 1;
  		else
    		SPI_OUT = 0;

        Delay_us(1);
  		// Clock in the bit to the ISD
  		SPI_CLK = 1;
        Delay_us(1);
  		// shift the mask for the next bit
  		mask = mask << 1;  
 	}

 	// bring the clock low
  	SPI_CLK = 0;
  	
  	// bring SS hi to end the transaction
    Delay_us(1);
 	SPI_SS = 1;

}

/////////////////////////////////////////////////////////////
// ISD_SPI_transact8( int8 Cmd )
//
// Takes Cmd and shifts it into the ISD.
// ISD returns 8 bits and gets stored in the upper 8 bits
// of IsdStatus_1_2
/////////////////////////////////////////////////////////////
void ISD_SPI_transact8( int8 Cmd )
{
 int8 i;
 int16 mask;
 IsdControlType *ptr;
 
 	ptr = &IsdControl;
    
 	// bring the clock low
  	SPI_CLK = 0;
  	
    Delay_us(1);
 	// bring SS low to start the transaction
 	SPI_SS = 0;
    Delay_us(1);

	// Then, we send the cmd LSB first
 	for( i=0, mask=0x01, ptr->IsdSpiStatus = 0xFF; i<2; i++)
 	{
  		// bring the clock low
  		SPI_CLK = 0;
  		// shift the bit from ISD
  		ptr->IsdSpiStatus = ptr->IsdSpiStatus << 1; 
  		// Get a bit from the ISD
  		if (!SPI_IN) 
    		ptr->IsdSpiStatus |= 0x0002;
  		// Set a bit to the ISD
  		if ( Cmd & mask )
    		SPI_OUT = 1;
  		else
    		SPI_OUT = 0;

        Delay_us(1);
  		// Clock in the bit to the ISD
  		SPI_CLK = 1;
        Delay_us(1);
  		// shift the mask for the next bit
  		mask = mask << 1;  
 	}
	// Then, we send the rest of the cmd LSB first
 	for( ; i<8; i++)
 	{
  		// bring the clock low
  		SPI_CLK = 0;
  		// Set a bit to the ISD
  	if ( Cmd & mask )
    	SPI_OUT = 1;
  	else
    	SPI_OUT = 0;

    Delay_us(1);
  	// Clock in the bit to the ISD
  	SPI_CLK = 1;
    Delay_us(1);
  	// shift the mask for the next bit
  	mask = mask << 1;  
 	}
 	// bring the clock low
  	SPI_CLK = 0;

	// bring SS hi to end the transaction
    Delay_us(1);
 	SPI_SS = 1;
}


/////////////////////////////////////////////////////////////////////////
// char *Isd_GetStats( int8 Clear, char *buf  )
//
// Returns a report of errors for the ISD.
// Stats get cleared if Clear is non-zero.
// The report is composed of eight numbers coverted to
// ascii separated by commas.
//	int8 IsdErrMsgTimeout;
//  int8 IsdEvtCollision;
//  int8 IsdSerCmdBadStt;
//
// char *Core_IntToAsciiStr( int16 i, char *str, int8 min_digs )
/////////////////////////////////////////////////////////////////////////
char *Isd_GetStats( int8 Clear, char *buf )
{
 IsdControlType *p;

	p = &IsdControl;

	_stat_append( (unsigned long)p->IsdErrMsgTimeout, buf, TRUE,3 );
	_stat_append( (unsigned long)p->IsdEvtCollision, buf, TRUE,3 );
	_stat_append( (unsigned long)p->IsdSerCmdBadStt, buf, TRUE,3 );
	_stat_append( (unsigned long)p->IsdBadSignals, buf, TRUE,3 );
	_stat_append( (unsigned long)p->IsdBadMsgs, buf, FALSE,3 );

	if (Clear)
	{
		p->IsdErrMsgTimeout = 0;
		p->IsdEvtCollision = 0;
		p->IsdSerCmdBadStt = 0;
		p->IsdBadSignals = 0;
		p->IsdBadMsgs = 0;
	}  	
 	return( buf );
}
