/* 
 * File:   usb_usart_com_api.h
 * Author: Dad
 *
 * Created on January 9, 2017, 8:13 AM
 */

#ifndef USB_USART_API_H
#define	USB_USART_API_H

#include "Core_api.h"

#ifdef	__cplusplus
extern "C" {
#endif

int USB_USART_GetRxByteCnt();
uint8 USB_USART_ReceiveBufferIsEmpty();
uint8 USB_USART_Read();

int USB_USART_GetTxFreeByteCnt();
uint8 USB_USART_TransmitBufferIsFull();
void USB_USART_Write( uint8 bite );
uint8 USB_USART_PutsNextTxBuf( uint8 *p, int len );

#ifdef	__cplusplus
}
#endif

#endif	/* USB_USART_API_H */

