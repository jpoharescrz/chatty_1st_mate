#ifndef TTS_UTILS_H
#define TTS_UTILS_H
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

#define TTS_IO_PROTOCOL_START '$'
#define TTS_IO_PROTOCOL_STOP '*'
#define TTS_IO_PROTOCOL_DELIM ','

#define TTS_IO_MAX_TOKENS 15
#define TTS_IO_MAX_CHARS 75

#define TTS_IO_BUF_SIZE 64
#define TTS_IO_MAX_CMDS 6

#define TTS_IO_RX_CMD_WAITING 0
#define TTS_IO_RX_CMD_RECEIVING TTS_IO_RX_CMD_WAITING+1

#define TTS_IO_TX_CMD_WAITING 0
#define TTS_IO_TX_CMD_RECEIVING TTS_IO_TX_CMD_WAITING+1

#define TTS_IO_CMD_WAITING 0
#define TTS_IO_CMD_RECEIVING TTS_IO_CMD_WAITING+1

typedef struct {
	uint8	TokenIdx;
	uint8	TokenCnt;
	int		MsgByteCnt;
} MsgTknRec;



typedef struct {
	uint8		TokenCnt;
	uint8		TokenCnt_BU;
	uint8		CurTokenCnt;
	uint8		TokenRdIdx;
	uint8		TokenWtIdx;
	uint8		*Tokens[ TTS_IO_MAX_TOKENS ];
	
	uint8 		MsgCnt;
	uint8		MsgTokenWtIdx;
	uint8		MsgTokenRdIdx;
	MsgTknRec	MsgTokens[ TTS_IO_MAX_CMDS ];
	
	uint8		BufByteCnt;
	uint8		BufByteCnt_BU;
	uint8		BufWtIdx;
	uint8		BufWtIdx_BU;
	uint8		BufRdIdx;
	uint8		Buf[ TTS_IO_BUF_SIZE ];
	
	uint8		InState;
	uint8		InByte;
	uint8		BadEvents;
	uint8		DroppedPkts;
	
	uint8		TgtTask;
	uint8		TgtTaskSignal;
	
} TTS_IO_CtrlBlkRec;

#ifdef TTS_UTILS
TTS_IO_CtrlBlkRec TTS_IO_InCtrlBlk, TTS_IO_OutCtrlBlk;
#else
extern TTS_IO_CtrlBlkRec TTS_IO_InCtrlBlk, TTS_IO_OutCtrlBlk;
#endif

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

#endif


