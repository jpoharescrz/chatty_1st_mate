/////////////////////////////////////////////////////////////////////////
////                          tts_utils.h                              
////                                                                 
//// Handles the TTS to ISD transactions
////
//// Clients start msg xfer using:
//// int8 TTS_IO_Start_New_msg( int8 ClientId )
//// which takes a ClientId and returns TRUE if the client can continue
////
//// Clients use:
//// void TTS_IO_put_xmt_char( char c )
//// void TTS_IO_put_xmt_buf( char *bptr )
//// to write characters and buffers to the TTS_IO_Tx msg buffer
//// 
//// void TTS_IO_send() is called by the TTS task to signal the ISD task and 
//// send the msg
////
#undef TTS_UTILS
#define TTS_UTILS

#include <xc.h>
#include <p24FJ128GB204.h>
#include <string.h>
#include "Core_api.h"
#include "isd_api.h"
//#include "pots_api.h"
#include "button_api.h"
#include "rs232_api.h"
#include "tts_api.h"

// Internal prototypes
void TTS_IO_Xmt_Error_Msg( int8 );
void TTS_IO_Xmt_Msg( char * , int8);
void TTS_IO_put_xmt_char( char c);
void TTS_IO_Reset_local();

int8 I2cIsSlaveReadyToRx();
void i2cInCharProc( char TtsIoInChar );


void TTS_IO_InitTask()
{
 	TTS_IO_InCtrlBlk.MsgCnt = 0;
 	TTS_IO_InCtrlBlk.MsgTokenWtIdx = 0;
 	TTS_IO_InCtrlBlk.MsgTokenRdIdx = 0;
 	
 	TTS_IO_InCtrlBlk.TokenRdIdx = 0;
 	TTS_IO_InCtrlBlk.TokenWtIdx = 0;
 	TTS_IO_InCtrlBlk.CurTokenCnt = 0;
 	TTS_IO_InCtrlBlk.TokenCnt = 0;
 	
 	TTS_IO_InCtrlBlk.BufByteCnt = 0;
 	TTS_IO_InCtrlBlk.BufWtIdx = 0;
 	TTS_IO_InCtrlBlk.BufRdIdx = 0;
 	
 	TTS_IO_InCtrlBlk.InState = TTS_IO_CMD_WAITING;
 	TTS_IO_InCtrlBlk.BadEvents = 0;
 	TTS_IO_InCtrlBlk.DroppedPkts = 0;
 	TTS_IO_InCtrlBlk.TgtTask = PROC_TTS_TASK;
 	TTS_IO_InCtrlBlk.TgtTaskSignal = SGNL_ISD_BIT;
 	
 	TTS_IO_OutCtrlBlk.MsgCnt = 0;
 	TTS_IO_OutCtrlBlk.TokenRdIdx = 0;
 	TTS_IO_OutCtrlBlk.TokenWtIdx = 0;
 	TTS_IO_OutCtrlBlk.CurTokenCnt = 0;
 	TTS_IO_OutCtrlBlk.TokenCnt = 0;
 	
 	TTS_IO_OutCtrlBlk.BufByteCnt = 0;
 	TTS_IO_OutCtrlBlk.BufWtIdx = 0;
 	TTS_IO_OutCtrlBlk.BufRdIdx = 0;
 	
 	TTS_IO_OutCtrlBlk.InState = TTS_IO_CMD_WAITING;
 	TTS_IO_OutCtrlBlk.BadEvents = 0;
 	TTS_IO_OutCtrlBlk.DroppedPkts = 0;
 	
 	TTS_IO_OutCtrlBlk.TgtTask = PROC_ISD_TASK;
 	TTS_IO_OutCtrlBlk.TgtTaskSignal = SGNL_TTS_BIT;
}


uint8 IS_TTS_IO_CONSUMER_RDY( uint8 Task )
{
	switch( Task )
	{
		case PROC_ISD_TASK:
		
			return( IsIsdReady( SGNL_TTS_MASK ) );
			break;
		
		case PROC_TTS_TASK:
		
			return( 1 );
			break;
		
		default:
		
			return( 0 );
			break;
	}
}


void SGNL_TTS_IO_CONSUMER( uint8 Task )
{
	switch( Task )
	{
		case PROC_ISD_TASK:
		
			SignalTask( PROC_ISD_TASK, SGNL_TTS_BIT );;
			break;
		
		case PROC_TTS_TASK:
		
			SignalTask( PROC_TTS_TASK, SGNL_ISD_BIT );
			break;
			
		default:
		
			break;
	}
}

//////////////////////////////////////////////////////////////////
// void TTS_IO_send()
//
// Increments the command count and signals the ISD Task
//////////////////////////////////////////////////////////////////
void TTS_IO_send( TTS_IO_CtrlBlkRec *p )
{
 	p->MsgCnt++;
 	SignalTask( p->TgtTask, p->TgtTaskSignal );
}

//////////////////////////////////////////////////////////////////
// uint8 TtsIoTknStart( TTS_IO_CtrlBlkRec p )
//
// Records the start of the token as the current buffer write index
// Returns 0 if no room in the token buffer
//////////////////////////////////////////////////////////////////
uint8 TtsIoTknStart(  TTS_IO_CtrlBlkRec *p )
{
 uint8 *ptr;
 
 	if ( p->TokenCnt < TTS_IO_MAX_TOKENS )
 	{
 		ptr = &(p->Buf[ p->BufWtIdx ]);	// get a pointer to wherer the msg starts
    	p->Tokens[ p->TokenWtIdx++ ] = ptr;
    	if ( p->TokenWtIdx > TTS_IO_MAX_TOKENS-1 )
    		p->TokenWtIdx = 0;
    	
    	p->CurTokenCnt++;
    	p->TokenCnt++;
    	return( 1 );
	}
	else
	{
		return( 0 );
	}
   
}

//////////////////////////////////////////////////////////////////
// uint8 TtsIoCharStore( TTS_IO_CtrlBlkRec *p )
//
// Stores the character in the buffer. Returns 0 if no room in the
// buffer
//////////////////////////////////////////////////////////////////
uint8 TtsIoCharStore(  TTS_IO_CtrlBlkRec *p, char c )
{
	if ( p->BufByteCnt < TTS_IO_BUF_SIZE )
	{
		p->Buf[ p->BufWtIdx++ ] = c; //store the character
           
    	if ( p->BufWtIdx > TTS_IO_BUF_SIZE-1 )
    		p->BufWtIdx = 0;
               		
    	p->BufByteCnt++;
    	p->MsgTokens[p->MsgTokenWtIdx].MsgByteCnt++;
    	
    	return( 1 );
	}
	else
	{
	 	return( 0 );	
	}	   
}

//////////////////////////////////////////////////////////////////
// void TtsIoDiscardMsg( TTS_IO_CtrlBlkRec *p )
//
// Restores TokenWtIdx, TokenCnt, BufWtIdx, BufByteCnt to what it was
// when the msg was started.
// The InState is reset and stats updated
//////////////////////////////////////////////////////////////////
void TtsIoDiscardMsg( TTS_IO_CtrlBlkRec *p )
{
	// Restore the Token buffer write index
	p->TokenWtIdx = p->MsgTokens[p->MsgTokenWtIdx].TokenIdx;
	// Restore Total Token count
    p->TokenCnt = p->TokenCnt_BU;
    // Restore Buffer write index
    p->BufWtIdx = p->BufWtIdx_BU;
    // Restore Buffer Byte Cnt
    p->BufByteCnt = p->BufByteCnt_BU;
   	
    p->InState = TTS_IO_CMD_WAITING;
    p->DroppedPkts++;
            	
}

//////////////////////////////////////////////////////////////////
// TtsIoInCharProc( TTS_IO_CtrlBlkRec *p, char c )
//
// Handle the next char from the slave
//////////////////////////////////////////////////////////////////
int TtsIoCharProc( TTS_IO_CtrlBlkRec *p, char c )
{ 
 
 int bad_state, result;

  	switch (p->InState)
  	{
   		// wait for the start of a msg   
   		case TTS_IO_CMD_WAITING:
   		                  
     		if(c == TTS_IO_PROTOCOL_START)
     		{
      			p->CurTokenCnt = 0;   			// No Tokens yet
        
      			p->MsgTokens[p->MsgTokenWtIdx].TokenIdx = p->TokenWtIdx;
      			p->MsgTokens[p->MsgTokenWtIdx].TokenCnt = 0;
      			p->MsgTokens[p->MsgTokenWtIdx].MsgByteCnt = 0;
      			
      			// Take a snapshot of the Total Token count in case this fails
      			p->TokenCnt_BU = p->TokenCnt;
      			p->BufWtIdx_BU = p->BufWtIdx;
      			p->BufByteCnt_BU = p->BufByteCnt;
 
 				if ( TtsIoTknStart( p ) != FALSE ) // If we could store anymore tokens
 				{
	 				p->InState = TTS_IO_CMD_RECEIVING;  // Start of a msg, next state
	 			}	 
	 			else	
	 				return( FALSE );
     		}
     		else
     			return( FALSE );
     			
     		break;

   		// store non-delim chars in TTS_IO_RxBuf char in the command
   		case TTS_IO_CMD_RECEIVING:
   		
     		switch (c)
     		{
      			case TTS_IO_PROTOCOL_DELIM:
      			
      				result = TtsIoCharStore(  p, 0 );	// terminate the Token
      				
      				if ( result == 0 ) // If we couldn't store the char
 					{
	 					TtsIoDiscardMsg( p );
	 					return( FALSE );
	 					break;
	 				}
	 				
	 				result = TtsIoTknStart( p );	// start a new token
	 					
	 				if ( result == 0 )				// if we couldn't start the token
	 				{
		 				TtsIoDiscardMsg( p );
		 				return( FALSE );
	 				}
	 				
           			break;
           
      			case TTS_IO_PROTOCOL_STOP:
      	
      	   			if (( p->MsgCnt > TTS_IO_MAX_CMDS ) || 
      	   				(!IS_TTS_IO_CONSUMER_RDY( p->TgtTask )))
      	   			{
	      	   			TtsIoDiscardMsg( p );
	      	   			return( FALSE );
	       			}
	       			else
	       			{
		       			result = TtsIoCharStore(  p, 0 );	// Store the character
      	
           				if ( result == 0 )
           				{
	           				TtsIoDiscardMsg( p );
	           				return( FALSE );
	           				break;
           				}
               		
               			p->MsgCnt++;
               		
            			p->MsgTokens[p->MsgTokenWtIdx++].TokenCnt = p->CurTokenCnt;
           
            			if ( p->MsgTokenWtIdx > TTS_IO_MAX_CMDS-1 )
              				p->MsgTokenWtIdx = 0;
              		
            			p->CurTokenCnt = 0;
            			SGNL_TTS_IO_CONSUMER( p->TgtTask );
            			p->InState = TTS_IO_CMD_WAITING;
           			}   
           			break;
           	
        		case TTS_IO_PROTOCOL_START:
           
        			// Handle a msg restart
        			p->CurTokenCnt = 0;   			// No Tokens yet
        			TtsIoDiscardMsg( p );
        			result = TtsIoTknStart( p );
 				
 					if ( result != 0 ) // If we could store tokens
 					{
	 					p->InState = TTS_IO_RX_CMD_RECEIVING;  // Start of a msg, next state
	 				}	
	 				return( FALSE );
           			break;
           
      			default:
      	
					// Write the token chars to the receive buffer
            		result = TtsIoCharStore(  p, c );	// Store the character
      	
           			if ( result == 0 )
           			{
	           			TtsIoDiscardMsg( p );
	           			return( FALSE );
           			}
           			break;
 
     		}//switch
     		break;
     
     	default:  // Bad RxState
	   		bad_state = 1; //diag
	   		return( FALSE );
       		break;
       		
    }//switch
    return( TRUE );
}

///////////////////////////////////////////////////////////////////
// int TTS_IO_put_OutChar( char c )
//
// Takes a character and writes it to the
// outgoing buffer. Both the general tx buffer count and
// command buffer count is incremented.
///////////////////////////////////////////////////////////////////
int TTS_IO_put_OutChar( char c )
{
 	return( TtsIoCharProc( &TTS_IO_OutCtrlBlk, c ) );
}

///////////////////////////////////////////////////////////////////
// int TTS_IO_put_InChar( char c )
//
// Takes a character and writes it to the
// incoming buffer. Both the general tx buffer count and
// command buffer count is incremented.
///////////////////////////////////////////////////////////////////
int TTS_IO_put_InChar( char c )
{
 	return( TtsIoCharProc( &TTS_IO_InCtrlBlk, c ) );
}

int TTS_IO_put_Buf( TTS_IO_CtrlBlkRec *p, char *bptr )
{
 
 	while ((*bptr) && ( p->BufByteCnt < TTS_IO_BUF_SIZE )) 
 	{
   		if (TtsIoCharProc( p, *bptr++ ) == FALSE) 
   			return( FALSE );			
 	}
 	return( TRUE );
}

///////////////////////////////////////////////////////////////////
// int TTS_IO_put_OutBuf( char *bptr )
//
// Takes a pointer to a character buffer an writes it to
// outgoing buffer. 
///////////////////////////////////////////////////////////////////
int TTS_IO_put_OutBuf( char *bptr )
{
 	return( TTS_IO_put_Buf( &TTS_IO_OutCtrlBlk, bptr )) ;
}

///////////////////////////////////////////////////////////////////
// int TTS_IO_put_InBuf( char *bptr )
//
// Takes a pointer to a character buffer an writes it to
// outgoing buffer. 
///////////////////////////////////////////////////////////////////
int TTS_IO_put_InBuf( char *bptr )
{
	return( TTS_IO_put_Buf( &TTS_IO_InCtrlBlk, bptr )) ;
}

///////////////////////////////////////////////////////////////////
// int TTS_IO_Xmt_OutMsg( char *MsgPtr )
// 
// Sends the msg to the slave in one atomic function
///////////////////////////////////////////////////////////////////
int TTS_IO_Xmt_OutMsg( char *MsgPtr )
{
  	if (TTS_IO_put_OutChar( TTS_IO_PROTOCOL_START) == FALSE) 
  		return( FALSE );
  		
  	if (TTS_IO_put_OutBuf( MsgPtr ) == FALSE) 
  		return( FALSE );
  		
  	if (TTS_IO_put_OutChar( TTS_IO_PROTOCOL_STOP) == FALSE) 
  		return( FALSE );
  	return( TRUE );
}

///////////////////////////////////////////////////////////////////
// int TTS_IO_Xmt_InMsg( char *MsgPtr )
// 
// Sends the msg to the TTS Task in one atomic function
///////////////////////////////////////////////////////////////////
int TTS_IO_Xmt_InMsg( char *MsgPtr )
{
  	if (TTS_IO_put_InChar( TTS_IO_PROTOCOL_START) == FALSE)
  		return( FALSE );
  		
  	if (TTS_IO_put_InBuf( MsgPtr ) == FALSE)
  		return( FALSE );
  		
  	if (TTS_IO_put_InChar( TTS_IO_PROTOCOL_STOP) == FALSE)
  		return( FALSE );
  		
  	return( TRUE );
}

int8 TTS_IO_GetFreeByteCnt( TTS_IO_CtrlBlkRec *p )
{
 
 	if (( p->MsgCnt < TTS_IO_MAX_CMDS ) && ( p->TokenCnt < TTS_IO_MAX_TOKENS ))
   		return( TTS_IO_BUF_SIZE - p->BufByteCnt );
 	else
   		return( 0 );
}

///////////////////////////////////////////////////////////////////
// int8 TTS_IO_GetOutFreeByteCnt()
// 
// Returns the number of free bytes in the outgoing buffer
///////////////////////////////////////////////////////////////////
int8 TTS_IO_GetOutFreeByteCnt()
{
 	return( TTS_IO_GetFreeByteCnt( &TTS_IO_OutCtrlBlk ));
}

///////////////////////////////////////////////////////////////////
// int8 TTS_IO_GetInFreeByteCnt()
// 
// Returns the number of free bytes in the incoming buffer
///////////////////////////////////////////////////////////////////
int8 TTS_IO_GetInFreeByteCnt()
{
	return( TTS_IO_GetFreeByteCnt( &TTS_IO_InCtrlBlk ));
}

///////////////////////////////////////////////////////////////////
// int8 TTS_IO_GetOutTokenCnt()
// 
// Returns the number of tokens in the incoming buffer
///////////////////////////////////////////////////////////////////
uint8 TTS_IO_GetOutTokenCnt()
{
 TTS_IO_CtrlBlkRec *p;
	
 	p = &TTS_IO_OutCtrlBlk;
 
 	return( p->MsgTokens[ p->MsgTokenRdIdx ].TokenCnt );
}

///////////////////////////////////////////////////////////////////
// int8 TTS_IO_GetInTokenCnt()
// 
// Returns the number of tokens in the incoming buffer
///////////////////////////////////////////////////////////////////
uint8 TTS_IO_GetInTokenCnt()
{
 TTS_IO_CtrlBlkRec *p;
	
 	p = &TTS_IO_InCtrlBlk;
 
 	return( p->MsgTokens[ p->MsgTokenRdIdx ].TokenCnt );
}


void TTS_IO_ClrTokenCnt( TTS_IO_CtrlBlkRec *p )
{
 
 	p->MsgTokens[ p->MsgTokenRdIdx ].TokenCnt = 0;
}
///////////////////////////////////////////////////////////////////
// void TTS_IO_ClrOutTokenCnt()
// 
// Clears the number of tokens in the outgoing buffer
///////////////////////////////////////////////////////////////////
void TTS_IO_ClrOutTokenCnt()
{
	
 	TTS_IO_ClrTokenCnt( &TTS_IO_OutCtrlBlk );
}

///////////////////////////////////////////////////////////////////
// void TTS_IO_ClrInTokenCnt()
// 
// Clears the number of tokens in the incoming buffer
///////////////////////////////////////////////////////////////////
void TTS_IO_ClrInTokenCnt()
{
	
	TTS_IO_ClrTokenCnt( &TTS_IO_InCtrlBlk );
}


///////////////////////////////////////////////////////////////////
// char *TTS_IO_GetToken( TTS_IO_CtrlBlkRec *p, int8 token )
// 
// Returns a pointer to the specified token in the outgoing buffer
///////////////////////////////////////////////////////////////////
uint8 TTS_IO_TokenCopy[20];
uint8 *TTS_IO_GetToken( TTS_IO_CtrlBlkRec *p, int8 token )
{
 int16 token_idx,i;
 unsigned char *ptr;
 
 	token_idx = p->MsgTokens[p->MsgTokenRdIdx].TokenIdx + token;
 	if (token_idx > TTS_IO_MAX_TOKENS-1)
 	{
  		token_idx = token_idx - TTS_IO_MAX_TOKENS;	 
 	}
 	
 	for (i=0, ptr = p->Tokens[ token_idx ]; i<20; i++)
 	{
	 	TTS_IO_TokenCopy[i] = *ptr;
	 	if ( *ptr == 0 ) break;
	 	if ( ptr == &(p->Buf[TTS_IO_BUF_SIZE-1]) ) 
	 		ptr = &(p->Buf[0]);
	 	else
	 		ptr++;
	}
	TTS_IO_TokenCopy[19] = 0;
 	return( TTS_IO_TokenCopy );
}

///////////////////////////////////////////////////////////////////
// char *TTS_IO_GetOutToken( int8 token )
// 
// Returns a pointer to the specified token in the outgoing buffer
///////////////////////////////////////////////////////////////////
uint8 *TTS_IO_GetOutToken( int8 token )
{

 	return( TTS_IO_GetToken( &TTS_IO_OutCtrlBlk, token) );
}

///////////////////////////////////////////////////////////////////
// char TTS_IO_GetInToken( int8 token )
// 
// Returns a pointer to the specified token in the incoming buffer
///////////////////////////////////////////////////////////////////
uint8 *TTS_IO_GetInToken( int8 token )
{
	
  	return( TTS_IO_GetToken( &TTS_IO_InCtrlBlk, token) );
}

///////////////////////////////////////////////////////////////////
// char TTS_IO_GetOutMsgCnt( )
// 
// Returns a count of Messages in the outgoing buffer
///////////////////////////////////////////////////////////////////
uint8 TTS_IO_GetOutMsgCnt()
{ 	
 	return( TTS_IO_OutCtrlBlk.MsgCnt );
}

///////////////////////////////////////////////////////////////////
// char TTS_IO_GetInMsgCnt( )
// 
// Returns a count of Messages in the incoming buffer
///////////////////////////////////////////////////////////////////
uint8 TTS_IO_GetInMsgCnt()
{ 	
 	return( TTS_IO_InCtrlBlk.MsgCnt );
}

///////////////////////////////////////////////////////////////////
// char TTS_IO_NextOutMsgCnt( )
// 
// Sets the current message to the next one in the outgoing buffer
///////////////////////////////////////////////////////////////////
uint8 TTS_IO_NextMsg( TTS_IO_CtrlBlkRec *p )
{ 	
 
 	if ( p->MsgCnt > 0 )
 	{
	 	p->TokenCnt -= p->MsgTokens[ p->MsgTokenRdIdx ].TokenCnt;
	 	p->BufByteCnt -= p->MsgTokens[p->MsgTokenRdIdx].MsgByteCnt;
	 	
        p->TokenRdIdx += p->MsgTokens[ p->MsgTokenRdIdx ].TokenCnt;
        if ( p->TokenRdIdx > TTS_IO_MAX_TOKENS-1 )
        	p->TokenRdIdx = p->TokenRdIdx - TTS_IO_MAX_TOKENS;
        	
	 	p->BufRdIdx += p->MsgTokens[p->MsgTokenRdIdx].MsgByteCnt;
	 	if ( p->BufRdIdx > TTS_IO_BUF_SIZE-1 )
    	 	p->BufRdIdx = p->BufRdIdx - TTS_IO_BUF_SIZE;
    	 	        	
	 	p->MsgTokenRdIdx++;
	 	if ( p->MsgTokenRdIdx > TTS_IO_MAX_CMDS-1 )
        	p->MsgTokenRdIdx = 0;

        p->MsgCnt--;
	}
 	return( p->MsgCnt );
}

///////////////////////////////////////////////////////////////////
// char TTS_IO_NextOutMsgCnt( )
// 
// Sets the current message to the next one in the outgoing buffer
///////////////////////////////////////////////////////////////////
uint8 TTS_IO_NextOutMsg()
{ 	
 	return( TTS_IO_NextMsg(&TTS_IO_OutCtrlBlk));
}

///////////////////////////////////////////////////////////////////
// char TTS_IO_NextInMsgCnt( )
// 
// Sets the current message to the next one in the outgoing buffer
///////////////////////////////////////////////////////////////////
uint8 TTS_IO_NextInMsg()
{ 	
	return( TTS_IO_NextMsg(&TTS_IO_InCtrlBlk));
}




