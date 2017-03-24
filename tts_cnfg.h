/////////////////////////////////////////////////////////////////////////
////                          tts_cnfg.h                              
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

#define TTS_FORCE_HI_PRIORITY 1
#define TTS_NO_CHANNELS 1

