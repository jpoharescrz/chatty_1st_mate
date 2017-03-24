/**
  UART1 Generated Driver File 

  @Company
    Microchip Technology Inc.

  @File Name
    uart1.c

  @Summary
    This is the generated source file for the UART1 driver using MPLAB(c) Code Configurator

  @Description
    This source file provides APIs for driver for UART1. 
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
#include "../../rs232_api.h"
#include "uart1.h"

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
    uint8_t             *rxTail ;

    uint8_t             *rxHead ;

    /* TX Byte Q */
    uint8_t             *txTail ;

    uint8_t             *txHead ;

    UART_BYTEQ_STATUS   rxStatus ;

    UART_BYTEQ_STATUS   txStatus ;
    
    usart_stats         stats;

} UART_OBJECT ;

static UART_OBJECT uart1_obj ;

/** UART Driver Queue Length

  @Summary
    Defines the length of the Transmit and Receive Buffers

*/

#define UART1_CONFIG_TX_BYTEQ_LENGTH 6
#define UART1_CONFIG_RX_BYTEQ_LENGTH 10


/** UART Driver Queue

  @Summary
    Defines the Transmit and Receive Buffers

*/

static uint8_t uart1_txByteQ[UART1_CONFIG_TX_BYTEQ_LENGTH] ;
static uint8_t uart1_rxByteQ[UART1_CONFIG_RX_BYTEQ_LENGTH] ;

/**
  Section: Driver Interface
*/
void UART1_Initialize_Port_Speed( int BRG )
{
       // STSEL 1; IREN disabled; PDSEL 8N; UARTEN enabled; RTSMD disabled; USIDL disabled; WAKE disabled; ABAUD disabled; LPBACK disabled; BRGH enabled; URXINV disabled; UEN TX_RX; 
   U1MODE = (0x8008 & ~(1<<15));  // disabling UARTEN bit   
   // UTXISEL0 TX_ONE_CHAR; UTXINV disabled; OERR NO_ERROR_cleared; URXISEL RX_ONE_CHAR; UTXBRK COMPLETED; UTXEN disabled; ADDEN disabled; 
   U1STA = 0x0000;
   // BaudRate = 4800; Frequency = 16000000 Hz; U1BRG 832; 
   U1BRG = BRG;
   // ADMADDR 0; ADMMASK 0; 
   U1ADMD = 0x0000;
   // T0PD 1 ETU; PTRCL T0; TXRPT Retransmits the error byte once; CONV Direct; SCEN disabled; 
   U1SCCON = 0x0000;
   // TXRPTIF disabled; TXRPTIE disabled; WTCIF disabled; WTCIE disabled; PARIE disabled; GTCIF disabled; GTCIE disabled; RXRPTIE disabled; RXRPTIF disabled; 
   U1SCINT = 0x0000;
   // GTC 0; 
   U1GTC = 0x0000;
   // WTCL 0; 
   U1WTCL = 0x0000;
   // WTCH 0; 
   U1WTCH = 0x0000;

   //Make sure to set LAT bit corresponding to TxPin as high before UART initialization
   U1MODEbits.UARTEN = 1;  // enabling UART ON bit
   U1STAbits.UTXEN = 0;    // enabling TX section
   IFS0bits.U1TXIF = false; // clear the tx int flag
   IEC0bits.U1TXIE = false ;
}
void UART1_Initialize (void)
{
    // BaudRate = 4800; Frequency = 16000000 Hz; U1BRG 832; 
    UART1_Initialize_Port_Speed( 0x340 );

   IEC0bits.U1RXIE = 0; // disable rx interrupts for now
   
   uart1_obj.txHead = uart1_txByteQ;
   uart1_obj.txTail = uart1_txByteQ;
   uart1_obj.rxHead = uart1_rxByteQ;
   uart1_obj.rxTail = uart1_rxByteQ;
   uart1_obj.rxStatus.s.empty = true;
   uart1_obj.txStatus.s.empty = true;
   uart1_obj.txStatus.s.full = false;
   uart1_obj.rxStatus.s.full = false;
   
   uart1_obj.stats.RxFErrs = 0;
   uart1_obj.stats.RxOErrs = 0;
   uart1_obj.stats.RxPErrs = 0;
}

/*
 * Appends to stats to (char *)buf and optionally clears stats 
 */
char *UART1_GetStats( int8 Clear, char *buf )
{
	_stat_append( (unsigned long) uart1_obj.stats.RxFErrs, buf, TRUE,3 );
	_stat_append( (unsigned long) uart1_obj.stats.RxOErrs, buf, TRUE,3 );
	_stat_append( (unsigned long) uart1_obj.stats.RxPErrs, buf, TRUE,3 );

	if (Clear)
	{
        uart1_obj.stats.RxFErrs = 0;
        uart1_obj.stats.RxOErrs = 0;
        uart1_obj.stats.RxPErrs = 0;
	}  	
 	return( buf );
}

uint8 UART1_GetTxDone()
{
    if (U1STAbits.TRMT)
        return 1;
    else
        return 0;
}
/**
    Maintains the driver's transmitter state machine and implements its ISR
*/
void __attribute__ ( ( interrupt, no_auto_psv ) ) _U1TXInterrupt ( void )
{ 
    
    IFS0bits.U1TXIF = false;
    if(!uart1_obj.txStatus.s.full)
    {
        RS232_A_Tx_Isr();
        if(uart1_obj.txStatus.s.empty)
        {
            IEC0bits.U1TXIE = false;
            return;
        }
    }

    while(!(U1STAbits.UTXBF == 1))
    {

        U1TXREG = *uart1_obj.txHead;

        uart1_obj.txHead++;

        if(uart1_obj.txHead == (uart1_txByteQ + UART1_CONFIG_TX_BYTEQ_LENGTH))
        {
            uart1_obj.txHead = uart1_txByteQ;
        }

        uart1_obj.txStatus.s.full = false;

        if(uart1_obj.txHead == uart1_obj.txTail)
        {
            uart1_obj.txStatus.s.empty = true;
            break;
        }
    }
}

void __attribute__ ( ( interrupt, no_auto_psv ) ) _U1RXInterrupt( void )
{   
    uint8 tmp;
    
    while((U1STAbits.URXDA == 1))
    {
        if (uart1_obj.rxStatus.s.full)
        {
            tmp = U1RXREG;
            uart1_obj.stats.RxOErrs++;
        }
        else
        {
            *uart1_obj.rxTail = U1RXREG;

            uart1_obj.rxTail++;

            if(uart1_obj.rxTail == (uart1_rxByteQ + UART1_CONFIG_RX_BYTEQ_LENGTH))
            {
                uart1_obj.rxTail = uart1_rxByteQ;
            }

            uart1_obj.rxStatus.s.empty = false;
        
            if(uart1_obj.rxTail == uart1_obj.rxHead)
            {
                //Sets the flag RX full
                uart1_obj.rxStatus.s.full = true;
                break;
            }
        }
    }
    if (uart1_obj.rxStatus.s.empty == false)
        RS232_A_Rx_Isr();
    
    IFS0bits.U1RXIF = false;
}


void __attribute__ ( ( interrupt, no_auto_psv ) ) _U1ErrInterrupt ( void )
{
    if ((U1STAbits.OERR == 1))
    {
        U1STAbits.OERR = 0;
        uart1_obj.stats.RxOErrs++;
    }
    if ((U1STAbits.FERR == 1))
    {
        U1STAbits.FERR = 0;
        uart1_obj.stats.RxFErrs++;
    }
    if ((U1STAbits.PERR == 1))
    {
        U1STAbits.PERR = 0;
        uart1_obj.stats.RxPErrs++;
    }

    IFS4bits.U1ERIF = false;
}

/**
  Section: UART Driver Client Routines
*/

uint8_t UART1_Read( void)
{
    uint8_t data = 0;

    data = *uart1_obj.rxHead;

    uart1_obj.rxHead++;

    if (uart1_obj.rxHead == (uart1_rxByteQ + UART1_CONFIG_RX_BYTEQ_LENGTH))
    {
        uart1_obj.rxHead = uart1_rxByteQ;
    }

    if (uart1_obj.rxHead == uart1_obj.rxTail)
    {
        uart1_obj.rxStatus.s.empty = true;
    }

    uart1_obj.rxStatus.s.full = false;

    return data;
}


unsigned int UART1_ReadBuffer( uint8_t *buffer, const unsigned int bufLen)
{
    unsigned int numBytesRead = 0 ;
    while ( numBytesRead < ( bufLen ))
    {
        if( uart1_obj.rxStatus.s.empty)
        {
            break;
        }
        else
        {
            buffer[numBytesRead++] = UART1_Read () ;
        }
    }

    return numBytesRead ;
}

void UART1_Enable_Tx_Int()
{
    //IFS0bits.U1TXIF = 0;
    IEC0bits.U1TXIE = 1 ;
    //U1STAbits.UTXEN = 0;
    U1STAbits.UTXEN = 1;
}

void UART1_Write( uint8 bite)
{
    //IEC0bits.U1TXIE = false;
    
    *uart1_obj.txTail = bite;

    uart1_obj.txTail++;
    
    if (uart1_obj.txTail == (uart1_txByteQ + UART1_CONFIG_TX_BYTEQ_LENGTH))
    {
        uart1_obj.txTail = uart1_txByteQ;
    }

    uart1_obj.txStatus.s.empty = false;

    if (uart1_obj.txHead == uart1_obj.txTail)
    {
        uart1_obj.txStatus.s.full = true;
    }
	UART1_Enable_Tx_Int();
}


unsigned int UART1_WriteBuffer( const uint8_t *buffer , const unsigned int bufLen )
{
    unsigned int numBytesWritten = 0 ;

    while ( numBytesWritten < ( bufLen ))
    {
        if((uart1_obj.txStatus.s.full))
        {
            break;
        }
        else
        {
            UART1_Write (buffer[numBytesWritten++] ) ;
        }
    }

    return numBytesWritten ;

}


UART1_TRANSFER_STATUS UART1_TransferStatusGet (void )
{
    UART1_TRANSFER_STATUS status = 0;

    if(uart1_obj.txStatus.s.full)
    {
        status |= UART1_TRANSFER_STATUS_TX_FULL;
    }

    if(uart1_obj.txStatus.s.empty)
    {
        status |= UART1_TRANSFER_STATUS_TX_EMPTY;
    }

    if(uart1_obj.rxStatus.s.full)
    {
        status |= UART1_TRANSFER_STATUS_RX_FULL;
    }

    if(uart1_obj.rxStatus.s.empty)
    {
        status |= UART1_TRANSFER_STATUS_RX_EMPTY;
    }
    else
    {
        status |= UART1_TRANSFER_STATUS_RX_DATA_PRESENT;
    }
    return status;
}


uint8_t UART1_Peek(uint16_t offset)
{
    if( (uart1_obj.rxHead + offset) > (uart1_rxByteQ + UART1_CONFIG_RX_BYTEQ_LENGTH))
    {
      return uart1_rxByteQ[offset - (uart1_rxByteQ + UART1_CONFIG_RX_BYTEQ_LENGTH - uart1_obj.rxHead)];
    }
    else
    {
      return *(uart1_obj.rxHead + offset);
    }
}


unsigned int UART1_ReceiveBufferSizeGet(void)
{
    if(!uart1_obj.rxStatus.s.full)
    {
        if(uart1_obj.rxHead > uart1_obj.rxTail)
        {
            return(uart1_obj.rxHead - uart1_obj.rxTail);
        }
        else
        {
            return(UART1_CONFIG_RX_BYTEQ_LENGTH - (uart1_obj.rxTail - uart1_obj.rxHead));
        } 
    }
    return 0;
}


unsigned int UART1_TransmitBufferSizeGet(void)
{
    if(!uart1_obj.txStatus.s.full)
    { 
        if(uart1_obj.txHead > uart1_obj.txTail)
        {
            return(uart1_obj.txHead - uart1_obj.txTail);
        }
        else
        {
            return(UART1_CONFIG_TX_BYTEQ_LENGTH - (uart1_obj.txTail - uart1_obj.txHead));
        }
    }
    return 0;
}


bool UART1_ReceiveBufferIsEmpty (void)
{
    return(uart1_obj.rxStatus.s.empty);
}


bool UART1_TransmitBufferIsFull(void)
{
    return(uart1_obj.txStatus.s.full);
}


UART1_STATUS UART1_StatusGet (void)
{
    return U1STA;
}



/**
  End of File
*/
