#ifndef RS232_API_H
#define RS232_API_H

#include "rs232_cnfg.h"
#include "rs232_data_defs.h"

void RS232_A_IntEnable();
void RS232_B_IntEnable();
void RS232_A_IntDisable();
void RS232_B_IntDisable();
void RS232_Xmt_Msg( uint8 UsartIdx, char *MsgPtr );
void RS232_Xmt_Error_Msg( uint8 UsartIdx, uint8 ErrorCode );
char *RS232_GetToken( uint8 UsartIdx, uint8 msg_fld );
uint8 RS232_GetTokenCnt( uint8 UsartIdx );
char *RS232_GetMsgHdrStr( uint8 MsgIdx, char *p);
uint8 RS232_GetRxRemappedMsgHdrIdx( uint8 UsartIdx );
uint8 RS232_GetRxMsgHdrIdx( uint8 UsartIdx );

char *RS232_GetStats( uint8 usartidx, int8 Clear, char *buf );
void RS232_SetSwitching( uint8 Port, uint8 Parms );
void RS232_UpdateSwitching();
char *RS232_GetMsgCopy( int UsartIdx, char *p );

int8 RS232_ProcTask( int8 signal );
void  RS232_Task_Init();
void RS232_UpdatePortASpeed();
void RS232_UpdatePortBSpeed();

void RS232_FreeBuffer( uint8 UsartIdx, uint8 task );

void RS232_A_Rx_Isr (void);
void RS232_B_Rx_Isr (void);
void RS232_USB_Rx_Isr (void);

void RS232_A_Tx_Isr(void);
void RS232_B_Tx_Isr(void);
void RS232_USB_Tx_Isr(void);

void RS232_load_msg_options();

#endif
