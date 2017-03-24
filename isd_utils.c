/////////////////////////////////////////////////////////////////////////
////                          isd_utils.c                            ////
////                                                                 ////
//
/////////////////////////////////////////////////////////////////////////
#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include <p24FJ128GB204.h>
#include <string.h>
#include "Core_api.h"
#include "rs232_api.h"
#include "Isd_data_defs.h"
#include "tts_api.h"
#include "Tts_CmdMap.h"
#include "Isd_api.h"
#include "Isd_cmds.h"

////////////////////////////////////////////////////////////////
// GetIsdRxTokens( int8 TknIdx )
// returns the token ptr from the command source
// Commands from the RS232 ports have an extra 'PMTTS'
// header. So, we add 1 to the token index
////////////////////////////////////////////////////////////////
char *GetIsdRxTokens( uint8 CmdSrc, int8 TknIdx )
{
 	switch (CmdSrc)
 	{
  		case ISD_CMD_SRC_RS232_A:
			return( RS232_GetToken( USARTA, (TknIdx+1) ));
		case ISD_CMD_SRC_RS232_B:
			return( RS232_GetToken( USARTB, (TknIdx+1) ));
		case ISD_CMD_SRC_RS232_USB:
			return( RS232_GetToken( USART_USB, (TknIdx+1) ));            
  		case ISD_CMD_SRC_TTS:
			return( (char *)TTS_IO_GetOutToken( TknIdx ) );
  		default:
 			return(0);
 	}
}

char GetIsdRxTokenChar( uint8 CmdSrc, int8 TknIdx, int CharIdx )
{
 char *cptr;
 
 	cptr = GetIsdRxTokens( CmdSrc, TknIdx );
 	
 	if (cptr)
 		return( cptr[ CharIdx ]);
 	else
 		return('0');
}

int16 GetIsdRxTokenInt( uint8 CmdSrc, int8 TknIdx )
{
 char *cptr;
 int16 temp16;
 
 	cptr = GetIsdRxTokens( CmdSrc, TknIdx );
 	if (cptr)
 	{
 		temp16 = atol( cptr );
 	}
 	else
 	{
	 	temp16 = 0;	
	}
 	return( temp16 );
}

////////////////////////////////////////////////////////////////
// Isd_GetCmd( int CmdSrc )
// returns the command from the command source
////////////////////////////////////////////////////////////////
int Isd_GetCmd( int CmdSrc )
{
 int i;
 char cmdstr[ISD_CMD_LENGTH];
 char *p;

  	// get a pointer to the command
#ifndef TTS_NO_CHANNELS
  	p = GetIsdRxTokens( CmdSrc, 1 );
#else
  	p = GetIsdRxTokens( CmdSrc, 0 );
#endif	
	  
  	for( i=0; i<MAX_ISD_CMDS; i++ )
  	{
   		// Get a command into RAM
   		strncpy( cmdstr, IsdCmds[i], ISD_CMD_LENGTH );
   		if ( !strncmp( p, cmdstr, ISD_CMD_LENGTH ) )
   		{
    		break;
   		}
  	}
  	return i;
}

////////////////////////////////////////////////////////////////
// GetIsdChannel( int CmdSrc )
// returns the channel from the command source
////////////////////////////////////////////////////////////////
int8 GetIsdChannel( int CmdSrc )
{
#ifndef TTS_NO_CHANNELS
 char *p;
	
  	p = GetIsdRxTokens( CmdSrc, 0 );
  
  	if (p && ((*p & 0x0F) < 0x0A))
  	{
   		return( (*p & 0x0F) );
  	}
  	else
#endif
   		return ( 0 );
}

uint8 GetIsdSerPort( int CmdSrc )
{
 	switch (CmdSrc)
 	{
	 	case ISD_CMD_SRC_RS232_A:
	 		return( USARTA );
	 	case ISD_CMD_SRC_RS232_B:
	 		return( USARTB );
	 	case ISD_CMD_SRC_RS232_USB:
	 		return( USART_USB );            
	 	default:
	 		return 0;
	 }
}

int GetIsdPortSignal( uint8 port )
{
 	switch (port)
 	{
        case USARTA:
            return(ISD_CMD_SRC_RS232_A);
        case USARTB:
            return(ISD_CMD_SRC_RS232_B);
        case USART_USB:
            return(ISD_CMD_SRC_RS232_USB);                    
	 	default:
	 		return 0;
	 }
}

////////////////////////////////////////////////////////////////
// GetIsdRxTokenCnt( int CmdSrc )
// returns the token count from the command source
////////////////////////////////////////////////////////////////
int8 GetIsdRxTokenCnt( int CmdSrc )
{
 	switch (CmdSrc)
 	{
  		case ISD_CMD_SRC_RS232_A:
			return( (RS232_GetTokenCnt( USARTA )-1));
			
		case ISD_CMD_SRC_RS232_B:
			return( (RS232_GetTokenCnt( USARTB )-1));
            
		case ISD_CMD_SRC_RS232_USB:
			return( (RS232_GetTokenCnt( USART_USB )-1));            
			
  		case ISD_CMD_SRC_TTS:
			return( TTS_IO_GetOutTokenCnt() );
			
  		default:
 			return(0);
 	}
}

#define MAX_ISD_MSG_LEN 60
void Isd_Xmt_Msg( int CmdSrc, char * MsgPtr )
{
 char evtstr[MAX_ISD_MSG_LEN+1];

	switch( CmdSrc )
	{
		case ISD_CMD_SRC_RS232_A:
			
		case ISD_CMD_SRC_RS232_B:
            
        case ISD_CMD_SRC_RS232_USB:
			strcpy( evtstr, UsartHdr );
 			strncpy( evtstr+6, MsgPtr, MAX_ISD_MSG_LEN-6);
            evtstr[MAX_ISD_MSG_LEN] = 0;
 			RS232_Xmt_Msg( GetIsdSerPort( CmdSrc ), evtstr );
 			break;
 						
		case ISD_CMD_SRC_TTS:
			TTS_IO_Xmt_InMsg( MsgPtr );
    		break;
 	}   
}	

uint8 Isd_Msg_Done( int CmdSrc )
{
	switch( CmdSrc )
	{
		case ISD_CMD_SRC_RS232_A:
		case ISD_CMD_SRC_RS232_B:
        case ISD_CMD_SRC_RS232_USB:
			return(0);
 			break;
 						
		case ISD_CMD_SRC_TTS:
			return( TTS_IO_NextOutMsg() );
    		break;
 	}   
 	return(0);
}	

void IsdSendEvent( int8 CmdSrc, int8 Channel, int8 Event)
{
 char evtstr[ISD_SER_EVT_LENGHTH+2];

#ifndef TTS_NO_CHANNELS		
 	// Get a command into RAM
 	evtstr[0] = Channel | 0x30;
 	evtstr[1] = PROTOCOL_DELIM;
 	strcpy( evtstr+2, (SerEvts[(unsigned int)Event]) );
#else
	strcpy( evtstr, (SerEvts[(unsigned int)Event]) );
#endif

 	Isd_Xmt_Msg( CmdSrc, evtstr );
}

void IsdSendTtsEvent(int8 Channel, int8 Event)
{
 	IsdSendEvent( ISD_CMD_SRC_TTS, Channel, Event );
}

void IsdSendCmdDone(int8 CmdSrc, int8 Channel)
{
 	IsdSendEvent( CmdSrc, Channel, SER_EVT_CMR_TKN );
}


void IsdTrace_Init()
{
	IsdTrace.TraceBits = 0;
	IsdTrace.TracePort = USARTB;
}

#define ISDTRACE_TTS_TO_ISD 1
#define ISDTRACE_TTS_TO_ISD_MASK (1 << ISDTRACE_TTS_TO_ISD)

void IsdTrace_SetBit( int tbit )
{
	IsdTrace.TraceBits |= (1 << tbit); 	
}

void IsdTrace_ClrBit( int tbit )
{
	IsdTrace.TraceBits &= (1 << tbit) ^ 0xFFFF; 	
}

void IsdTrace_SetMask( int tmask )
{
	IsdTrace.TraceBits = tmask; 	
}

void IsdTrace_ClrMask()
{
	IsdTrace.TraceBits = 0; 	
}

void IsdTrace_SetPort( int port )
{
	IsdTrace.TracePort = port;	
}

#define MAX_ISD_TRACE_MSG_LEN MAX_ISD_MSG_LEN
void IsdTraceChK()
{
 int i,j;
 char evtstr[MAX_ISD_TRACE_MSG_LEN+1],*cptr, protocol_delim[2];
 
	if (IsdTrace.TraceBits & ISDTRACE_TTS_TO_ISD_MASK) 
	{
		i = TTS_IO_GetOutTokenCnt();
		if (i)
		{
            protocol_delim[0] = PROTOCOL_DELIM; protocol_delim[1] = 0;
			j=0; evtstr[0] = 0;
			cptr = (char *)TTS_IO_GetOutToken( j++ );
			strcat( evtstr, cptr );	
            // check the length including the protocol delimiter
			for( ; (j<i) && (strlen(evtstr)<(MAX_ISD_TRACE_MSG_LEN-1)); j++)
			{
                strcat( evtstr, protocol_delim );	
				cptr = (char *)TTS_IO_GetOutToken( j );
                if ((strlen(cptr) + strlen(evtstr)) < MAX_ISD_TRACE_MSG_LEN)
                {
                    strcat( evtstr, cptr );
                }
                else
                    break;
			}
			Isd_Xmt_Msg( GetIsdPortSignal(IsdTrace.TracePort), evtstr );
		}
	}
}

void _isd_free_port_buffer( int8 CmdSrc )		
{
		switch( CmdSrc )
		{
			case ISD_CMD_SRC_RS232_A:	
				
				RS232_FreeBuffer( USARTA, PROC_ISD_TASK );
				break;
			
			case ISD_CMD_SRC_RS232_B:
			
				RS232_FreeBuffer( USARTB, PROC_ISD_TASK );
				break;
                
			case ISD_CMD_SRC_RS232_USB:
			
				RS232_FreeBuffer( USART_USB, PROC_ISD_TASK );
				break;                
				
			default:
				break;
			
		}
} 
