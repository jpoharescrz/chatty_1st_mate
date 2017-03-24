
#ifndef RS232_DATA_DEFS_H
#define RS232_DATA_DEFS_H

//#include "ADSC_LITE_0.X/mcc_generated_files/uart1.h"
//#include "ADSC_LITE_0.X/mcc_generated_files/uart2.h"

//#define CR 0x0D
//#define LF 0x0A

// MAX_RX_BUFFERS should be at least 2
#define MAX_RX_BUFFERS 2
#define BUFFER0 0
#define BUFFER1 1

#ifdef _BRGH_0
#define USART_BRG_4800_INIT ((FCY/(16 * 4800L))-1)
#define USART_BRG_9600_INIT ((FCY/(16 * 9600L))-1)
#define USART_BRG_19200_INIT ((FCY/(16 * 19200L))-1)
#define USART_BRG_38400_INIT ((FCY/(16 * 38400L))-1)
#else
#define USART_BRG_4800_INIT ((FCY/(4 * 4800L))-1)
#define USART_BRG_9600_INIT ((FCY/(4 * 9600L))-1)
#define USART_BRG_19200_INIT ((FCY/(4 * 19200L))-1)
#define USART_BRG_38400_INIT ((FCY/(4 * 38400L))-1)
#endif

#define RX_CMD_WAITING 0
#define RX_CMD_HDR_CHK RX_CMD_WAITING+1
#define RX_CMD_HDR_CHK_DELIM RX_CMD_WAITING+2
#define RX_CMD_RECEIVING RX_CMD_WAITING+3
#define RX_CMD_CHECKSUM0 RX_CMD_WAITING+4
#define RX_CMD_CHECKSUM1 RX_CMD_WAITING+5
#define RX_CMD_MAX_STATE RX_CMD_CHECKSUM1

typedef union
{
    struct
    {
            uint8 rx_loop:1;
            uint8 switch_to_usart_a:1;
            uint8 switch_to_usart_b:1;
            uint8 switch_to_usart_usb:1;         
            uint8 reserved:4;
    }s;
    uint8 status;
}
uart_switch_cnfg;

typedef struct RS232_Switch_Ctl_Blk {
	
char  SwitchBuf[ RX_BUFFER_SIZE ];
uint8 SwitchBufIdx;

}RS232_Switch_CBlk;

typedef struct RS232_Ctl_Blk {
	
uint8 FillBuffer;
uint8 buffer_rsrc_lock[ MAX_RX_BUFFERS ];
uint8 PortNumber;           //USART_PORT_A, USART_PORT_B, ...
uint8 PortSgnlNumber;       // SGNL_USARTA_BIT, SGNL_USARTB_BIT, SGNL_USART_USB_BIT

uint8 RxState;
uart_switch_cnfg switch_cnfg;
uint8 RxLoop;		//True or False
uint8 RxSwitch;		//Switches msgs wholesale to ports
struct RS232_Ctl_Blk *SwitchPortCBlk;
RS232_Switch_CBlk	*switch_ptr;
uint8 RxSwitchToUsb; //Switches pkts to USB for monitoring
uint8 RxTokenCnt;
uint8 RxMsgTokenCnt;
uint8 RxTokenIdx;
uint8 RxTokenStart;
uint8 RxMsgTokenStart;
uint8 *RxTokens[ RX_TOKEN_BUF_SIZE * MAX_RX_BUFFERS ];

uint8 RxBufIdx;
uint8 RxBufCnt;
uint8 RxChkSum;
char  RxChkSumChars[2];
uint8 RxBuf[ RX_BUFFER_SIZE * MAX_RX_BUFFERS ];	

uint8 HdrCnt;
uint8 RxHdr[ RS232_HDR_SIZE+1 ];
uint8 RxHdrNumber[ MAX_RX_BUFFERS ];
uint8 RxHdrIdx;
uint8 RxMsgHdrIdx;;
uint8 RxHdrChk;

uint16 RxPFErrs;	// Protocol Frame Errors
uint16 RxClientBusyErrs;

uint16 CodeErrs;	// Code Errors

uint16 TxBufOverFlowErrs;
uint16 RxBufOverFlowErrs;

uint16 RxMsgs;
uint16 RxBytes;
uint16 RxUnMatchedMsgs;

uint16 TxMsgs;
uint16 TxBytes;
	
uint8 TxByteCnt;
uint8 TxBufWtIdx;
uint8 TxBufRdIdx;
uint8 TxChkSum;
char  TxChkSumChars[2];
uint8 TxBuf[ TX_BUF_SIZE ];

}RS232_CBlk;


#ifdef RS232

#define  USART_RX_BUFFERS_MASK 0x07
#define USART_RX_BUFFERS() (usart_rx_tx_buf_status.status & USART_RX_BUFFERS_MASK)

#define USART_USB_RX_RDY (!USB_USART_ReceiveBufferIsEmpty())
#define USART_A_RX_RDY (!UART1_ReceiveBufferIsEmpty())
#define USART_B_RX_RDY (!UART2_ReceiveBufferIsEmpty())

#define USARTA_READ_BYTE() UART1_Read()
#define USARTB_READ_BYTE() UART2_Read()
#define USARTUSB_READ_BYTE() USB_USART_Read()

#define  USART_TX_BUFFERS_MASK 0x038
#define USART_TX_BUFFERS() (usart_rx_tx_buf_status.status & USART_TX_BUFFERS_MASK)

#define USART_USB_TX_RDY (!USB_USART_TransmitBufferIsFull())
#define USART_A_TX_RDY (!UART1_TransmitBufferIsFull())
#define USART_B_TX_RDY (!UART1_TransmitBufferIsFull())

#define USARTA_WRITE_BYTE(c) UART1_Write(c)
#define USARTB_WRITE_BYTE(c) UART2_Write(c)
#define USARTUSB_WRITE_BYTE(c) USB_USART_Write(c)

uint8 RS232_task_state = 0;
RS232_CBlk RS232_a, RS232_b, RS232_usb;
RS232_CBlk *RS232_CBlkPtrs[] = {&RS232_a,&RS232_b,&RS232_usb};

RS232_Switch_CBlk RS232_Switch_a, RS232_Switch_b, RS232_Switch_usb;

char const HexToAscii[] = 
	{'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	
void _rs232_put_xmt_char( RS232_CBlk *p, char c );
void _rs232_put_xmt_buf( RS232_CBlk *p, char *ptr );

#endif

void RS232_Xmt_Error_Msg( uint8, uint8 );
void RS232_put_xmt_char( RS232_CBlk *p, char c );
char *RS232_MakeChkSum( char *ptr );

#endif
