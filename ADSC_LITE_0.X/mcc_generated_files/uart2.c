/**
  UART2 Generated Driver File 

  @Company
    Microchip Technology Inc.

  @File Name
    uart2.c

  @Summary
    This is the generated source file for the UART2 driver using MPLAB(c) Code Configurator

  @Description
    This source file provides APIs for driver for UART2. 
    Generation Information : 
        Product Revision  :  MPLAB(c) Code Configurator - pic24-dspic-pic32mm : v1.25
        Device            :  PIC24FJ128GB204
        Driver Version    :  0.5
    The generated drivers are tested against the following:
        Compiler          :  XC16 1.26
        MPLAB 	          :  MPLAB X 3.45
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/

#include <xc.h>
#include <p24FJ128GB204.h>
#include "../../Core_api.h"
#include "RS232_api.h"
#include "uart2.h"

/**
  Section: Data Type Definitions
*/

/** UART Driver Queue Status

  @Summary
    Defines the object required for the status of the queue.
*/

typedef struct {

uint16 RxOErrs;		// Overrun Errors
uint16 RxFErrs;		// Framing Errors
uint16 RxPErrs;		// Parity Errors
} 

usart_stats;

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

/** UART Driver Hardware Instance Object

  @Summary
    Defines the object required for the maintenance of the hardware instance.

*/
typedef struct
{
    /* RX Byte Q */
    uint8_t                                      *rxTail ;

    uint8_t                                      *rxHead ;

    /* TX Byte Q */
    uint8_t                                      *txTail ;

    uint8_t                                      *txHead ;

    UART_BYTEQ_STATUS                        rxStatus ;

    UART_BYTEQ_STATUS                        txStatus ;
    
    usart_stats         stats;

} UART_OBJECT ;

static UART_OBJECT uart2_obj ;

/** UART Driver Queue Length

  @Summary
    Defines the length of the Transmit and Receive Buffers

*/

#define UART2_CONFIG_TX_BYTEQ_LENGTH 6
#define UART2_CONFIG_RX_BYTEQ_LENGTH 10


/** UART Driver Queue

  @Summary
    Defines the Transmit and Receive Buffers

*/

static uint8_t uart2_txByteQ[UART2_CONFIG_TX_BYTEQ_LENGTH] ;
static uint8_t uart2_rxByteQ[UART2_CONFIG_RX_BYTEQ_LENGTH] ;


/**
  Section: Driver Interface
*/

void UART2_Initialize_Port_Speed( int BRG )
{
   // STSEL 1; IREN disabled; PDSEL 8N; UARTEN enabled; RTSMD disabled; USIDL disabled; WAKE disabled; ABAUD disabled; LPBACK disabled; BRGH enabled; URXINV disabled; UEN TX_RX; 
   U2MODE = (0x8008 & ~(1<<15));  // disabling UARTEN bit   
   // UTXISEL0 TX_ONE_CHAR; UTXINV disabled; OERR NO_ERROR_cleared; URXISEL RX_ONE_CHAR; UTXBRK COMPLETED; UTXEN disabled; ADDEN disabled; 
   U2STA = 0x0000;
   // BaudRate = 4800; Frequency = 16000000 Hz; U2BRG 832; 
   U2BRG = BRG;
   // ADMADDR 0; ADMMASK 0; 
   U2ADMD = 0x0000;
   // T0PD 1 ETU; PTRCL T0; TXRPT Retransmits the error byte once; CONV Direct; SCEN disabled; 
   U2SCCON = 0x0000;
   // TXRPTIF disabled; TXRPTIE disabled; WTCIF disabled; WTCIE disabled; PARIE disabled; GTCIF disabled; GTCIE disabled; RXRPTIE disabled; RXRPTIF disabled; 
   U2SCINT = 0x0000;
   // GTC 0; 
   U2GTC = 0x0000;
   // WTCL 0; 
   U2WTCL = 0x0000;
   // WTCH 0; 
   U2WTCH = 0x0000;
   
   //Make sure to set LAT bit corresponding to TxPin as high before UART initialization
   U2MODEbits.UARTEN = 1;  // enabling UART ON bit
   U2STAbits.UTXEN = 0;    // enabling TX section
   IFS1bits.U2TXIF = false; // clear the tx int flag
   IEC1bits.U2TXIE = false ;
}

void UART2_Initialize (void)
{
    // BaudRate = 4800; Frequency = 16000000 Hz; U2BRG 832;
    UART2_Initialize_Port_Speed( 0x340 );
   
   IEC1bits.U2RXIE = 0; // disable rx interrupts for now
   
   uart2_obj.txHead = uart2_txByteQ;
   uart2_obj.txTail = uart2_txByteQ;
   uart2_obj.rxHead = uart2_rxByteQ;
   uart2_obj.rxTail = uart2_rxByteQ;
   uart2_obj.rxStatus.s.empty = true;
   uart2_obj.txStatus.s.empty = true;
   uart2_obj.txStatus.s.full = false;
   uart2_obj.rxStatus.s.full = false;
   
   uart2_obj.stats.RxFErrs = 0;
   uart2_obj.stats.RxOErrs = 0;
   uart2_obj.stats.RxPErrs = 0;
}

/*
 * Appends to stats to (char *)buf and optionally clears stats 
 */
char *UART2_GetStats( int8 Clear, char *buf )
{
	_stat_append( (unsigned long) uart2_obj.stats.RxFErrs, buf, TRUE,3 );
	_stat_append( (unsigned long) uart2_obj.stats.RxOErrs, buf, TRUE,3 );
	_stat_append( (unsigned long) uart2_obj.stats.RxPErrs, buf, TRUE,3 );

	if (Clear)
	{
        uart2_obj.stats.RxFErrs = 0;
        uart2_obj.stats.RxOErrs = 0;
        uart2_obj.stats.RxPErrs = 0;
	}  	
 	return( buf );
}

uint8 UART2_GetTxDone()
{
    if (U2STAbits.TRMT)
        return 1;
    else
        return 0;
}
/**
    Maintains the driver's transmitter state machine and implements its ISR
*/
void __attribute__ ( ( interrupt, no_auto_psv ) ) _U2TXInterrupt ( void )
{ 
    IFS1bits.U2TXIF = false;
    
    if(!uart2_obj.txStatus.s.full)
    {
        RS232_B_Tx_Isr();
        if(uart2_obj.txStatus.s.empty)
        {
            IEC1bits.U2TXIE = false;
            return;
        }
    }

    while(!(U2STAbits.UTXBF == 1))
    {

        U2TXREG = *uart2_obj.txHead;

        uart2_obj.txHead++;

        if(uart2_obj.txHead == (uart2_txByteQ + UART2_CONFIG_TX_BYTEQ_LENGTH))
        {
            uart2_obj.txHead = uart2_txByteQ;
        }

        uart2_obj.txStatus.s.full = false;

        if(uart2_obj.txHead == uart2_obj.txTail)
        {
            uart2_obj.txStatus.s.empty = true;
            break;
        }
    }
}

void __attribute__ ( ( interrupt, no_auto_psv ) ) _U2RXInterrupt( void )
{
    uint8 tmp;
    
    while((U2STAbits.URXDA == 1))
    {
        if (uart2_obj.rxStatus.s.full)
        {
            tmp = U2RXREG;
            uart2_obj.stats.RxOErrs++;
        }
        else
        {
            *uart2_obj.rxTail = U2RXREG;

            uart2_obj.rxTail++;

            if(uart2_obj.rxTail == (uart2_rxByteQ + UART2_CONFIG_RX_BYTEQ_LENGTH))
            {
                uart2_obj.rxTail = uart2_rxByteQ;
            }

            uart2_obj.rxStatus.s.empty = false;
        
            if(uart2_obj.rxTail == uart2_obj.rxHead)
            {
                // Sets the flag RX full
                uart2_obj.rxStatus.s.full = true;
                break;
            }
        }
    }
    if (uart2_obj.rxStatus.s.empty == false)
        RS232_B_Rx_Isr();
    IFS1bits.U2RXIF = false;
}


void __attribute__ ( ( interrupt, no_auto_psv ) ) _U2ErrInterrupt ( void )
{
    if ((U2STAbits.OERR == 1))
    {
        U2STAbits.OERR = 0;
    }
        if ((U2STAbits.OERR == 1))
    {
        U2STAbits.OERR = 0;
        uart2_obj.stats.RxOErrs++;
    }
    if ((U2STAbits.FERR == 1))
    {
        U2STAbits.FERR = 0;
        uart2_obj.stats.RxFErrs++;
    }
    if ((U2STAbits.PERR == 1))
    {
        U2STAbits.PERR = 0;
        uart2_obj.stats.RxPErrs++;
    }

    IFS4bits.U2ERIF = false;
}

/**
  Section: UART Driver Client Routines
*/

uint8_t UART2_Read( void)
{
    uint8_t data = 0;

    data = *uart2_obj.rxHead;

    uart2_obj.rxHead++;

    if (uart2_obj.rxHead == (uart2_rxByteQ + UART2_CONFIG_RX_BYTEQ_LENGTH))
    {
        uart2_obj.rxHead = uart2_rxByteQ;
    }

    if (uart2_obj.rxHead == uart2_obj.rxTail)
    {
        uart2_obj.rxStatus.s.empty = true;
    }

    uart2_obj.rxStatus.s.full = false;

    return data;
}


unsigned int UART2_ReadBuffer( uint8_t *buffer, const unsigned int bufLen)
{
    unsigned int numBytesRead = 0 ;
    while ( numBytesRead < ( bufLen ))
    {
        if( uart2_obj.rxStatus.s.empty)
        {
            break;
        }
        else
        {
            buffer[numBytesRead++] = UART2_Read () ;
        }
    }

    return numBytesRead ;
}

void UART2_Enable_Tx_Int()
{
    //IFS1bits.U2TXIF = 0;
    IEC1bits.U2TXIE = 1;
    //U2STAbits.UTXEN = 0;
    U2STAbits.UTXEN = 1;
}

void UART2_Write( const uint8_t bite)
{
    //IEC1bits.U2TXIE = false;
    
    *uart2_obj.txTail = bite;

    uart2_obj.txTail++;
    
    if (uart2_obj.txTail == (uart2_txByteQ + UART2_CONFIG_TX_BYTEQ_LENGTH))
    {
        uart2_obj.txTail = uart2_txByteQ;
    }

    uart2_obj.txStatus.s.empty = false;

    if (uart2_obj.txHead == uart2_obj.txTail)
    {
        uart2_obj.txStatus.s.full = true;
    }

    UART2_Enable_Tx_Int();
	
}


unsigned int UART2_WriteBuffer( const uint8_t *buffer , const unsigned int bufLen )
{
    unsigned int numBytesWritten = 0 ;

    while ( numBytesWritten < ( bufLen ))
    {
        if((uart2_obj.txStatus.s.full))
        {
            break;
        }
        else
        {
            UART2_Write (buffer[numBytesWritten++] ) ;
        }
    }

    return numBytesWritten ;

}


UART2_TRANSFER_STATUS UART2_TransferStatusGet (void )
{
    UART2_TRANSFER_STATUS status = 0;

    if(uart2_obj.txStatus.s.full)
    {
        status |= UART2_TRANSFER_STATUS_TX_FULL;
    }

    if(uart2_obj.txStatus.s.empty)
    {
        status |= UART2_TRANSFER_STATUS_TX_EMPTY;
    }

    if(uart2_obj.rxStatus.s.full)
    {
        status |= UART2_TRANSFER_STATUS_RX_FULL;
    }

    if(uart2_obj.rxStatus.s.empty)
    {
        status |= UART2_TRANSFER_STATUS_RX_EMPTY;
    }
    else
    {
        status |= UART2_TRANSFER_STATUS_RX_DATA_PRESENT;
    }
    return status;
}


uint8_t UART2_Peek(uint16_t offset)
{
    if( (uart2_obj.rxHead + offset) > (uart2_rxByteQ + UART2_CONFIG_RX_BYTEQ_LENGTH))
    {
      return uart2_rxByteQ[offset - (uart2_rxByteQ + UART2_CONFIG_RX_BYTEQ_LENGTH - uart2_obj.rxHead)];
    }
    else
    {
      return *(uart2_obj.rxHead + offset);
    }
}


unsigned int UART2_ReceiveBufferSizeGet(void)
{
    if(!uart2_obj.rxStatus.s.full)
    {
        if(uart2_obj.rxHead > uart2_obj.rxTail)
        {
            return(uart2_obj.rxHead - uart2_obj.rxTail);
        }
        else
        {
            return(UART2_CONFIG_RX_BYTEQ_LENGTH - (uart2_obj.rxTail - uart2_obj.rxHead));
        } 
    }
    return 0;
}


unsigned int UART2_TransmitBufferSizeGet(void)
{
    if(!uart2_obj.txStatus.s.full)
    { 
        if(uart2_obj.txHead > uart2_obj.txTail)
        {
            return(uart2_obj.txHead - uart2_obj.txTail);
        }
        else
        {
            return(UART2_CONFIG_TX_BYTEQ_LENGTH - (uart2_obj.txTail - uart2_obj.txHead));
        }
    }
    return 0;
}


bool UART2_ReceiveBufferIsEmpty (void)
{
    return(uart2_obj.rxStatus.s.empty);
}


bool UART2_TransmitBufferIsFull(void)
{
    return(uart2_obj.txStatus.s.full);
}


UART2_STATUS UART2_StatusGet (void)
{
    return U2STA;
}



/**
  End of File
*/
