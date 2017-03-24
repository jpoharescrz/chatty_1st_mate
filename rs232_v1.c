/////////////////////////////////////////////////////////////////////////
////                          RS232_v1.c                              ////
////                                                                 ////

#define RS232 1
#define UART34_NOWARN 1

#include <xc.h>
#include <p24FJ128GB204.h>
#include <string.h>
#include "Core_api.h"
#include "ADSC_LITE_0.X/mcc_generated_files/uart1.h"
#include "ADSC_LITE_0.X/mcc_generated_files/uart2.h"
#include "RS232_api.h"
#include "button_api.h"
#include "usb_usart_api.h"


////////////////////////////////////////////////////////////////////////
// void Init_CBlk( RS232_CBlk *p )
// 
// Initializes the RS232 Control Blocks
// 
////////////////////////////////////////////////////////////////////////
void Init_CBLk( RS232_CBlk *p )
{
	p->RxState = RX_CMD_WAITING;
	p->RxLoop = FALSE;
	p->RxSwitch = NO_SWITCHING;
    p->RxSwitchToUsb = NO_SWITCHING;
	p->RxHdrChk = TRUE;
	p->FillBuffer=BUFFER0;

	p->RxTokenCnt=0;
	p->RxMsgTokenCnt=0;
	p->RxTokenIdx=0;
	p->RxTokenStart=0;
	p->RxMsgTokenStart=0;
 
	p->RxBufIdx = 0;
	p->RxBufCnt = 0;
	p->RxPFErrs = 0;
	p->RxBufOverFlowErrs = 0;
	
	p->RxMsgs = 0;
    p->RxBytes = 0;
	p->RxUnMatchedMsgs = 0;
    p->RxClientBusyErrs = 0;
	p->TxMsgs = 0;
    p->TxBytes = 0;
	
	p->CodeErrs = 0;
	
	p->TxBufOverFlowErrs = 0;
 
	p->TxByteCnt = 0;
	p->TxBufWtIdx = 0;
	p->TxBufRdIdx = 0;
}

void RS232_load_msg_options()
{
 uint8 *p;
 msg_options_rec rec;
 int i,j;
	
	if (Cnfg_IsCnfgFlash2ParmValid( CNFG_FLASH_2_RS232_MSG_OPTIONS_PFL_IDX ))
	{
		p = Cnfg_GetCnfgFlash2ParmPtr( CNFG_FLASH_2_RS232_MSG_OPTIONS_PFL_IDX );
        p += 4; // skip past checksum
        for (i=1; i<7; i++)
        {
            rs232_msgs[i].msg_hdr[0] = p[0]; // first talker id char
            rs232_msgs[i].msg_hdr[1] = p[1]; // second talker id char
            rec.flash_image_bytes[2] = p[2]; // msg options
            rs232_msgs[i].usart_A_enabled = rec.s.usart_A_enabled;
            rs232_msgs[i].usart_B_enabled = rec.s.usart_B_enabled;
            rs232_msgs[i].usart_USB_enabled = rec.s.usart_USB_enabled;
            p += 4;
        }
	}
    else // update flash with default
    {
        p = Cnfg_GetCnfgFlash2ParmPtr( CNFG_FLASH_2_RS232_MSG_OPTIONS_PFL_IDX );
        for ( i=1; i<7; i++)
        {
            p += 4;
            p[0] = rs232_msgs[i].msg_hdr[0]; // first talker id char   // load talker ID
            p[1] = rs232_msgs[i].msg_hdr[1]; // first talker id char;   // Load talker ID
            rec.s.usart_A_enabled = rs232_msgs[i].usart_A_enabled;
            rec.s.usart_B_enabled = rs232_msgs[i].usart_B_enabled;
            rec.s.usart_USB_enabled = rs232_msgs[i].usart_USB_enabled;
            p[2] = rec.flash_image_bytes[2]; // msg options
        }
        j = Cnfg_GetCnfgFlash2ParmChecksum( CNFG_FLASH_2_RS232_MSG_OPTIONS_PFL_IDX );
        p = Cnfg_GetCnfgFlash2ParmPtr( CNFG_FLASH_2_RS232_MSG_OPTIONS_PFL_IDX );
        p[0] = (uint8)j;
        Cnfg_WriteCnfgFlash2Parm( CNFG_FLASH_2_RS232_MSG_OPTIONS_PFL_IDX, 0 );
    }
}

////////////////////////////////////////////////////////////////////////
// void RS232_Init()
// 
// Initializes the RS232 ports
// 
////////////////////////////////////////////////////////////////////////

void RS232_Init()
{
 RS232_CBlk *p;
    
    //usart_rx_tx_buf_status.status = 0;
 
	Init_CBLk( &RS232_a );
		
	Init_CBLk( &RS232_b );
    
    Init_CBLk( &RS232_usb );
	
	RS232_a.PortNumber = USARTA;
	RS232_a.PortSgnlNumber = SGNL_USARTA_BIT;
	RS232_a.SwitchPortCBlk = &RS232_b;
	RS232_a.buffer_rsrc_lock[0] = RSRC_USARTA_BUF0;
	RS232_a.buffer_rsrc_lock[1] = RSRC_USARTA_BUF1;
	RS232_a.switch_ptr = &RS232_Switch_a;
	RS232_Switch_a.SwitchBufIdx = 0xFF;
	
	if (Btn_GetParm( BTN_PARM_XNG_LOOP_A_IDX ))
		RS232_a.switch_cnfg.s.rx_loop = TRUE;
		
	if (Btn_GetParm( BTN_PARM_XNG_A2B_IDX ))
		RS232_a.switch_cnfg.s.switch_to_usart_b = FALSE;
	
	p = &RS232_a;

    UART1_Initialize();
	
	RS232_b.PortNumber = USARTB;
	RS232_b.PortSgnlNumber = SGNL_USARTB_BIT;
	RS232_b.SwitchPortCBlk = &RS232_a;
	RS232_b.buffer_rsrc_lock[0] = RSRC_USARTB_BUF0;
	RS232_b.buffer_rsrc_lock[1] = RSRC_USARTB_BUF1;
	RS232_b.switch_ptr = &RS232_Switch_b;
	RS232_Switch_b.SwitchBufIdx = 0xFF;
	
	if (Btn_GetParm( BTN_PARM_XNG_LOOP_B_IDX ))
		RS232_b.switch_cnfg.s.rx_loop = TRUE;
		
	if (Btn_GetParm( BTN_PARM_XNG_B2A_IDX ))
		RS232_b.switch_cnfg.s.switch_to_usart_a = FALSE;
		
	p = &RS232_b;
 
    UART2_Initialize();

	RS232_usb.PortNumber = USART_USB;
	RS232_usb.PortSgnlNumber = SGNL_USART_USB_BIT;
	RS232_usb.SwitchPortCBlk = &RS232_a;
	RS232_usb.buffer_rsrc_lock[0] = RSRC_USARTUSB_BUF0;
	RS232_usb.buffer_rsrc_lock[1] = RSRC_USARTUSB_BUF1;
	RS232_usb.switch_ptr = &RS232_Switch_usb;
	RS232_Switch_usb.SwitchBufIdx = 0xFF;

	RS232_usb.RxLoop = FALSE;
	RS232_usb.RxSwitch = FALSE;
    
    RS232_load_msg_options();   //Loads msg talker ids and options from flash
}

void RS232_A_IntEnable()
{
	RS232_UpdatePortASpeed();
    _U1RXIE = 1; //Enable UART1 RX Interrupt
    IEC4bits.U1ERIE = 1; // Enable error interrupt
} 

void RS232_B_IntEnable()
{
	RS232_UpdatePortBSpeed();
    _U2RXIE = 1; //Enable UART2 RX Interrupt
    IEC4bits.U2ERIE = 1; // Enable error interrupt
} 

void RS232_A_IntDisable()
{
	_U1RXIE = 0; //Disable UART1 RX Interrupt
    IEC4bits.U1ERIE = 0; // Disable error interrupt
}

void RS232_B_IntDisable()
{
	_U2RXIE = 0; //Disable UART2 RX Interrupt
    IEC4bits.U2ERIE = 0; // Disable error interrupt
}

char *RS232_GetMsgHdrStr( uint8 MsgIdx, char *p)
{
    if (MsgIdx)      
	{
		strcpy( p, rs232_msgs[ MsgIdx ].msg_hdr );
 	}
 	else
 	{
	 	RS232_a.CodeErrs++;
		strcpy( p, rs232_msgs[ 0 ].msg_hdr );
 	}	
 	return( p );
}

//**************************************************************
// int8 RS232_HDRCHK_FUNC( uint8 *cptr )
//
// Takes a pointer to the command string in the RS232
// rx buffer (cptr) and tries to match it with a command
// we're interested in. Returns an index of the command if
// matched and 0xff if no match. We modify RS232_Cnfg.h's 
// 'int8 RS232_x_HdrChk( char *cptr )' to call this function
// and pass on the result.
//**************************************************************
#define PROC_WILDCARD_CHARS
int8 RS232_HDRCHK_FUNC( uint8 *cptr )
{
 int16 i,j;
 // char cmdstr[RS232_HDR_SIZE+1];
 char *p;

  	// Set i to the NMEA command index
    for( i=1; i<MAX_RS232_MSGS; i++ )      
  	{
        
        p = rs232_msgs[i].msg_hdr;
#ifdef PROC_WILDCARD_CHARS
        for ( j=0; (j<RS232_HDR_SIZE+1) && (p[j]>0); j++)
        {
            if (p[j] == '?')
                continue;
            else
                if (cptr[j] != p[j])
                    break;
        }
        if (p[j] == 0)
            return(i);
#else        
    	if ( !strcmp( (char *)cptr, p ) )
   		{
      		return( i );
    		break;
   		}//if
#endif        
  	}//for
    return(0);
}

////////////////////////////////////////////////////////////////////////
// uint8 NextRxBuffer( uint8 buf )
// 
// Takes the current buffer number 0-MAX_RX_BUFFERS
// and returns the next buffer to use
////////////////////////////////////////////////////////////////////////
uint8 NextRxBuffer( uint8 buf )
{
	uint8 i;

	i = AddModulo( buf, 1, MAX_RX_BUFFERS );
	return( i );
}

/////////////////////////////////////////////////////////////////////////
// void RS232_BufferInit(  RS232_CBlk *p, uint8 buf )
//
// Initializes the RS232_a variables to use the buffer spec'd in buf
/////////////////////////////////////////////////////////////////////////
void RS232_BufferInit( RS232_CBlk *p, uint8 buf )
{
	p->RxTokenCnt = 0;   // No Tokens yet
	p->RxBufCnt = 0;   // No Bytes yet
	p->RxBufIdx = buf * RX_BUFFER_SIZE;      // No Characters yet
	p->RxTokenIdx = buf * RX_TOKEN_BUF_SIZE;
	p->RxTokenStart= p->RxTokenIdx;
	p->HdrCnt = 0;		                   // Start the header check
}

/////////////////////////////////////////////////////////////////////////
// void RS232_next_token( RS232_CBlk *p )
//
// Puts the current buffer address in the token list
/////////////////////////////////////////////////////////////////////////
void RS232_next_token( RS232_CBlk *p )
{        
	if ( p->RxTokenCnt < MAX_RX_TOKENS-1 )
	{
		p->RxTokens[ p->RxTokenIdx++ ] = &(p->RxBuf[ p->RxBufIdx ]);
		p->RxTokenCnt++;
	}
	else
	{
		p->RxState = RX_CMD_WAITING;  // bail this msg
		p->RxPFErrs++;
	}
}    

/////////////////////////////////////////////////////////////////////////
// void RS232_new_msg_init( )
//
// Initializes the RS232_a variables to accept a new msg
/////////////////////////////////////////////////////////////////////////
void RS232_new_msg_init( RS232_CBlk *p )
{	     
 RS232_Switch_CBlk *swp;
 
 	swp = p->switch_ptr;
 	
	if ( RsrcIsLockFree( p->buffer_rsrc_lock[ p->FillBuffer ] ) && (swp->SwitchBufIdx != p->FillBuffer) )
	{	
    	RS232_BufferInit( p, p->FillBuffer );
    	p->RxChkSum = 0;
    	RS232_next_token( p );
		if (p->RxHdrChk)
		{
      		p->RxState = RX_CMD_HDR_CHK;       // Start of a header, next state
		}
		else
		{   
     		p->RxState = RX_CMD_RECEIVING;     // Ignore the header, just start collecting 
		}
	}	  
	else
		p->RxBufOverFlowErrs++; 
}    

/////////////////////////////////////////////////////////////////////////
// void RS232_write_rx_buf( RS232_CBlk *p, uint8 c )
//
// Writes a byte (c) to the current rx buffer position
/////////////////////////////////////////////////////////////////////////
void RS232_write_rx_buf( RS232_CBlk *p, uint8 c )
{
	if ( p->RxBufCnt < MAX_RX_CHARS-1 )
	{
		p->RxBuf[ p->RxBufIdx++ ] = c;
		p->RxBufCnt++;
	}
	else
	{
		p->RxState = RX_CMD_WAITING;  // bail this msg
		p->RxPFErrs++;
	}
}  

/////////////////////////////////////////////////////////////////////////
// void RS232_Signal_Switching( RS232_CBlk *p )
//
// Signals the RS232 switching task
/////////////////////////////////////////////////////////////////////////
int8 RS232_Signal_Switching( RS232_CBlk *p )
{
 RS232_Switch_CBlk *swp;
 int8 RtnSignal;
 
    RtnSignal = 0;
 	swp = p->switch_ptr;
 	swp->SwitchBufIdx = p->FillBuffer;
 	
	switch( p->PortNumber )
 	{
	 	case USARTA:
            RtnSignal |= SGNL_USARTA_SWITCH_MASK;
	 		break;
	 		
	 	case USARTB:
            RtnSignal |= SGNL_USARTB_SWITCH_MASK;
	 		break;
            
	 	case USART_USB:
            RtnSignal |= SGNL_USART_USB_SWITCH_MASK;
	 		break;            
	 	
	 	default:	 	
	 		break;
	}
    return( RtnSignal );
}

RS232_CBlk *_rs232_get_port_cblk( uint8 port_number )
{
 RS232_CBlk *p;
 
	switch (port_number)
	{
		case USARTA:	
			p = RS232_CBlkPtrs[ USARTA ];
			break;
			
		case USARTB:
			p = RS232_CBlkPtrs[ USARTB ];
			break;
            
		case USART_USB:
			p = RS232_CBlkPtrs[ USART_USB ];
			break;            
			
		default:
			p = RS232_CBlkPtrs[ USARTA ];
			Core_CodeError(RS232_FATAL_ERROR);
			break;		
	}
	return( p );
}

/////////////////////////////////////////////////////////////////////////
// void RS232_Signal_Client( RS232_CBlk *p )
//
// Calls the proper Client Signal function
/////////////////////////////////////////////////////////////////////////
void RS232_Signal_Client( RS232_CBlk *p )
{
	uint8 task;
	
	//task = RS232_MsgConstOptions[ p->RxHdrIdx ].task;
    task = rs232_msgs[ p->RxHdrIdx ].task;
	
	SignalTask( task, p->PortSgnlNumber );
	RsrcSetLockOwner( p->buffer_rsrc_lock[ p->FillBuffer ], task );
}

/////////////////////////////////////////////////////////////////////////
// void RS232_FreeBuffer( uint8 UsartIdx, uint8 task )
//
// Frees the buffer the task was using
/////////////////////////////////////////////////////////////////////////
void RS232_FreeBuffer( uint8 UsartIdx, uint8 task )
{
 RS232_CBlk *p;
 int i;
 
 	p = _rs232_get_port_cblk( UsartIdx );
 	
 	for( i=0; i<MAX_RX_BUFFERS; i++)
 	{
	 	if (RsrcGetLockOwner( p->buffer_rsrc_lock[i] ) == task)
	 	{
		 	RsrcFreeLock( p->buffer_rsrc_lock[i] );
	 		return;
	 	}
	}
	Core_CodeError( RS232_FATAL_ERROR );
}

/////////////////////////////////////////////////////////////////////////
// uint16 RS232_Client_Rdy( RS232_CBlk *p )
//
// Checks if the Client is ready to be signaled
/////////////////////////////////////////////////////////////////////////
uint16 RS232_Client_Rdy( RS232_CBlk *p )
{
uint8 task;
	
	task = rs232_msgs[ p->RxHdrIdx ].task;
    if (GetTaskSignal( task, p->PortSgnlNumber ))
        return(0);
    else
    {
        if (task == PROC_ISD_TASK)
        {
            if (IsIsdReady( (1 << p->PortSgnlNumber) ))
                return(1);
        }
        else
        {
            return(1);
        }
    }
    return(0);
}
int _rs232_msg_enabled( RS232_CBlk *p) 
{
int i=0;
    
    switch( p->PortNumber )
 	{
	 	case USARTA:
            if ( p->RxHdrIdx && rs232_msgs[ p->RxHdrIdx ].usart_A_enabled) 
                i = 1;
            break;
        case USARTB:
            if ( p->RxHdrIdx && rs232_msgs[ p->RxHdrIdx ].usart_B_enabled)
                i = 1;
            break;
	 		
	 	case USART_USB:
            if ( p->RxHdrIdx && rs232_msgs[ p->RxHdrIdx ].usart_USB_enabled)
                i = 1;
	 		break;
	 	
	 	default:
	 		break;
	}
	return(i);
}    

                  
int _rs232_is_rx_byte_available(RS232_CBlk *p)
{
 int i=0;
    
    switch( p->PortNumber )
 	{
	 	case USARTA:
            if (!UART1_ReceiveBufferIsEmpty())
                i = 1;
            break;
        case USARTB:
            if (!UART2_ReceiveBufferIsEmpty())
                i = 1;
            break;
	 		
	 	case USART_USB:
	 		i = USB_USART_GetRxByteCnt();
	 		break;
	 	
	 	default:
	 		i = 0;
	 		break;
	}
	return(i);
}

uint8 _rs232_get_next_rx_byte(RS232_CBlk *p)
{
 uint8 i;
 
    switch( p->PortNumber )
 	{
	 	case USARTA:
            i = USARTA_READ_BYTE();
            break;
        case USARTB:
            i = USARTB_READ_BYTE();
	 		break;
	 		
	 	case USART_USB:
	 		i = USARTUSB_READ_BYTE();
	 		break;
	 	
	 	default:
	 		i = 0;
	 		break;
	}
	return(i);
}

/////////////////////////////////////////////////////////////////////////
// uint8 _rs232_proc_rx_byte( RS232_CBlk *p , int8 signal)
//  @Summary
//    Read a byte of data from the UART1, UART2, or USB UART and
//    process it in the context of a NMEA 0183 data stream.
//
//  @Description
//    Read a byte of data from the UART1, UART2, or USB UART and
//    process it in the context of a NMEA 0183 data stream. Assumes
//    that it's being called from the RS232 task and needs to preserve
//    the task signal that's being passed in and returned. Also assumes
//    that a byte is available to process.
//
//  @Preconditions
//    See description above
//
//  @Param
//    None.
//
//  @Returns
//    RS232 task signal
//
/////////////////////////////////////////////////////////////////////////
int8 _rs232_proc_rx_byte( RS232_CBlk *p )
{
 uint8 inChar,i;
 int8 RtnSignal;
 
    RtnSignal = 0;
 
    //inChar = *p->RxRegPtr; // read the USART
    inChar = _rs232_get_next_rx_byte(p); // read the USART buffer
    

    p->RxChkSum ^= inChar;

    if (p->RxState > RX_CMD_MAX_STATE)
    {
        p->RxState = RX_CMD_WAITING;
    }
    
    if (p->RxState != RX_CMD_WAITING)
        p->RxBytes++;

    switch (p->RxState)
    {
        // RX_CMD_WAITING: wait for the start of a msg   
        case RX_CMD_WAITING:

            if((char)inChar == PROTOCOL_START)
            {
                RS232_new_msg_init( p );
            }
            break;

        // RX_CMD_HDR_CHK: Fills the header structure and then calls the clients header
        // checker in RS232_cnfg.h
        case RX_CMD_HDR_CHK:

            if((char)inChar == PROTOCOL_START)
            {
                RS232_new_msg_init( p );
                break;
            }
            
            if (( inChar == PROTOCOL_DELIM ) && p->HdrCnt)
            {
                p->RxHdr[ p->HdrCnt ] = 0; // terminate
                i = RS232_HDRCHK_FUNC( p->RxHdr );
                p->RxHdrIdx = i;

                for (i=0; i<p->HdrCnt; i++)
                {
                    RS232_write_rx_buf( p, p->RxHdr[ i ]);
                }
              
                p->RxState = RX_CMD_RECEIVING;    // Start of a msg, next state
                RS232_write_rx_buf( p, 0 );
                RS232_next_token( p );
            }
            else
            {
                if ( p->HdrCnt < RS232_HDR_SIZE )
                    p->RxHdr[ p->HdrCnt++ ] = inChar;
            }

            break;

        // store non-delim chars in RS232_RxBuf char in the command
        case RX_CMD_RECEIVING:

            switch ((char)inChar)
            {
                case PROTOCOL_DELIM:

                    RS232_write_rx_buf( p, 0 );
                    RS232_next_token( p );
                    break;

                case PROTOCOL_STOP:

                    p->RxChkSum ^= inChar; // Reverse PROTOCOL_STOP XOR
                    p->RxChkSumChars[0] = HexToAscii[ (p->RxChkSum >> 4) & 0x0F ];
                    p->RxChkSumChars[1] = HexToAscii[ (p->RxChkSum) & 0x0F ];

                    p->RxBuf[ p->RxBufIdx ] = 0; //terminate the last Token

                    p->RxState = RX_CMD_CHECKSUM0;
                    break;

                case PROTOCOL_START:

                    RS232_new_msg_init( p );
                    break;

                default:

                    if ( p->RxBufCnt > MAX_RX_CHARS )
                    {
                        RS232_Xmt_Error_Msg( p->PortNumber, CMD_ERR_BAD_ARGS );
                        p->RxState = RX_CMD_WAITING;
                    }
                    // Write the token chars to the receive buffer
                    RS232_write_rx_buf( p, (uint8)inChar );
                    break;
            }//switch
            break;


        case RX_CMD_CHECKSUM0:	

            // Check the first checksum byte               				
            if (inChar == p->RxChkSumChars[0])
            {
                p->RxState = RX_CMD_CHECKSUM1;
            }
            else
            {
                p->RxState = RX_CMD_WAITING;
            }
            break;

        case RX_CMD_CHECKSUM1:

#undef BUG_0            
#ifdef BUG_0
            if (p->PortNumber == USARTA)
            {
                p->RxState = RX_CMD_WAITING;
            }
#endif            
            p->RxState = RX_CMD_WAITING;

            if (inChar == p->RxChkSumChars[1])
            {
                p->RxMsgs++;
                if ( _rs232_msg_enabled( p ) )             
                {
                    if (!RS232_Client_Rdy( p ))
                    {
                        p->RxClientBusyErrs++;
                    }
                    else
                    {
                        RS232_Signal_Client( p );
                    }
                }		
                else
                {
                    p->RxUnMatchedMsgs++;
                }					
                p->RxMsgTokenStart = p->RxTokenStart;
                p->RxMsgTokenCnt = p->RxTokenCnt;
                p->RxMsgHdrIdx = p->RxHdrIdx;
                RtnSignal |= RS232_Signal_Switching( p );

                p->FillBuffer = NextRxBuffer( p->FillBuffer );
            }	
            else
                p->RxPFErrs++;
            
            break;

    }//switch

    return( RtnSignal );
}

uint8 RS232_GetRxRemappedMsgHdrIdx( uint8 UsartIdx )
{
 RS232_CBlk *p;
  	
  	p = _rs232_get_port_cblk( UsartIdx );  
    return( rs232_msgs[p->RxMsgHdrIdx].msg_remap );
}

uint8 RS232_GetRxMsgHdrIdx( uint8 UsartIdx )
{
 RS232_CBlk *p;
  	
  	p = _rs232_get_port_cblk( UsartIdx );  
    return( p->RxMsgHdrIdx );
}
 


uint8 _rs232_tx_buf_full( RS232_CBlk *p )
{
 uint8 i=1;
 
    switch (p->PortNumber)
    {
        case USARTA:
            i = UART1_TransmitBufferIsFull();
            //if ((i) && UART1_GetTxDone())
            //    UART1_Enable_Tx_Int();            
            break;
        case USARTB:
            i = UART2_TransmitBufferIsFull();
            //if ((i)  && UART2_GetTxDone())
            //    UART2_Enable_Tx_Int();
            break;
        case USART_USB:
            i = USB_USART_TransmitBufferIsFull();
            break;
            
        default:
            break;
    }
    return(i);
}

/////////////////////////////////////////////////////////////////////////
// uint8 _rs232_proc_tx_buf( RS232_CBlk *p )
//
// Checks for bytes to send and then if UxTXREG can take a byte before
// writing the next byte to UxTXREG
/////////////////////////////////////////////////////////////////////////
uint8 _rs232_proc_tx_buf( RS232_CBlk *p )
{
 uint8 c;

	while ( ( p->TxByteCnt > 0 ) && ( !_rs232_tx_buf_full( p )))
	{
		c = p->TxBuf[ p->TxBufRdIdx++ ];
		//*p->TxRegPtr = c;
        switch (p->PortNumber)
        {
            case USARTA:
                UART1_Write( c );
                break;
            case USARTB:
                UART2_Write( c );
                break;
            case USART_USB:
                USB_USART_Write( c );
                break;
            default:
                return( 0 );
        }
		p->TxByteCnt--;
        p->TxBytes++;
		p->TxBufRdIdx &= (TX_BUF_SIZE-1);
	} 
    return( p->TxByteCnt );
}

////////////////////////////////////////////////////////////////////////
// void _rs232_put_xmt_char( RS232_CBlk *p, char c )
//
//
/////////////////////////////////////////////////////////////////////////
void _rs232_put_xmt_char( RS232_CBlk *p, char c )
{
	if (p->TxByteCnt < TX_BUF_SIZE-1)
	{
		p->TxBuf[ p->TxBufWtIdx++ ] = c;
		p->TxByteCnt++;
		p->TxBufWtIdx &= (TX_BUF_SIZE-1);
	}
	else
	{
		p->TxBufOverFlowErrs++;
	}
	//_rs232_proc_tx_buf( p );
}

////////////////////////////////////////////////////////////////////////
// uint16 _rs232_get_tx_buf_free_bytes( RS232_CBlk *p )
//
//
/////////////////////////////////////////////////////////////////////////
uint16 _rs232_get_tx_buf_free_bytes( RS232_CBlk *p )
{
	return( TX_BUF_SIZE - p->TxByteCnt );
}

////////////////////////////////////////////////////////////////////////
// void _rs232_put_xmt_buf( char c )
//
//
/////////////////////////////////////////////////////////////////////////
void _rs232_put_xmt_buf( RS232_CBlk *p, char *ptr )
{
	while ((*ptr) && (p->TxByteCnt < TX_BUF_SIZE-1))
	{	
		p->TxBuf[ p->TxBufWtIdx++ ] = *ptr++;
		p->TxByteCnt++;	
		p->TxBufWtIdx &= (TX_BUF_SIZE-1);
	}
	
	if (p->TxByteCnt >= TX_BUF_SIZE-1)
	{
		p->TxBufOverFlowErrs++;
	}
	//_rs232_proc_tx_buf( p );
}
  	
/////////////////////////////////////////////////////////////////
// void RS232_Xmt_Msg( uint8 UsartIdx, char *MsgPtr )
//
// Sends a msg out the spec'd USART
// The message is framed with PROTOCOL_START & PROTOCOL_STOP
// UsartIdx is either USARTA or USARTB
// MsgPtr points at the unframed 0-terminated message to send
/////////////////////////////////////////////////////////////////
void RS232_Xmt_Msg( uint8 UsartIdx, char *MsgPtr )
{
 RS232_CBlk *p;
 char *cs;
  	
  	p = _rs232_get_port_cblk( UsartIdx );
 
 	if ((strlen( MsgPtr ) + 7) <= _rs232_get_tx_buf_free_bytes( p ) )
 	{
    	_rs232_put_xmt_char( p, PROTOCOL_START );
    	_rs232_put_xmt_buf( p, MsgPtr );
    	_rs232_put_xmt_char( p, PROTOCOL_STOP );
    	cs = RS232_MakeChkSum( MsgPtr );
    	_rs232_put_xmt_char( p, cs[0] );
    	_rs232_put_xmt_char( p, cs[1] );
        
        // append CR LF
        _rs232_put_xmt_char( p, 0x0D );
        _rs232_put_xmt_char( p, 0x0A );
    
    	p->TxMsgs++;
  	}
  	else
    {
  		p->TxBufOverFlowErrs++;
    }
    _rs232_proc_tx_buf( p );
}

//////////////////////////////////////////////////////////////////////////////
// void RS232_Xmt_Error_Msg( uint8 UsartIdx, uint8 ErrorCode )
//
// Sends an error msg with the spec'd ErrorCode
// UsartIdx is either USARTA or USARTB
/////////////////////////////////////////////////////////////////////////////
void RS232_Xmt_Error_Msg( uint8 UsartIdx, uint8 ErrorCode )
{
 char ErrorCodeStr[4];
 char ErrorCodeMsgStr[20];
 
 RS232_CBlk *p;
 char *cs;
 
 	p = _rs232_get_port_cblk( UsartIdx );
	
	
 	if ( _rs232_get_tx_buf_free_bytes( p ) >= 14 )
 	{
		strncpy(ErrorCodeMsgStr, ErrorCodeToken, 3);
		ErrorCodeMsgStr[3] = PROTOCOL_DELIM;
		ErrorCodeMsgStr[4] = 0;
	
		int8_to_ascii10( ErrorCode,ErrorCodeStr );
		strcat( ErrorCodeMsgStr, ErrorCodeStr );

		_rs232_put_xmt_char( p, PROTOCOL_START );
		_rs232_put_xmt_buf( p, ErrorCodeMsgStr );
		_rs232_put_xmt_char( p, PROTOCOL_STOP );
	
		cs = RS232_MakeChkSum( ErrorCodeMsgStr );
    	_rs232_put_xmt_char( p, cs[0] );
    	_rs232_put_xmt_char( p, cs[1] );
	}
	else
	{
		p->TxBufOverFlowErrs++;
	}
    _rs232_proc_tx_buf( p );
}

//////////////////////////////////////////////////////////////////////
// char *RS232_GetToken( uint8 UsartIdx, uint8 msg_fld )
//
// Returns a pointer to the spec'd token
// UsartIdx is either USARTA or USARTB
// msg_fld is 0->MAX_RX_TOKENS-1
//////////////////////////////////////////////////////////////////////
char *RS232_GetToken( uint8 UsartIdx, uint8 msg_fld )
{
 char *tptr;
 uint8 i;
 RS232_CBlk *p;
 
 	p = _rs232_get_port_cblk( UsartIdx );

	i = AddModulo( p->RxMsgTokenStart, msg_fld, MAX_RX_TOKENS*MAX_RX_BUFFERS );
	tptr = (char *)(p->RxTokens[ i ]);
 
	return( tptr );
}

//////////////////////////////////////////////////////////////////////
// uint8 RS232_GetTokenCnt( uint8 UsartIdx )
//
// Returns the token count of the message waiting
// at UsartIdx which is either USARTA or USARTB
//////////////////////////////////////////////////////////////////////
uint8 RS232_GetTokenCnt( uint8 UsartIdx )
{
 RS232_CBlk *p;
 
 	p = _rs232_get_port_cblk( UsartIdx );
  
	return( p->RxMsgTokenCnt );
}

////////////////////////////////////////////////////////////////////////
// RS232 A Receiver Interrupt
//
//
/////////////////////////////////////////////////////////////////////////
void RS232_A_Rx_Isr (void)
{	
    //usart_rx_tx_buf_status.s.usart_a_rx_bytes = 1;
    SignalTask( PROC_RS232_TASK, SGNL_USART_RX_BYTES_BIT);
}

////////////////////////////////////////////////////////////////////////
// RS232 B Receiver Interrupt
//
//
/////////////////////////////////////////////////////////////////////////
void RS232_B_Rx_Isr (void)
{	
    //usart_rx_tx_buf_status.s.usart_b_rx_bytes = 1;
    SignalTask( PROC_RS232_TASK, SGNL_USART_RX_BYTES_BIT);
}

////////////////////////////////////////////////////////////////////////
// RS232 B Receiver Interrupt
//
//
/////////////////////////////////////////////////////////////////////////
void RS232_USB_Rx_Isr (void)
{	
    //usart_rx_tx_buf_status.s.usart_usb_rx_bytes = 1;
    SignalTask( PROC_RS232_TASK, SGNL_USART_RX_BYTES_BIT);
}

#define TX_DIRECT
////////////////////////////////////////////////////////////////////////
// RS232 A Transmitter Interrupt
//
//
/////////////////////////////////////////////////////////////////////////
void RS232_A_Tx_Isr(void)
{
#ifdef TX_DIRECT
    _rs232_proc_tx_buf( &RS232_a );
#else
	if (RS232_a.TxByteCnt > 0)
    {
        usart_rx_tx_buf_status.s.usart_a_tx_bytes = 1;
        SignalTask( PROC_RS232_TASK, SGNL_USART_TX_BYTES_BIT);
    }
#endif
}

////////////////////////////////////////////////////////////////////////
// RS232 B Transmitter Interrupt
//
//
/////////////////////////////////////////////////////////////////////////
void RS232_B_Tx_Isr(void)
{
#ifdef TX_DIRECT
    _rs232_proc_tx_buf( &RS232_b );
#else
	if (RS232_b.TxByteCnt > 0)
    {
        usart_rx_tx_buf_status.s.usart_b_tx_bytes = 1;
        SignalTask( PROC_RS232_TASK, SGNL_USART_TX_BYTES_BIT);
    }
#endif
}

////////////////////////////////////////////////////////////////////////
// RS232 B Transmitter Interrupt
//
//
/////////////////////////////////////////////////////////////////////////
void RS232_USB_Tx_Isr(void)
{
#ifdef TX_DIRECT
    _rs232_proc_tx_buf( &RS232_usb );
#else
	if (RS232_usb.TxByteCnt > 0)
    {
        usart_rx_tx_buf_status.s.usart_usb_tx_bytes = 1;
        SignalTask( PROC_RS232_TASK, SGNL_USART_TX_BYTES_BIT);
    }
#endif
}
#ifdef USE_CNFG_MSG
/////////////////////////////////////////////////////////////////////////
// void RS232_CnfgMsg( int8 msg_index, uint8 enabled, uint8 looped, uint8 switched  )
//
// Configures a messasge. 
// uint msgIndex;
//	#define RS232_MSG_XXHDM 0
//	#define RS232_MSG_XXRMB 1
//	#define RS232_MSG_XXRMC 2
//	#define RS232_MSG_PMTTS 3
//	#define RS232_MSG_PMTST 4
//	#define RS232_MSG_PMKT001 5
//	#define RS232_MSG_XXHDG 6
//	#define RS232_MSG_APHDG 7
//	#define MAX_RS232_MSGS 8
//
//	uint8 enabled;
//		TRUE/FALSE. If TRUE then processed
//	uint8 looped;
//		TRUE/FALSE. If TRUE then the msg is looped back out the port. If FALSE,
//					then msg isn't looped back
//	uint8 switched;
//		TRUE/FALSE. If TRUE then the msg is looped back out the other port. If FALSE,
//					then msg isn't looped back to the other port
//
/////////////////////////////////////////////////////////////////////////
void RS232_CnfgMsg( int8 msg_index, uint8 enabled, uint8 do_not_loop, uint8 switched  )
{
	if (msg_index < MAX_RS232_MSGS)
	{
		rs232_msgs[ (int)msg_index ].enabled = enabled;
		rs232_msgs[ (int)msg_index ].do_not_loop = do_not_loop;
		rs232_msgs[ (int)msg_index ].is_switchable = switched;
	}
}
#endif
/////////////////////////////////////////////////////////////////////////
// char *RS232_GetStats( int8 Clear, char *p  )
//
// Returns a report of errors for USARTA and USARTB.
// Stats get cleared if Clear is non-zero.
// The report is composed of eight numbers coverted to
// ascii separated by commas.
//
// UsartA: RxOErrs, RxFErrs, RxPErrs, RxPFErrs, CodeErrs, TxBufOvrErrs, RxBufOvrErrs, TxPkts, RxPkts
// UsartB: RxOErrs, RxFErrs, RxPErrs, RxPFErrs, CodeErrs, TxBufOvrErrs, RxBufOvrErrs, TxPkts, RxPkts
//
// char *Core_IntToAsciiStr( int16 i, char *str, int8 min_digs )
/////////////////////////////////////////////////////////////////////////
char *RS232_GetStats( uint8 UsartIdx, int8 Clear, char *buf )
{
 RS232_CBlk *p;

	p = _rs232_get_port_cblk( UsartIdx );

    switch ( UsartIdx ) {
        
        case USARTA:
            UART1_GetStats( Clear, buf );
            break;
            
        case USARTB:
            UART2_GetStats( Clear, buf );
            break;
        
        default:
        case USART_USB:
            _stat_append( 0, buf, TRUE,2 );
            _stat_append( 0, buf, TRUE,2 );
            _stat_append( 0, buf, TRUE,2 );
            break;
    }
    
	_stat_append( (unsigned long) p->RxPFErrs, buf, TRUE,2 );
	_stat_append( (unsigned long) p->CodeErrs, buf, TRUE,2 );
	_stat_append( (unsigned long) p->TxBufOverFlowErrs, buf, TRUE,3 );
	_stat_append( (unsigned long) p->RxBufOverFlowErrs, buf, TRUE,3 );
	_stat_append( (unsigned long) p->TxMsgs, buf, TRUE,3 );
	_stat_append( (unsigned long) p->RxMsgs, buf, TRUE,3 );
	_stat_append( (unsigned long) p->TxBytes, buf, TRUE,7 );
	_stat_append( (unsigned long) p->RxBytes, buf, TRUE,7 );    
	_stat_append( (unsigned long) p->RxClientBusyErrs, buf, FALSE,3 );

	if (Clear)
	{
		p->RxPFErrs = 0;
		p->CodeErrs = 0;
		p->TxBufOverFlowErrs = 0;
		p->RxBufOverFlowErrs = 0;
		p->RxMsgs = 0;
		p->TxMsgs = 0;
		p->RxBytes = 0;
		p->TxBytes = 0; 
        p->RxClientBusyErrs = 0;
	}  	
 	return( buf );
}

//////////////////////////////////////////////////////////////////////
// void RS232_UpdateSwitching()
//
// Typically called from button navigation land so as to
// propagate the switching settings into the RS232 control
// structure
// 
//////////////////////////////////////////////////////////////////////
void RS232_UpdateSwitching()
{
	if (Btn_GetParm( BTN_PARM_XNG_LOOP_A_IDX ))
        RS232_a.switch_cnfg.s.rx_loop = TRUE;
	else
		RS232_a.switch_cnfg.s.rx_loop = FALSE;
		
	if (Btn_GetParm( BTN_PARM_XNG_A2B_IDX ))
		RS232_a.switch_cnfg.s.switch_to_usart_b = TRUE;
	else
		RS232_a.switch_cnfg.s.switch_to_usart_b = FALSE;
    
    if (Btn_GetParm( BTN_PARM_XNG_A2U_IDX ))
		RS232_a.switch_cnfg.s.switch_to_usart_usb = TRUE;
	else
		RS232_a.switch_cnfg.s.switch_to_usart_usb = FALSE;    
		
	if (Btn_GetParm( BTN_PARM_XNG_LOOP_B_IDX ))
		RS232_b.switch_cnfg.s.rx_loop = TRUE;
	else
		RS232_b.switch_cnfg.s.rx_loop = FALSE;
		
	if (Btn_GetParm( BTN_PARM_XNG_B2A_IDX ))
		RS232_b.switch_cnfg.s.switch_to_usart_a = TRUE;
	else
		RS232_b.switch_cnfg.s.switch_to_usart_a = FALSE;
    
    if (Btn_GetParm( BTN_PARM_XNG_B2U_IDX ))
		RS232_b.switch_cnfg.s.switch_to_usart_usb = TRUE;
	else
		RS232_b.switch_cnfg.s.switch_to_usart_usb = FALSE;
    
    if (Btn_GetParm( BTN_PARM_XNG_LOOP_USB_IDX ))
		RS232_usb.switch_cnfg.s.rx_loop = TRUE;
	else
		RS232_usb.switch_cnfg.s.rx_loop = FALSE;
    
    	if (Btn_GetParm( BTN_PARM_XNG_U2A_IDX ))
		RS232_usb.switch_cnfg.s.switch_to_usart_a = TRUE;
	else
		RS232_usb.switch_cnfg.s.switch_to_usart_a = FALSE;
    
    if (Btn_GetParm( BTN_PARM_XNG_U2B_IDX ))
		RS232_usb.switch_cnfg.s.switch_to_usart_b = TRUE;
	else
		RS232_usb.switch_cnfg.s.switch_to_usart_b = FALSE;
}

//////////////////////////////////////////////////////////////////////
// void RS232_UpdatePortASpeed()
//
// Typically called from button navigation land so as to
// propagate the port speed settings into the RS232 control
// structure
// 
//////////////////////////////////////////////////////////////////////
void RS232_UpdatePortASpeed()
{
    uint16 brg_init;
    
	switch (Btn_GetParm( BTN_PARM_USART_A_SPEED_IDX ))
	{
		case 0:
			brg_init = (uint16)USART_BRG_4800_INIT;
			break;
		case 1:
			brg_init = (uint16)USART_BRG_9600_INIT;
			break;
		case 2:
			brg_init = (uint16)USART_BRG_19200_INIT;
			break;
		case 3:
			brg_init = (uint16)USART_BRG_38400_INIT;
			break;
		default:
            return;
	}
    UART1_Initialize_Port_Speed(brg_init);
}

//////////////////////////////////////////////////////////////////////
// void RS232_UpdatePortBSpeed()
//
// Typically called from button navigation land so as to
// propagate the port speed settings into the RS232 control
// structure
// 
//////////////////////////////////////////////////////////////////////
void RS232_UpdatePortBSpeed()
{
uint16 brg_init;
    
	switch (Btn_GetParm( BTN_PARM_USART_B_SPEED_IDX ))
	{
		case 0:
			brg_init = (uint16)USART_BRG_4800_INIT;
			break;
		case 1:
			brg_init = (uint16)USART_BRG_9600_INIT;
			break;
		case 2:
			brg_init = (uint16)USART_BRG_19200_INIT;
			break;
		case 3:
			brg_init = (uint16)USART_BRG_38400_INIT;
			break;
		default:
            return;
	}
    UART2_Initialize_Port_Speed(brg_init);
}

//////////////////////////////////////////////////////////////////////
// char *RS232_MakeChkSum( char *ptr )
//
// Calculates the checksum of the buffer ptr[]
// and returns a pointer to the checksum
// 
//////////////////////////////////////////////////////////////////////
char *RS232_MakeChkSum( char *ptr )
{
 static char ChkSum[3];
 uint8 cs, i;
 
 	for( cs = 0, i=0; (*ptr != 0) && (i < RX_BUFFER_SIZE); ptr++, i++)
 	{
	 	//if ( *ptr != PROTOCOL_DELIM )
	 		cs ^= *ptr;	
	}
 
 	ChkSum[0] = HexToAscii[ (cs >> 4) & 0x0F ];
 	ChkSum[1] = HexToAscii[ (cs) & 0x0F ];
 	ChkSum[2] = 0;
 		
 	return( ChkSum );
}

//////////////////////////////////////////////////////////////////////
// char *RS232_GetMsgCopy( int UsartIdx, uint8 *p )
//
// Returns a copy of the last msg
// at UsartIdx which is either USARTA or USARTB
//////////////////////////////////////////////////////////////////////
#define FAST_MSG_COPY
char *RS232_GetMsgCopy( int UsartIdx, char *dest_msg_buf )
{
 int token_idx=0, token_cnt;
 char *cptr, *dst_p;
 RS232_CBlk *pp;

	pp = _rs232_get_port_cblk( UsartIdx );

	token_cnt = RS232_GetTokenCnt( UsartIdx );
    dst_p = dest_msg_buf;
	if ( token_cnt )
	{
        cptr = RS232_GetToken( UsartIdx, token_idx );
        while( token_idx < token_cnt )
        {
            if (*cptr == 0) 
            {
                *dst_p++ = PROTOCOL_DELIM;
                token_idx++; cptr++;
            }
            else               
                *dst_p++ = *cptr++;
        }
        dst_p[-1] = 0;  // step on the last protocol delimiter
		
		return( dest_msg_buf );
	}
	return(0);
}

//////////////////////////////////////////////////////////////////////
// void _rs232_switch_msg_copy( RS232_CBlk *p )
//
// Gets a copy of the message currently being processed
// for that port and optionally switches it out of either
// or both ports 
//////////////////////////////////////////////////////////////////////
void _rs232_switch_msg_copy( RS232_CBlk *p )
{
 RS232_Switch_CBlk *swp;
 char *msg_ptr;
 
 	swp = p->switch_ptr;
 	msg_ptr = RS232_GetMsgCopy( p->PortNumber, swp->SwitchBuf );
    
    if ((p->switch_cnfg.s.rx_loop))
 	{
        if (p->RxMsgHdrIdx < MAX_RS232_MSGS)
        {
            if (!rs232_msgs[ p->RxHdrIdx ].do_not_loop)
            {
                RS232_Xmt_Msg( p->PortNumber, msg_ptr );
            }      
        }     
	}
	
    if (p->switch_cnfg.s.switch_to_usart_a) 
    {
        if (p->RxHdrIdx < MAX_RS232_MSGS) 
        {
            if (rs232_msgs[ p->RxHdrIdx ].is_switchable)
            {         
                RS232_Xmt_Msg( USARTA, msg_ptr );
            }
        }     
    }     
    
    if (p->switch_cnfg.s.switch_to_usart_b) 
    {
        if (p->RxHdrIdx < MAX_RS232_MSGS) 
        {
            if (rs232_msgs[ p->RxHdrIdx ].is_switchable)
            {         
                RS232_Xmt_Msg( USARTB, msg_ptr );
            }
        }     
    }     

    if (p->switch_cnfg.s.switch_to_usart_usb) 
    {
        if (p->RxHdrIdx < MAX_RS232_MSGS) 
        {
            if (rs232_msgs[ p->RxHdrIdx ].switch_to_usb)
            {         
                RS232_Xmt_Msg( USART_USB, msg_ptr );
            }
        }
    }
	swp->SwitchBufIdx = 0xFF;   // buffer is available
}

//////////////////////////////////////////////////////////////////////
// void RS232_InitTask()
//
// Initializes the RS232 task
// 
//////////////////////////////////////////////////////////////////////
void RS232_InitTask()
{

}

//////////////////////////////////////////////////////////////////////
// int8 RS232_ProcTask( int8 signal )
//
// The RS232 task gets signalled when a valid packet arrives.
// The packet is then switched to either/both/none ports
// 
//////////////////////////////////////////////////////////////////////

int8 RS232_ProcTask( int8 signal )
{
 int8 RtnSignal = signal;

 	switch( RS232_task_state )
 	{
        case 0:
            
            RS232_task_state++;
            if (signal & SGNL_USART_RX_BYTES_MASK)
            {
                if (USART_USB_RX_RDY)
                {
                    // _rs232_proc_rx_byte() could potentially signal RS232_Task
                    // so we need to preserve signal bits when calling it.
                    RtnSignal |= _rs232_proc_rx_byte( &RS232_usb );
                    break;
                }
            }
            else
            {
                RS232_task_state = 3;
                break;
            }
            
        case 1:
            RS232_task_state++;
            if ( USART_A_RX_RDY )
            {
                RtnSignal |= _rs232_proc_rx_byte( &RS232_a  );
                break;
            }
            
        case 2:
            RS232_task_state++;
            if ( USART_B_RX_RDY )
            {
                RtnSignal |= _rs232_proc_rx_byte( &RS232_b );
                break;
            }     

	 	case 3:
            RS232_task_state++;
            
            if ((!USART_A_RX_RDY)&&(!USART_B_RX_RDY)&&(!USART_USB_RX_RDY))
            {
                RtnSignal &= _SGNL_USART_RX_BYTES_MASK;
            }
            
            if (!(signal & (SGNL_USARTA_SWITCH_MASK | SGNL_USARTB_SWITCH_MASK | SGNL_USART_USB_SWITCH_MASK)))
            {
                RS232_task_state = 0;
                break;
            }
            
			if (signal & SGNL_USARTA_SWITCH_MASK)
			{
				_rs232_switch_msg_copy( &RS232_a );
				RtnSignal &= _SGNL_USARTA_SWITCH_MASK;
                break;
			}
            
		case 4:
            RS232_task_state++;
            
			if (signal & SGNL_USARTB_SWITCH_MASK)
			{
				_rs232_switch_msg_copy( &RS232_b );
				RtnSignal &= _SGNL_USARTB_SWITCH_MASK;
                break;
			}
			
        case 5:
            RS232_task_state = 0;
			if (signal & SGNL_USART_USB_SWITCH_MASK)
			{
				_rs232_switch_msg_copy( &RS232_usb );
				RtnSignal &= _SGNL_USART_USB_SWITCH_MASK;
			}
			break;
	}
	return( RtnSignal );
}

