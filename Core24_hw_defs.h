/////////////////////////////////////////////////////////////////////////
////                          Core24_hw_defs.H                              ////
////         
//// This file is included in Core24.c before any user includes                                                        ////
/////////////////////////////////////////////////////////////////////////
#ifndef CORE24_HW_DEFS_H
#define CORE24_HW_DEFS_H
//#use delay(clock=20000000)
//#USE rs232(baud=4800, ERRORS, UART1, stream=COMPASS)
//#USE rs232(baud=4800, ERRORS, UART2, stream=GPS)
//#USE i2c(master, SDA=PIN_B9, SCL=PIN_B8, FORCE_HW, FAST)
//#USE fast_io(ALL)

#define CNFG_FLASH_ADDR	0x14C00
#define CNFG_FLASH_PAGE_SIZE 0x400
#define CNFG_FLASH_2_ADDR 0x15000
#define CNFG_FLASH_ADDR_MASK 0x3FFFF

//#pin_select U1RX=PIN_C2
//#pin_select U1TX=PIN_C1
//#pin_select U2RX=PIN_B4
//#pin_select U2TX=PIN_C0
//#pin_select OC1=PIN_B0
//#pin_select OC2=PIN_B1

//////////////////////////////////////////////////////////////
//control register address definitions
//////////////////////////////////////////////////////////////

// T2 internal, divide by 8
#define T2CON_INIT 0x8020

//#Bit I2C_RESET = PORTC.6

#define OC_PWM_ON 0x06
#define OC_OFF 0x00
#define OC_LOW_TO_HIGH 0x01
#define OC_HIGH_TO_LOW 0x02
#define OC_TMR2 0x00
#define OC_TMR3 0x08

//#word NVMCON = 0x0760
//#Bit NVMCON_WR = NVMCON.15
//#Bit NVMCON_WREN = NVMCON.14
//#Bit NVMCON_WRERR = NVMCON.13
//#Bit NVMCON_ERASE = NVMCON.6
#define NVMCON_PAGE_ERASE 0x0002
#define NVMCON_WORD_PROGRAM 0x0003

//#word NVMKEY = 0x0766

//#word TBLPAG = 0x0032



// Input Pin defines
#define  RP0      0
#define  RP1      1
#define  RP2      2
#define  RP3      3
#define  RP4      4
#define  RP5      5
#define  RP6      6
#define  RP7      7
#define  RP8      8
#define  RP9      9
#define  RP10     10
#define  RP11     11
#define  RP12     12
#define  RP13     13
#define  RP14     14
#define  RP15     15
#define  RP16     16
#define  RP17     17
#define  RP18     18
#define  RP19     19
#define  RP20     20
#define  RP21     21
#define  RP22     22
#define  RP23     23
#define  RP24     24
#define  RP25     25
#define  RP26     26
#define  RP27     27
#define  RP28     28
#define  RP29     29
#define  RP30     30
#define  RP31     31

#define	 PIO_LOCK_BIT	#6
#define  PIO_LOCK OSCCONbits.IOLOCK

///////////////////////////////////////////////////////////////////
// *********************************************************************
// * IO Port Configuration Settings
// *
// * defines masks to configure functions of the io ports
// *********************************************************************


//* Port A Configuration
// Bit 0: Input, Analog input from wind angle aneneometer
// Bit 1: Output, Piezo Buzzer
// Bit 2: Input, OSCI
// Bit 3: Input, OSCO                  Summary 0b1111, 0xF
// Bit 4: Input, not used
// Bit 5: Input, N/A
// Bit 6: Input, N/A
// Bit 7: Output, 'Tgt Panel LED'      Summary 0b0111, 0x7
// Bit 8: Input, not used
// Bit 9: Output, ISD_SS
// Bit 10: Output, 'BTN_ROW_3'
// Bit 11: Input, N/A                  Summary 0b1111, 0xD
// Bit 12: Input, N/A
// Bit 13: Input, N/A
// Bit 14: Input, N/A
// Bit 15: Input, N/A                  Summary 0b1111, 0xF
#define  TRISA_INIT        0xF97C
#define  PORTA_INIT        0x0200      // clear all bits
#define  ODCA_INIT         0x0200      // open drain config

//* Port B Configuration
// Bit 0: Output, 'ISD_MOSI'
// Bit 1: Input, 'ISD_MISO'
// Bit 2: Output, 'AMP_DPOT_CS'
// Bit 3: Output, 'AMP_DPOT_UD'       Summary 0b0001, 0x2
// Bit 4: Input, UART1 Rx, 'RS232_2_RX'
// Bit 5: Input, PGD3
// Bit 6: Input, PGC3
// Bit 7: Input, 'ISD_INT'            Summary 0b1111, 0xF
// Bit 8: Output, SCL1, 'SCL'
// Bit 9: Output, SDA1, 'SDA'
// Bit 10: Input, 'BTN_IN_3'
// Bit 11: Output, 'BTN_ROW_0'     Summary 0b1111, 0x4
// Bit 12: Output, 'BTN_ROW_1'
// Bit 13: Output, 'BTN_ROW_2'
// Bit 14: Input, 'ISD_RAC'
// Bit 15: Output, 'ISD_SCLK'         Summary 0b0111, 0x4
#define  TRISB_INIT        0x44F2
#define  PORTB_INIT        0x3000      // clear all bits, except Oc1,Oc2
#define  ODCB_INIT         0x0080      // open drain config

//* Port C Configuration
// Bit 0: Output, RS232_2_TX
// Bit 1: Output, 'RS232_1_TX'
// Bit 2: Input, 'RS232_1_RX'
// Bit 3: Output, monitor led          Summary 0b0100, 0x4
// Bit 4: Input, RS232_1_RX_ALT
// Bit 5: Input, Not Used
// Bit 6: Output, 'I2C_RESET'
// Bit 7: Input, 'BTN_IN_0'          Summary 0b1011, 0xA
// Bit 8: Input, 'BTN_IN_1'
// Bit 9: Input, 'BTN_IN_2'
// Bit 10: Input, Not Used
// Bit 11: Input, Not Used             Summary 0b1111, 0xF
// Bit 12: Input, Not Used
// Bit 13: Input, Not Used
// Bit 14: Input, Not Used
// Bit 15: Input, N/A                  Summary 0b1111, 0xF
#define  TRISC_INIT        0xFFB4               
#define  PORTC_INIT        0x0000      // Clear all bits
#define  ODCC_INIT         0x0040      // open drain config

#define TMR1_PRESCALER   1
#define CLOCKRATE (32000000L)
#define FCY (CLOCKRATE/2)
#define INTERRUPT_RATE 1000L
#define ISR_1MS_INIT INTERRUPT_RATE / 1000
// Set up timer0 so that we get an interrupt every 1 mS
// 8mhz / (4 * 2 * 125) = 8000
// TIMER0_INIT = CLOCKRATE/(4*TMR0_PRESCALER*INTERRUPT_RATE)

#define TIMER1_INIT   CLOCKRATE/(2*TMR1_PRESCALER*INTERRUPT_RATE)   

// *********************************************************************
// * Interrupt Configuration Settings
// *
// * defines mask to enable interrupts
// *********************************************************************
// * GIE                          EQU     H'0007'   //  bit 7, Global Int enable
// * PEIE                         EQU     H'0006' //  bit 6, Peripheral int enable
// * T0IE                         EQU     H'0005' //  bit 5, Timer int enable
// * INTE                         EQU     H'0004' //  bit 4, Ext int enable 
// * RBIE                         EQU     H'0003' //  bit 3, 
// * T0IF                         EQU     H'0002' //  bit 2, Timer int flag
// * INTF                         EQU     H'0001' //  bit 1, Ext int flag
// * RBIF                         EQU     H'0000' //  bit 0, 

#define INTCON_MASK   0b00100000   //  enable TMR0


// *********************************************************************
// * Fatal Error (FE) Code definitions
// * 
// * FE Codes defined here
// *********************************************************************
// * 

#define     RUNQUE_OVERFLOW         2
#define     RUNQUE_EMPTYQUE         3
#define     FE_1MSQUE_OVERFLOW      4
#define     FE_10MSQUE_OVERFLOW     5
#define     FE_100MSQUE_OVERFLOW    6
#define     FE_1SECQUE_OVERFLOW     7
#define     FE_10SECQUE_OVERFLOW    8
#define     FE_UNKNOWN_TASK         9
#define     USER_ERRORS             10

// ********************************************************************
// * Led Monitor Services and i/o bit definitions
// ********************************************************************
#define   MONITOR_LED_MASK  0b1000000000
#define   MONITOR_LED_BIT   RA9
#define   MONITOR_LED_PORT  PORTAbits
#define   MONITOR_LED 		MONITOR_LED_PORT.MONITOR_LED_BIT

// * Led Monitor Pulse codes
#define   LED_PULSE_OK            2*2
#define   LED_PULSE_ISD_PLAY      3*2
#define   LED_PULSE_ISD_REC         4*2
#define   LED_PULSE_ISD_PLAYING_MSG     5*2
#define   LED_PULSE_ISD_RECORDING_MSG   6*2

// * Led Monitor period in seconds
#define   LED_PULSE_PERIOD_INIT   3

#endif
