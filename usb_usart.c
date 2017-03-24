/********************************************************************
 Change History:
  Rev    Description
  ----   -----------
 * 
 * *****************************************************************/

#include <xc.h>
#include <p24FJ128GB204.h>
#include "usb_usart.h"
#include "usb_usart_api.h"
#include "Core_api.h"
#include "rs232_api.h"
#include "ADSC_LITE_0.X/mcc_generated_files/usb/usb_device.h"
#include "ADSC_LITE_0.X/mcc_generated_files/usb/usb_device_config.h"

void USB_USART_init()
{
    //USBDeviceInit();
    USBModuleDisable();
    usb_usart_ctl.attached = false;
    usb_usart_ctl.state = 0;
    usb_usart_ctl.rx_status.s.empty = 1;
    usb_usart_ctl.tx_status.s.empty = 1;
    usb_usart_ctl.rx_status.s.full = 0;
    usb_usart_ctl.tx_status.s.full = 0;
    //USBDeviceAttach();
    
    Core_SchedTask(_1MS_QUE, PROC_USB_USART_TASK | 0x80, 1, SGNL_TIMER_MASK, 1);
}

/******************************************************************************
 	Function:
 		void _usb_usart_chk_rx()    // get any bytes from host
 
 	Description:
    This functions gets any received bytes from the host and puts them
    in the receive buffer. The rs232 task is signalled if any bytes 
    came in.
	
 	PreCondition:
 *      None

	Parameters:
		None
		
	Return Values:
		None
		
	Remarks:
	 
  *****************************************************************************/
#undef USB_NEAR_LOOP
#ifdef USB_NEAR_LOOP 
void USB_USART_Write( uint8 bite );
uint8 USB_USART_Read();
#endif
extern USB_HANDLE CDCDataOutHandle;
extern volatile unsigned char cdc_data_rx[CDC_DATA_OUT_EP_SIZE];

void _usb_usart_chk_rx()    // get any bytes from host
{
 int j=0, len;
 static int pkt_idx=0;

    if ((USB_RX_BUF_SIZE - usb_usart_ctl.rx_buf_cnt) && (!USBHandleBusy(CDCDataOutHandle)))
    {
        len = USBHandleGetLength(CDCDataOutHandle);

        //i = getsUSBUSART( bites, free_rx_buf_bytes);
        while ((pkt_idx < len) && ( j < (USB_RX_BUF_SIZE - usb_usart_ctl.rx_buf_cnt) ) )
        {
            // store the character
            usb_usart_ctl.rx_buf[ usb_usart_ctl.rx_in_idx++ ] = cdc_data_rx[ pkt_idx++ ];
            // wrap index in necessary
            if (usb_usart_ctl.rx_in_idx >= USB_RX_BUF_SIZE)
                usb_usart_ctl.rx_in_idx = 0;

            usb_usart_ctl.rx_buf_cnt++;
#ifdef USB_NEAR_LOOP            
            USB_USART_Write( USB_USART_Read() );      
#endif
            j++;
        }
        // if we've consumed all the data in this packet
        if (pkt_idx >= len)
        {
            pkt_idx = 0;
            /*
            * Prepare dual-ram buffer for next OUT transaction
            */
            CDCDataOutHandle = USBRxOnePacket(CDC_DATA_EP,(uint8_t*)&cdc_data_rx,sizeof(cdc_data_rx));
        }
    }

    if (usb_usart_ctl.rx_buf_cnt >= USB_RX_BUF_SIZE)
        usb_usart_ctl.rx_status.s.full = 1;
 
#ifndef USB_NEAR_LOOP     
    if (j>0)
    {
        RS232_USB_Rx_Isr();
        usb_usart_ctl.rx_status.s.empty = 0;
    }
#endif    
}

uint8 USB_USART_ReceiveBufferIsEmpty()
{
    return(usb_usart_ctl.rx_status.s.empty);
}

int USB_USART_GetRxByteCnt()
{
    return(usb_usart_ctl.rx_buf_cnt);
}

uint8 USB_USART_Read()
{
 uint8 bite = 0;
 
    if (usb_usart_ctl.rx_buf_cnt)
    {
        bite = usb_usart_ctl.rx_buf[ usb_usart_ctl.rx_out_idx++ ];
        // wrap index in necessary
        if (usb_usart_ctl.rx_out_idx >= USB_RX_BUF_SIZE)
            usb_usart_ctl.rx_out_idx = 0;

        usb_usart_ctl.rx_buf_cnt--;
        usb_usart_ctl.rx_status.s.full = 0;
        if (usb_usart_ctl.rx_buf_cnt == 0)
            usb_usart_ctl.rx_status.s.empty = 1;
    }
    return( bite );
}

uint8 USB_USART_TransmitBufferIsFull()
{
    if (USBUSARTIsTxTrfReady())
        return( usb_usart_ctl.tx_status.s.full );
    else
        return(1);
}

int USB_USART_GetTxFreeByteCnt()
{
    return( USB_TX_BUF_SIZE - usb_usart_ctl.tx_buf_cnt );
}

void USB_USART_Write( uint8 bite )
{
        usb_usart_ctl.tx_buf[ usb_usart_ctl.tx_in_idx++ ] = bite;
        // wrap index in necessary
        if (usb_usart_ctl.tx_in_idx >= USB_TX_BUF_SIZE)
            usb_usart_ctl.tx_in_idx = 0;

        usb_usart_ctl.tx_buf_cnt++;
        usb_usart_ctl.tx_status.s.empty = 0;
        if (usb_usart_ctl.tx_buf_cnt >= USB_TX_BUF_SIZE)
            usb_usart_ctl.tx_status.s.full = 1;
}

uint8 USB_USART_PutsNextTxBuf( uint8 *p, int len )
{
    uint8 free,i=0;
    
    free = USB_TX_BUF_SIZE - usb_usart_ctl.tx_buf_cnt;
    while( free && len)
    {
        usb_usart_ctl.tx_buf[ usb_usart_ctl.tx_in_idx++ ] = *p++;
        // wrap index in necessary
        if (usb_usart_ctl.tx_in_idx >= USB_TX_BUF_SIZE)
            usb_usart_ctl.tx_in_idx = 0;

        usb_usart_ctl.tx_buf_cnt++; free--; len--; i++;
        usb_usart_ctl.tx_status.s.empty = 0;
    }
    if (free == 0)
        usb_usart_ctl.tx_status.s.full = 1;
    
    return(i);
}

void _usb_usart_chk_tx()    // send any bytes to host
{
 uint8 *p, byte_cnt, byte_run;
 
    if (USBUSARTIsTxTrfReady())
    {
        if (usb_usart_ctl.tx_buf_cnt == 0)
        {
            usb_usart_ctl.tx_status.s.empty = 1;
            RS232_USB_Tx_Isr();
        }        
        else
        {
            // Get the address of byte block to send
            p = &(usb_usart_ctl.tx_buf[ usb_usart_ctl.tx_out_idx ]);
            
            // figure out how many bytes
            byte_run = USB_TX_BUF_SIZE - usb_usart_ctl.tx_out_idx;
            byte_cnt = usb_usart_ctl.tx_buf_cnt;
            
            // keep the array within the buffer
            if (byte_cnt > byte_run) 
                byte_cnt = byte_run;
            
            putUSBUSART( (uint8_t *) p, byte_cnt );

            usb_usart_ctl.tx_out_idx += byte_cnt;
            if (usb_usart_ctl.tx_out_idx >= USB_TX_BUF_SIZE)
                usb_usart_ctl.tx_out_idx = 0;

            usb_usart_ctl.tx_buf_cnt -= byte_cnt;  
            usb_usart_ctl.tx_status.s.full = 0;
        }
    }
     
}
/******************************************************************************
    Function:
        int8 USB_USART_ProcTask( int8 signal )
 
    Description:
    This task is run every 1 ms. Handles the USB communications and
    presents a virtual COM port to the rs232 task.
 
	
    PreCondition:
 *      None

    Parameters:
        None
		
    Return Values:
        None
		
    Remarks:
	 
 *****************************************************************************/
#define USB_STATE_MACHINE
int8 USB_USART_ProcTask( int8 signal )
{
    switch (usb_usart_ctl.state)
    {
        case 0:
            if (USB_BUS_SENSE)  // if we're attached to a host
            {
                if (usb_usart_ctl.attached == false) // if we previously weren't attached
                {
                    USBDeviceInit();
                    usb_usart_ctl.attached = true;
                }
                usb_usart_ctl.state++;
            }
            else
            {
                usb_usart_ctl.attached = false;
                USBModuleDisable();
                return(SGNL_NO_REQUEUE);
            }
            break;

        case 1:
            USBDeviceTasks();
            usb_usart_ctl.state++;
            break;

        case 2:
            if((USBGetDeviceState() >= CONFIGURED_STATE) ||
                (USBIsDeviceSuspended() != true))
            {
                CDCTxService();
                usb_usart_ctl.state++;
            }
            else
            {
                usb_usart_ctl.state = 0;
                return(SGNL_NO_REQUEUE);
            }
            break;
        case 3:
            _usb_usart_chk_rx();    // get any bytes from host
            usb_usart_ctl.state++;
            break;
        case 4:
            _usb_usart_chk_tx();    // send any bytes to host
            usb_usart_ctl.state++;
            break;
        case 5:
            CDCTxService();         // send any bytes to host
            usb_usart_ctl.state = 0;
            return(SGNL_NO_REQUEUE);
            break;                        
    }

    return( signal );
}

