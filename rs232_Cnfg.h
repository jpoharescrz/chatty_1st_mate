/*///////////////////////////////////////////////////////////////////////
////                          rs232_Cnfg.h                              ////
////   This file is included in the rs232_vX.c file and
////   defines how the RS232 commands will get parsed, the
////   sizes of buffers, the number of tokens and how they'll                                                              ////
////   be delimited. User Error codes may also be spec'd
////   here
////   The rs232_Cnfg.h should be included at the top of the
////   client code which will need RS232 input.
////   
////   Clients can specify how each USART functions here.
////
////   All tokens are contained in RxBuf[] and are 0-terminated.
////   That is, when the PROTOCOL_DELIM character is seen the
////   current token is 0-terminated and a new token started.
////   Tokens do NOT contain any PROTOCOL_START, PROTOCOL_STOP,
////   or PROTOCOL_DELIM characters.
////
////   The client can also use the following transmit functions:
////   Void Xmt_Msg( char *ptr )
////     *ptr is a null terminated string
////   Void Xmt_Error_Msg( int8 ErrorCode )
////     Sends the ErrorCodeToken[] defined below and then the
////     spec'd ErrorCode.     
////
///////////////////////////////////////////////////////////////*/
#include "error_defs.h"
#include "isd_api.h"

enum usart_ports {
 USARTA,
 USARTB,
 USART_USB,
 MAX_USART_PORTS
};
//#define MAX_USART_PORTS 2
//#define USARTA USART_PORT_A
//#define USARTB USART_PORT_B


/*Switching Bit Masks */
#define USARTA_BIT_MASK 1<<USARTA
#define USARTB_BIT_MASK 1<<USARTB
#define USART_USB_BIT_MASK 1<<USART_USB
#define NO_SWITCHING 0

/*Switching Signals */
enum rs232_task_signals {
    SGNL_USARTA_SWITCH_MASK_BIT,
    SGNL_USARTB_SWITCH_MASK_BIT,
    SGNL_USART_USB_SWITCH_MASK_BIT,
    SGNL_USART_RX_BYTES_BIT,
    SGNL_USART_TX_BYTES_BIT
};

#define SGNL_USARTA_SWITCH_MASK (1<<SGNL_USARTA_SWITCH_MASK_BIT)
#define SGNL_USARTB_SWITCH_MASK (1<<SGNL_USARTB_SWITCH_MASK_BIT)
#define SGNL_USART_USB_SWITCH_MASK (1<<SGNL_USART_USB_SWITCH_MASK_BIT)
#define SGNL_USART_RX_BYTES_MASK (1<<SGNL_USART_RX_BYTES_BIT)
#define SGNL_USART_TX_BYTES_MASK (1<<SGNL_USART_TX_BYTES_BIT)

#define _SGNL_USARTA_SWITCH_MASK (SGNL_USARTA_SWITCH_MASK^0xFF) 
#define _SGNL_USARTB_SWITCH_MASK (SGNL_USARTB_SWITCH_MASK^0xFF)
#define _SGNL_USART_USB_SWITCH_MASK (SGNL_USART_USB_SWITCH_MASK^0xFF)
#define _SGNL_USART_RX_BYTES_MASK (SGNL_USART_RX_BYTES_MASK^0xFF)
#define _SGNL_USART_TX_BYTES_MASK (SGNL_USART_TX_BYTES_MASK^0xFF)

/* PROTOCOL defines. Applies to both USARTA, USARTB and USART_USB */
#define PROTOCOL_START '$'
#define PROTOCOL_STOP '*'
#define PROTOCOL_DELIM ','
#define RS232_HDR_SIZE 7

#define UNMATCHED_ZERO_IDX
/*/////////////////////////////////////////////////////////////////////////////////////
// Clients define the NMEA messages they'll use here
// */
enum rs232_msg_indices {
 RS232_UNMATCHED_MSG,
 RS232_MSG_XXHDM,   //2
 RS232_MSG_XXHDT,   //4
 RS232_MSG_XXHDG,   //8
 RS232_MSG_XXRMB,   //10
 RS232_MSG_XXRMC,   //20
 RS232_MSG_XXVTG,
 RS232_MSG_PMTTS,
 RS232_MSG_PMTST,
#ifdef ONBOARD_GPS_MODULE
 RS232_MSG_PMKT001,
#endif
 MAX_RS232_MSGS
};

#define RS232_MSG_XXHDM_MASK (1 << RS232_MSG_XXHDM )
#define RS232_MSG_XXHDT_MASK (1 << RS232_MSG_XXHDT )
#define RS232_MSG_XXHDG_MASK (1 << RS232_MSG_XXHDG )
#define RS232_MSG_XXRMB_MASK (1 << RS232_MSG_XXRMB )
#define RS232_MSG_XXRMC_MASK (1 << RS232_MSG_XXRMC )
#define RS232_MSG_XXVTG_MASK (1 << RS232_MSG_XXVTG )

#ifdef RS232

typedef struct rs232_msg_struct {
    char msg_hdr[RS232_HDR_SIZE+1];
    uint8 task;
	uint8 msg_remap;
    uint8 usart_A_enabled:1;
    uint8 usart_B_enabled:1;
    uint8 usart_USB_enabled:1;
    uint8 do_not_loop:1;
	uint8 is_switchable:1;
    uint8 switch_to_usb:1;
    uint8 reserved:2;
}rs232_msg_rec;

rs232_msg_rec rs232_msgs[] = {
    // unmatched msg, mapped to RS232_UNMATCHED_MSG, usart_A_enabled, usart_B_enabled, usart_USB_enabled, do_not_loop, switchable, switchable to usb
    {"XXXXX",0, RS232_UNMATCHED_MSG, TRUE, TRUE,TRUE,FALSE, TRUE,TRUE,0},
    // HCHDM msg, mapped to RS232_MSG_XXHDM, usart_A_enabled, usart_B_enabled, usart_USB_enabled, do_not_loop, switchable, switchable to usb
    {"??HDM",PROC_NMEATASK, RS232_MSG_XXHDM, TRUE,TRUE,TRUE, FALSE, TRUE,TRUE,0},
    // HCHDM msg, mapped to RS232_MSG_XXHDM, usart_A_enabled, usart_B_enabled, usart_USB_enabled, do_not_loop, switchable, switchable to usb
    {"??HDT",PROC_NMEATASK, RS232_MSG_XXHDT, TRUE,TRUE,TRUE, FALSE, TRUE,TRUE,0},    
    // HCHDG msg, mapped to RS232_MSG_XXHDG, usart_A_enabled, usart_B_enabled, usart_USB_enabled, do_not_loop, switchable, switchable to usb
    {"??HDG",PROC_NMEATASK, RS232_MSG_XXHDG, TRUE,TRUE,TRUE, FALSE, TRUE,TRUE,0},   
    // GPRMB msg, mapped to RS232_MSG_XXRMB, usart_A_enabled, usart_B_enabled, usart_USB_enabled, do_not_loop, switchable, switchable to usb
    {"??RMB",PROC_NMEATASK, RS232_MSG_XXRMB, TRUE,TRUE,TRUE, FALSE, TRUE,TRUE,0},  
    // GPRMC msg, mapped to RS232_MSG_XXRMC, usart_A_enabled, usart_B_enabled, usart_USB_enabled, do_not_loop, switchable, switchable to usb
    {"??RMC",PROC_NMEATASK, RS232_MSG_XXRMC, TRUE,TRUE,TRUE, FALSE, TRUE,TRUE,0},  
    // GPVTG msg, mapped to RS232_MSG_XXRMC, usart_A_enabled, usart_B_enabled, usart_USB_enabled, do_not_loop, switchable, switchable to usb
    {"??VTG",PROC_NMEATASK, RS232_MSG_XXHDM, FALSE, FALSE, FALSE, FALSE, TRUE,TRUE,0},      
    // PMTTS msg, mapped to RS232_MSG_PMTTS, usart_A_enabled, usart_B_enabled, usart_USB_enabled, do_not_loop, switchable, switchable to usb
    {"PMTTS",PROC_ISD_TASK, RS232_MSG_PMTTS, TRUE,TRUE,TRUE, TRUE, FALSE, FALSE, 0}, 
    // PMTST msg, mapped to RS232_MSG_PMTST, usart_A_enabled, usart_B_enabled, usart_USB_enabled, do_not_loop, switchable, switchable to usb
    {"PMTST",PROC_NMEATASK, RS232_MSG_PMTST, TRUE,TRUE,TRUE, TRUE, FALSE, FALSE, 0}
};

typedef struct msg_options_struct {
    char talker_id[2];
    uint8 usart_A_enabled:1;
    uint8 usart_B_enabled:1;
    uint8 usart_USB_enabled:1;    
    uint8 res0:5;
    uint8 res1;
} msg_opt_rec;

typedef union msg_options_rec {
    uint8 flash_image_bytes[4];
    msg_opt_rec s;
}msg_options_rec;

#endif

// MAX_RX_TOKENS sets the maximum number of fields in a msg
#define MAX_RX_TOKENS 30
#define RX_TOKEN_BUF_SIZE MAX_RX_TOKENS

// MAX_RX_CHARS sets the maximum chars per msg
#define MAX_RX_CHARS 85
#define RX_BUFFER_SIZE MAX_RX_CHARS

// TX_BUF_SIZE sets the maximum char length of a TX'd msg
#define TX_BUF_SIZE 256
