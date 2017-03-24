/////////////////////////////////////////////////////////////////////////
////                          I2C_Master_c.h                              
////                                                                 
//// Handles the I2C transactions
//// Currently all I2C transactions are writes
//// Commands are seen as a number of bytes to write to the transmit register
//void __attribute__ ((section (".libperi")))OpenI2C1(unsigned int config1,unsigned int config2) ;
//////////////////////////////////////////////////////////////////////////
#ifndef _I2C_V2_1
#define _I2C_V2_1
#endif

#include <xc.h>
#include <p24FJ128GB204.h>
#include "ADSC_LITE_0.X/mcc_generated_files/i2c1.h"
#include "Core_api.h"
#include "I2C_Cnfg.h"
#include "I2C_data_defs.h"
#include "I2C_api.h"

// Internal prototypes
void I2C1_Reset_local();

void I2C_InitTask()
{
 	// Reset TTS on the I2C bus
 	//I2C1_Reset_local();
 	I2C1_RESET = 0;
 	I2C_CtrlBlk.HwLockUp = FALSE;
 	I2C_CtrlBlk.HwLockUpCnt = 0;
 	I2C1CONL = 0;
 	PORTB &= 0xFCFF;
 	I2C1CONL = I2C_CONFIG_INIT;
 	I2C1BRG = I2C_CNFG_BRG;

 	I2C_CtrlBlk.I2cTaskState = I2C_STT_POWERUP;

 	// Get the I2C_ProcTask() running in 50 ms
 	//Put10MsSchedRtn( PROC_I2CTASK, 5 );
    Core_SchedTask(_10MS_QUE, (PROC_I2CTASK), 5, SGNL_TIMER_MASK, 0);

 	I2C_CtrlBlk.I2cPupTimer = 0;

 	I2C_CtrlBlk.TxCmdCnt = 0;
 	I2C_CtrlBlk.TxCmdRdIdx = 0;
 	I2C_CtrlBlk.TxCmdWtIdx = 0;

 	I2C_CtrlBlk.TxByteCnt = 0;
 	I2C_CtrlBlk.TxBufRdIdx = 0;
 	I2C_CtrlBlk.TxBufWtIdx = 0;
}

uint8 I2C_Task_Rdy()
{
	if (I2C_CtrlBlk.I2cTaskState != I2C_STT_POWERUP)
	{
		return(TRUE);
	}
	else
	{
		return(FALSE);	
	}
}
void I2C1_Reset_local()
{
 	// Pulse the I2C reset line
 	I2C1_RESET = 0;
 	Delay_ms( 30 );
 	I2C1_RESET = 1; 
}

void i2c1_ack(uint8 ack)
{
    if (ack)
        I2C1_ACKDATA = 1;
    else
        I2C1_ACKDATA = 0;
    
     // Initiate the acknowledge
    I2C1_ACKDATA_EN = 1;
}

uint8 i2c1_read_byte( uint8 ack )
{
 int8 wait_cntr = 25;
 uint8 result;
 
	I2C1CONL |= I2C_RCEN_MASK;
	while ((I2C1CONL & I2C_RCEN_MASK) && wait_cntr)
	{
		Delay_us(10);
		wait_cntr--;
	}
	if (!wait_cntr) 
	{
		I2C_CtrlBlk.HwLockUp = TRUE;
		return(0);
	}
	I2C1STAT &= _I2C_I2COV_MASK;
	result = I2C1RCV;
	
    i2c1_ack( ack );
    
	return( result );
}
		
void i2c_abort_chk()
{
	if (I2C_CtrlBlk.i2c_abort_cntr)
	{
		I2C_CtrlBlk.i2c_abort_cntr--;
	}
	else
	{
		i2c1_stop();
		I2C_CtrlBlk.I2cTaskState = I2C_STT_WAITING;
	}	
}

int8 i2c_bus_collision_chk()
{
	if (I2C1_BCL) // if there's a bus collision....
    {
    	I2C_CtrlBlk.I2cTaskState = I2C_STT_WAITING;
        i2c1_disable();
		i2c1_clr_sda_scl();     // clear SDA & SCL
		I2C_CtrlBlk.HwLockUpCnt++;
    	I2C1_Reset_local();
    	I2C1CONL = I2C_CONFIG_INIT;
    	i2c1_stop();
        I2C1_BCL = 0;   // clear the bus collision status bit
    	return( TRUE );
    }	
    return( FALSE );
}

//////////////////////////////////////////////////////////////////
// I2C_ProcTask( int8 signal )
//
// Signalled every 50 ms
// Check the TxBuf for stuff to send and gets it over to the 
// slave, then 
// check the slave's txbuf[] for stuff to receive and gets it
// into the receive buffer.
//////////////////////////////////////////////////////////////////
int8 I2C_ProcTask( int8 signal )
{
 int8 RtnSignal;

 	RtnSignal = signal;
		
 	switch( I2C_CtrlBlk.I2cTaskState )
 	{
  		case I2C_STT_POWERUP:
  		
    		if ( I2C_CtrlBlk.I2cPupTimer < I2C_POWERUP_CNT )
    		{
      			I2C_CtrlBlk.I2cPupTimer++;
      			// Get the I2C_ProcTask() running in 50 ms
 				// Put10MsSchedRtn( PROC_I2CTASK, 5 );
                Core_SchedTask(_10MS_QUE, (PROC_I2CTASK), 5, SGNL_TIMER_MASK, 0);
 			}
    		else
    		{
	    		I2C1_RESET = 1;
      			I2C_CtrlBlk.I2cTaskState = I2C_STT_WAITING;
      		}				

   			RtnSignal &= _SGNL_TIMER_MASK;
    		break;

  		case I2C_STT_WAITING:
    		// If we have something to do, go do it.
    		if ( I2C_CtrlBlk.TxCmdCnt && I2C1_IDLE_OK())
    		{
	    		I2C_CtrlBlk.i2c_abort_cntr = 100;
	    		I2C1_BCL = 0;     // Clear BCL, Bus Collision Detect bit
                // send the start condition
                i2c1_start();
	    		
    			I2C_CtrlBlk.CurCmdPtr = &I2C_CtrlBlk.TxCmdRecords[ (int) I2C_CtrlBlk.TxCmdRdIdx ];
      				
    			I2C_CtrlBlk.I2cTaskState = I2C_STT_SEND_ADDR;
    		}
    		break;
    		
 		case I2C_STT_SEND_ADDR:
    		//if the Tx buffer is empty
    		if ( I2C1_TBF == 0 )
    		{ 
     			i2c1_write( I2C_CtrlBlk.CurCmdPtr->SlaveAddr & 0xFE);
     			I2C_CtrlBlk.I2cTaskState = I2C_STT_SEND_ADDR_ACK;
    		}
    		break;
    		
    	case I2C_STT_SEND_ADDR_ACK:
    		
    		if (i2c_bus_collision_chk()) break;
    			
    		if (( I2C1_TRSTAT == 0 ) && (I2C1_ACKSTAT))
    		{	
	    		i2c1_write( I2C_CtrlBlk.CurCmdPtr->Reg );
	    		I2C_CtrlBlk.I2cTaskState = I2C_STT_SEND_REG_ACK;
	    	}
	    	else
	    	{
		    	i2c_abort_chk();
		    }
	    	break;
	    	
	    case I2C_STT_SEND_REG_ACK:
	    
	    	if (i2c_bus_collision_chk()) break;
    		
   			if (( I2C1_TRSTAT == 0 ) && (I2C1_ACKSTAT) )
    		{
	    		switch ( I2C_CtrlBlk.CurCmdPtr->CmdType )
	    		{
		    		case I2C_CMD_WRT_BYTES:
		    			I2C_CtrlBlk.I2cTaskState = I2C_STT_WRITE_BYTES;
		    			break;
		    		
		    		case I2C_CMD_READ_REG8_BYTES:
		    			if (I2C1_IDLE_OK())
		    			{
		    				i2c1_restart();
		    				I2C_CtrlBlk.I2cTaskState = I2C_STT_SEND_READ_ADDR;
		    			}
		    			break;
		    		
		    		default:
		    			I2C_CtrlBlk.I2cTaskState = I2C_STT_STOP;
		    			break;
		    	}
    		}
    		
    		break;

  		// We have stuff to send
  		case I2C_STT_WRITE_BYTES:
  		
  			if (i2c_bus_collision_chk()) break;
  			
    		// if we're not done sending
    		if ( I2C1_TRSTAT )
      			break;

    		if ((I2C_CtrlBlk.CurCmdPtr->ByteCnt > 0) && I2C1_ACKSTAT && I2C1_IDLE_OK())
    		{
     			I2C_CtrlBlk.TxByteCnt--;
     			I2C_CtrlBlk.CurCmdPtr->ByteCnt--;
     			if (!I2C_CtrlBlk.CurCmdPtr->ByteCnt)
      				I2C_CtrlBlk.I2cTaskState = I2C_STT_STOP_WRITING;
				
     			i2c1_write( I2C_CtrlBlk.TxBuf[ (int)(I2C_CtrlBlk.TxBufRdIdx) ]);
     			I2C_CtrlBlk.TxBufRdIdx++;
     			if ( I2C_CtrlBlk.TxBufRdIdx > (I2C_TX_BUF_SIZE-1)) 
       				I2C_CtrlBlk.TxBufRdIdx = 0;
    		}
    		else // This shouldn't happen
    		{
     			I2C_CtrlBlk.I2cTaskState = I2C_STT_STOP_WRITING;
    		}
    		break;

  		case I2C_STT_STOP_WRITING:
  		
  			if (i2c_bus_collision_chk()) break;
  			
    		// if we're not done sending
    		if (( I2C1_TRSTAT ) || (!(I2C1_ACKSTAT)))
      			break;
      			
      		I2C_CtrlBlk.I2cTaskState = I2C_STT_STOP;
    		break;
    		
    	case I2C_STT_SEND_READ_ADDR:
    		//if the Tx buffer is empty
    		if (( I2C1_TBF == 0 ) && (I2C1_IDLE_OK())) 
    		{ 
     			i2c1_write( I2C_CtrlBlk.CurCmdPtr->SlaveAddr | 0x01);
     			I2C_CtrlBlk.I2cTaskState = I2C_STT_SEND_READ_ADDR_ACK;
    		}
    		break;
    		
    	case I2C_STT_SEND_READ_ADDR_ACK:
    	
    		if (i2c_bus_collision_chk()) break;
    		
    		if (( I2C1_TRSTAT == 0 ) && (I2C1_ACKSTAT))
    		{
	    		I2C_CtrlBlk.I2cTaskState = I2C_STT_INITIATE_READ;
	    	}
	    	else
	    	{
		    	i2c_abort_chk();
		    }
	    	break;
	    	
	    case I2C_STT_INITIATE_READ:
	    
	    	if (I2C_CtrlBlk.CurCmdPtr->ByteCnt)
	    	{
	    		I2C1CONL |= I2C_RCEN_MASK;
	    		I2C_CtrlBlk.read_wait_cntr = 200;
	    		I2C_CtrlBlk.I2cTaskState = I2C_STT_COMPLETE_READ;
	    	}
	    	else
	    		I2C_CtrlBlk.I2cTaskState = I2C_STT_STOP;
	    		
	    	break;
	    		    
	    case I2C_STT_COMPLETE_READ:
	    	
	    	if (!(I2C1CONL & I2C_RCEN_MASK)) // HW clears RCEN when byte rcv'd
	    	{
		    	I2C1STAT &= _I2C_I2COV_MASK;
				*(I2C_CtrlBlk.CurCmdPtr->read_ptr) = i2c1_read(); // read the data
				
				if (I2C_CtrlBlk.CurCmdPtr->ByteCnt > 1)
                    i2c1_ack( 0 );
                else
                    i2c1_ack( 1 );
		    	
		    	I2C_CtrlBlk.CurCmdPtr->read_ptr++;
		    	if (I2C_CtrlBlk.CurCmdPtr->ByteCnt)
		    	{ 
		    		I2C_CtrlBlk.CurCmdPtr->ByteCnt--;
		    		I2C_CtrlBlk.I2cTaskState = I2C_STT_INITIATE_READ;
		    	}
		    	else 
		    	{
			    	I2C_CtrlBlk.I2cTaskState = I2C_STT_STOP;
			    }
		    }
		    else
		    {
			    if (I2C_CtrlBlk.read_wait_cntr) 
			    	I2C_CtrlBlk.read_wait_cntr--;
			    else
			    {
				    i2c1_disable();
                    i2c1_clr_sda_scl();
					
					I2C_CtrlBlk.HwLockUpCnt++;
					I2C_CtrlBlk.I2cTaskState = I2C_STT_STOP;
					I2C1CONL = I2C_CONFIG_INIT;
				}
			}
			break;
		  
		case I2C_STT_STOP:
			if (I2C1_IDLE_OK())
			{
				i2c1_stop();
    			I2C_CtrlBlk.I2cTaskState = I2C_STT_WAITING;
    			I2C_CtrlBlk.TxCmdRdIdx++;

    			if ( I2C_CtrlBlk.TxCmdRdIdx > I2C_MAX_TX_CMDS-1 ) 
      				I2C_CtrlBlk.TxCmdRdIdx = 0;
      			
    			I2C_CtrlBlk.TxCmdCnt--;
    		
    			if (I2C_CtrlBlk.CurCmdPtr->TaskSgnl != 0xFF )
					SignalTask( I2C_CtrlBlk.CurCmdPtr->TaskId, I2C_CtrlBlk.CurCmdPtr->TaskSgnl );

    			if (I2C_CtrlBlk.TxCmdCnt == 0)
    			{
    				RtnSignal = SGNL_NO_REQUEUE;
    			}
   			} 		
    		break;
 	}
 	return( RtnSignal );
}

//////////////////////////////////////////////////////////////////
// void I2C_send()
//
// Increments the command count and signals the I2C Task
//////////////////////////////////////////////////////////////////
void I2C_send()
{
 	I2C_CtrlBlk.TxCmdCnt++;
 	SignalTask( PROC_I2CTASK, SGNL_GENERIC_BIT );
}

///////////////////////////////////////////////////////////////////
// void I2C_put_xmt_char( char c )
//
// Takes a character and writes it to the
// transmit buffer. Both the general tx buffer count and
// command buffer count is incremented.
///////////////////////////////////////////////////////////////////
void I2C_put_xmt_char( char c )
{

 if (I2C_CtrlBlk.TxByteCnt < I2C_TX_BUF_SIZE-1)
 {
  I2C_CtrlBlk.TxBuf[ (int) I2C_CtrlBlk.TxBufWtIdx ] = c;
  I2C_CtrlBlk.TxBufWtIdx++;
  I2C_CtrlBlk.TxByteCnt++;

  if ( I2C_CtrlBlk.TxBufWtIdx > I2C_TX_BUF_SIZE-1 ) 
    I2C_CtrlBlk.TxBufWtIdx = 0;
 }
}

///////////////////////////////////////////////////////////////////
// void I2C_put_xmt_buf( char *bptr )
//
// Takes a pointer to a character buffer an writes it to
// transmit buffer. Both the general tx buffer count and
// command buffer count is incremented.
///////////////////////////////////////////////////////////////////
void I2C_put_xmt_buf( char *bptr )
{
 
 while ((*bptr) && (I2C_CtrlBlk.TxByteCnt < I2C_TX_BUF_SIZE-1)) 
 {
   I2C_CtrlBlk.TxBuf[ (int) I2C_CtrlBlk.TxBufWtIdx ] = *bptr; bptr++;
   I2C_CtrlBlk.TxBufWtIdx++;
   I2C_CtrlBlk.TxByteCnt++;
   if ( I2C_CtrlBlk.TxBufWtIdx > I2C_TX_BUF_SIZE-1 ) 
     I2C_CtrlBlk.TxBufWtIdx = 0;
 }

}
  
//////////////////////////////////////////////////////////////////
// int8 I2C_Start_New_msg( int8 SlaveAddr, int8 CmdType, int8 TaskId, uint8 TaskSgnl, uint8 *read_ptr )
//
// Starts a new I2C msg
// Writes the ClientId to the next TxCmdRecord[].SlaveAddr
// Sets I2C_CtrlBlkRec.TxCmdCntPtr to the address of TxCmdRecord[].ByteCnt
// Increments I2C_CtrlBlkRec.TxCmdWtIdx 
// Clears the msg counter
// 
//////////////////////////////////////////////////////////////////
int8 I2C_Start_New_msg( int8 SlaveAddr, int8 Reg, int8 ByteCnt, int8 CmdType, int8 TaskId, uint8 TaskSgnl, uint8 *read_ptr )
{
 I2C_TxCmdRec *cptr;
 

 if ( I2C_CtrlBlk.TxCmdCnt < I2C_MAX_TX_CMDS )
 {
  cptr = &(I2C_CtrlBlk.TxCmdRecords[ (int) I2C_CtrlBlk.TxCmdWtIdx ]);
  cptr->SlaveAddr = SlaveAddr;
  cptr->Reg = Reg;
  cptr->ByteCnt = ByteCnt;
  cptr->CmdType = CmdType;
  cptr->TaskId = TaskId;
  cptr->TaskSgnl = TaskSgnl;
  cptr->read_ptr = read_ptr;

  I2C_CtrlBlk.TxCmdWtIdx++;
  if ( I2C_CtrlBlk.TxCmdWtIdx > (I2C_MAX_TX_CMDS-1) )
    I2C_CtrlBlk.TxCmdWtIdx = 0;
  
  return( TRUE );
 }
 else
 {
  return( FALSE );
 }
}

///////////////////////////////////////////////////////////////////
// void I2C_Set_RegValue( int8 SlaveAddr, int8 Reg, int8 Value )
// 
// Sends the msg to the slave in one atomic function
///////////////////////////////////////////////////////////////////
uint8 I2C_Set_RegValue( int8 SlaveAddr, int8 Reg, uint8 Value)
{
	if (I2C_Start_New_msg( SlaveAddr, Reg, 1, I2C_CMD_WRT_BYTES, 0, 0xFF, 0 ))
	{
		I2C_put_xmt_char( Value );
		I2C_send();
		return( TRUE );
	}
	else
		return( FALSE );
}

///////////////////////////////////////////////////////////////////
// uint8 I2C_Get_RegValue( int8 SlaveAddr, int8 Reg, uint8 *value_ptr, int8 cnt  )
// 
// Reads the slave register in one atomic function
///////////////////////////////////////////////////////////////////
uint8 I2C_Get_RegValues( int8 SlaveAddr, int8 Reg, int8 cnt, int8 TaskId, uint8 TaskSgnl, uint8 *read_ptr )
{		
	if (I2C_Start_New_msg( SlaveAddr, Reg, cnt, I2C_CMD_READ_REG8_BYTES, TaskId, TaskSgnl, read_ptr ))
	{
		I2C_send();
		return( TRUE );
	}
	else
		return( FALSE );
}

///////////////////////////////////////////////////////////////////
// int8 I2C_GetFreeByteCnt()
// 
// Returns the number of free bytes in the tx buffer
///////////////////////////////////////////////////////////////////
int8 I2C_GetFreeByteCnt()
{
 if ( I2C_CtrlBlk.TxCmdCnt < I2C_MAX_TX_CMDS )
   return( I2C_TX_BUF_SIZE - I2C_CtrlBlk.TxByteCnt );
 else
   return( 0 );
}

///////////////////////////////////////////////////////////////////
// int8 I2C_SlaveAttached( int8 SlaveAddr )
// 
// Returns true if the slave is attached
///////////////////////////////////////////////////////////////////
int8 I2C_SlaveAttached( int8 SlaveAddr )
{
	if (I2C1_IDLE_OK())
	{
		// Clear BCL
		//I2C1STAT &= 0xFBFF;
        I2C1_BCL = 0;
		
		i2c1_start();
		Delay_us(90);
		while (!I2C1_IDLE_OK()){};
		
		i2c1_write( SlaveAddr );
		Delay_us(100);
		if ((I2C1_TRSTAT == 0) && I2C1_ACKSTAT && !I2C1_BCL)
		{
			i2c1_stop();
			return( TRUE );	
		}
		while (!I2C1_IDLE_OK()){};
		i2c1_stop();
		
	}
	return( FALSE );
}


