/////////////////////////////////////////////////////////////////////////
////                          cmps03.c                              ////
////   This file handles the CMPS03 Fluxgate compass
////   
/////////////////////////////////////////////////////////////////
#define CMPS03
#include <xc.h>
#include <p24FJ128GB204.h>
#include <math.h>
#include "Core_api.h"
#include "tts_api.h"
#include "button_api.h"
#include "i2c_api.h"
#include "fmt_api.h"
#include "cmps03_api.h"
#include "rs232_api.h"

///////////////////////////////////////////////////////////////////
// void CMPS03_Init()
// 
// Initializes the Flux gate compass
///////////////////////////////////////////////////////////////////
void CMPS03_Init()
{
 
 	CMPS03_CtlBlk.TaskState = CMPS03_STT_INITIALIZING;
 	
 	strcpy(CMPS03_CtlBlk.msg_str,"HCHDG,");
	CMPS03_CtlBlk.msg_str[6] = '1';
	CMPS03_CtlBlk.msg_str[7] = '2';
	CMPS03_CtlBlk.msg_str[8] = '3';
	CMPS03_CtlBlk.msg_str[9] = '.';
	CMPS03_CtlBlk.msg_str[10] = '4';
	CMPS03_CtlBlk.msg_str[11] = ',';
	CMPS03_CtlBlk.msg_str[12] = 'M';
	CMPS03_CtlBlk.msg_str[13] = 0;
	
	// Schedule task to run every 200 ms
 	// Put100MsSchedRtn( (PROC_CMPS03_TASK | 0x80), 2 );	
    Core_SchedTask(_100MS_QUE, (PROC_CMPS03_TASK | 0x80), 2, SGNL_TIMER_MASK, 2);
} 


int8 CMPS03_ProcTask( int8 signal )
{
 int8 RtnSignal;
 uint16 raw_hdg;
 char hundreds,tens,ones,tenths;
	
	RtnSignal = SGNL_NO_REQUEUE;
	
	switch (CMPS03_CtlBlk.TaskState)
	{
		case CMPS03_STT_INITIALIZING:
			
			if (FMT_GetStatus() != FMT_STATUS_INITIALIZING )
			{
				if (I2C_SlaveAttached( CMPS03_I2C_ADDR ))
				{
					if (I2C_Task_Rdy())
					{
					 //DelTaskFrom10MsQue( PROC_CMPS03_TASK );
					 // Schedule task to run every second
 					 //Put100MsSchedRtn( (PROC_CMPS03_TASK | 0x80), 9 );
					 CMPS03_CtlBlk.TaskState = CMPS03_STT_RESET_0;
					}
				}
				else // No CMPS03, delete it from queue
				{
					//DelTaskFrom100MsQue( PROC_CMPS03_TASK );	
                    Core_DelTaskFromSchedQue(_100MS_QUE, PROC_CMPS03_TASK );
				}
			}
			break;
		
		// Start factory calibration reset sequence
		case CMPS03_STT_RESET_0:
		
			if (RsrcIsLockFree( RSRC_I2C ))
			{
				RsrcSetLockOwner( RSRC_I2C, PROC_CMPS03_TASK );
				I2C_Set_RegValue( CMPS03_I2C_ADDR, CMPS03_REG_CMD, 0x20);
				CMPS03_CtlBlk.TaskState = CMPS03_STT_RESET_1;
			}
		
			break;
		
		case CMPS03_STT_RESET_1:
		
			I2C_Set_RegValue( CMPS03_I2C_ADDR, CMPS03_REG_CMD, 0x2A);
			CMPS03_CtlBlk.TaskState = CMPS03_STT_RESET_2;
		
			break;
			
		case CMPS03_STT_RESET_2:
		
			I2C_Set_RegValue( CMPS03_I2C_ADDR, CMPS03_REG_CMD, 0x60);
			CMPS03_CtlBlk.TaskState = CMPS03_STT_READ_REGS;
		
			break;
		
		case CMPS03_STT_READ_REGS:
		
			if (signal & SGNL_TIMER_MASK)
			{
				I2C_Get_RegValues( CMPS03_I2C_ADDR, CMPS03_REG_FW_REV, 16, PROC_CMPS03_TASK, SGNL_GENERIC_BIT, CMPS03_CtlBlk.reg_values );
				RsrcFreeLock( RSRC_I2C );
			}
						
			if (signal & SGNL_GENERIC_MASK)
			{
				CMPS03_CtlBlk.TaskState = CMPS03_STT_RUNNING;
			}
			break;
			
		
		case CMPS03_STT_RUNNING:
	
			// Read the heading registers from the CMPS03
			if (signal & SGNL_TIMER_MASK)
			{
				if ( I2C_Task_Rdy() && RsrcIsLockFree( RSRC_I2C ) )
				{
					I2C_Get_RegValues( CMPS03_I2C_ADDR, CMPS03_REG_HDG_HI, 2, PROC_CMPS03_TASK, SGNL_GENERIC_BIT, CMPS03_CtlBlk.reg_values+2 );
				}
			}
	
			// Process the heading and send an HCHDG msg out port B
			if (signal & SGNL_GENERIC_MASK)
			{
				raw_hdg = CMPS03_CtlBlk.reg_values[2] << 8;
				raw_hdg |= CMPS03_CtlBlk.reg_values[3];
				
				hundreds = (raw_hdg / 1000);
				raw_hdg -= hundreds*1000;
				hundreds |= 0x30;
								
				tens = (raw_hdg / 100);
				raw_hdg -= tens*100;
				tens |= 0x30;	
								
				ones = (raw_hdg / 10);
				raw_hdg -= ones*10;
				ones |= 0x30;
				
				tenths = raw_hdg | 0x30;
				
				///////////////////////////////////
				//$HCHDG,238.1,M<CR><LF>
				///////////////////////////////////
		
				CMPS03_CtlBlk.msg_str[6] = hundreds;
				CMPS03_CtlBlk.msg_str[7] = tens;
				CMPS03_CtlBlk.msg_str[8] = ones;
				
				CMPS03_CtlBlk.msg_str[10] = tenths;
				
				RS232_Xmt_Msg( USARTB, CMPS03_CtlBlk.msg_str );
		
			}
			break;
	}
	
	return( RtnSignal );
	
}

