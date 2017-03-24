/* 
 * File:   usb_usart.h
 * Author: Paul OHare
 *
 * Created on January 9, 2017, 8:13 AM
 */

#ifndef USB_USART_H
#define	USB_USART_H



#ifdef	__cplusplus
extern "C" {
#endif
#include "ADSC_LITE_0.X/mcc_generated_files/usb/usb.h"
#include "Core_api.h"

typedef union
{
    struct
    {
            uint8_t full:1;
            uint8_t empty:1;
            uint8_t reserved:6;
    }s;
    uint8_t status;
}

UART_BYTEQ_STATUS;

#define USB_RX_BUF_SIZE 8
#define USB_TX_BUF_SIZE 8

typedef struct usb_control_block {
    int8 state;
    int attached;

    uint8 rx_buf[ USB_RX_BUF_SIZE ];
    int rx_buf_cnt;
    int rx_in_idx;
    int rx_out_idx;
    int rx_ovr_errs;
    UART_BYTEQ_STATUS rx_status;
    
    uint8 tx_buf[ USB_TX_BUF_SIZE ];
    int tx_buf_cnt;
    int tx_in_idx;
    int tx_out_idx;
    int tx_ovr_errs;
    UART_BYTEQ_STATUS tx_status;
    
}usb_ctl;

usb_ctl usb_usart_ctl;

#ifdef	__cplusplus
}
#endif

#endif	/* USB_USART_H */

