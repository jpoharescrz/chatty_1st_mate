/////////////////////////////////////////////////////////////////////////
////                          Display_03.c                             ////
////    
#include <xc.h>
#include <p24FJ128GB204.h>
#include <string.h>
#include "Core_api.h"
#include "display_data_defs.h"
#include "button_api.h"
#include "i2c_api.h"

// Internal prototypes
void Disp_ClearBarDisplay();
void Disp_SetLvlDgtDsply( int8 level );
void Disp_SetLvlBarDsply( int8 level );
void InitDsplyDimTask();
void InitDsplyDimmers();
void Disp_NoSignalDgtDisplayImg();	// display 'OFF' on digit display

#define 	DGT_DISP_ADDR				0x01
#define		DGT_DISP_CMD_UPD_DIGITS		1
#define		DGT_DISP_CMD_LEVEL			2

#define		BAR_DISP_ADDR				0x02
#define		BAR_DISP_CMD_UPD_DIGITS		1
#define		BAR_DISP_CMD_LEVEL			2

//temporary stumps
int i2c_Send(int i, int j, char *p){ return(1); };


void Disp_InitTask()
{ 
 int i;

	for (i=0; i<DISP_REG_MAX_ADDR+1; i++)
	{  	
	 Disp_Ctl_union.registers[i] = 0;
 	}
	Disp_Ctl_union._disp_ctl_blk.reg_idx = 0;
	Disp_Ctl_union._disp_ctl_blk.ProcState = DISP_STT_INITIALIZING;   
	Disp_Ctl_union._disp_ctl_blk.powerup_dly = 20;
	Disp_Ctl_union._disp_ctl_blk.targeting = 0;
	Disp_Ctl_union._disp_ctl_blk.target_offset = 0;
	
	// Schedule task to run every 200 ms
 	// Put100MsSchedRtn( (PROC_DSPLYTASK | 0x80), 2 );	
    Core_SchedTask(_100MS_QUE, (PROC_DSPLYTASK | 0x80), 2, SGNL_TIMER_MASK, 2);

 	Disp_NoSignalDgtDisplayImg();	// display '---' on digit display
}

char *Disp_GetDigDsplyStr()
{
 	return( digDsplyAscii );
}

int8 Disp_GetBarDsplyLed()
{
 	return( 0 );
}

int8 *Disp_GetBarDsplyAddr()
{
 	return( bar_dsply );
}

int8 Disp_GetBarDsplyLevel()
{
 int8 i;

 	i = BarDsplyLevel;
 	return( i );
}

int8 Disp_GetDgtDsplyLevel()
{
 int8 i;

 	i = DgtDsplyLevel;
 	return( i );
}

////////////////////////////////////////////////////////////////////////////////////
// bar display organization
//
// The bar display consists of three pc boards each holding a rectangular
// array of leds (16 x 4). Bits are shifted into each board, 32 bits per
// board with each bit driving either two inner vertical leds or two outer vertical leds
// The display board are connected serially to each other from
// left to right as the user looks at the display. Hence, a total of 3 x 32 = 96
// bits are shifted into the bar display each time a new HDM msg is received
// (~100 ms). A 96 bit image is kept in bar_dsply[12];
// 
//////////////////////////////////////////////////////////////////////////////////// 
void Disp_ClearBarDisplay()
{
	Disp_Ctl_union._disp_ctl_blk.sweep = FALSE;
}

////////////////////////////////////////////////////////////////////////////////////
// void Disp_SetBarLed(uint16 led_pos)
//
// Description: Takes a bit position in led_pos and sets the corresponding bit in
//    bar_dsply
//   The lower 6 bits designates the led postion from 0-63
//   Bit 6 specifies the inner two leds to be set.
//   Bit 7 specifies the outer two leds to be set.
////////////////////////////////////////////////////////////////////////////////////
void Disp_SetBarLed(uint16 led_pos)
{

}

////////////////////////////////////////////////////////////////////////////////////
// void Disp_SetBarLeds(int8 led_pos, int8 count, int8 port)
//
// Description: Sets a range of bar display leds.
//    Takes a bit position in 'led_pos' and sets the corresponding bits in
//   bar_dsply for 'count' leds in the direction spec'd in 'port'
//   If 'port' is TRUE then leds are set from 'led_pos' to the left.
//   The lower 6 bits of 'led_pos' designates the led postion from 0-63
//   Bit 6 specifies the inner two leds to be set.
//   Bit 7 specifies the outer two leds to be set.
//   
////////////////////////////////////////////////////////////////////////////////////
void Disp_SetBarLeds(int8 led_pos, int8 count, int8 port )
{

}
////////////////////////////////////////////////////////////////////////////////////
// void Disp_ClrBarLed(int8 led_pos)
//
// Description: Takes a bit position in led_pos and clears the corresponding bit in
//    bar_dsply
//   The lower 6 bits designates the led postion from 0-63
//   Bit 6 specifies the inner two leds to be clear.
//   Bit 7 specifies the outer two leds to be clear.
////////////////////////////////////////////////////////////////////////////////////
void Disp_ClrBarLed(uint16 led_pos)
{

}

////////////////////////////////////////////////////////////////////////////////////
// void Disp_ClrBarLeds(int8 led_pos, int8 count, int8 port)
//
// Description: Clears a range of bar display leds.
//    Takes a bit position in 'led_pos' and sets the corresponding bits in
//   bar_dsply for 'count' leds in the direction spec'd in 'port'
//   If 'port' is TRUE then leds are cleared from 'led_pos' to the left.
//   The lower 6 bits of 'led_pos' designates the led postion from 0-63
//   Bit 6 specifies the inner two leds to be cleared.
//   Bit 7 specifies the outer two leds to be cleared.
//   
////////////////////////////////////////////////////////////////////////////////////
void Disp_ClrBarLeds(int8 led_pos, int8 count, int8 port )
{

}
////////////////////////////////////////////////////////////////////////////////////
// void Disp_SetBarInnerLed(int8 led_pos)
//
// Description: Takes a bit position in led_pos and sets the corresponding bit in
//    bar_dsply
//   The lower 6 bits designates the led postion from 0-63
//  This function assumes the two inner leds get set
//   
////////////////////////////////////////////////////////////////////////////////////
void Disp_SetBarInnerLed(uint16 led_pos)
{

}

////////////////////////////////////////////////////////////////////////////////////
// void Disp_SetBarOuterLed(int8 led_pos)
//
// Description: Takes a bit position in led_pos and sets the corresponding bit in
//    bar_dsply
//   The lower 6 bits designates the led postion from 0-63
//  This function assumes the two outer leds get set
//   
////////////////////////////////////////////////////////////////////////////////////
void Disp_SetBarOuterLed(uint16 led_pos)
{
 
}

//////////////////////////////////////////////////////////////////
// char *GetDsplyAsciiStr( int16 i, char *str )
//
// Converts the number i to a 3 digit ascii string
//////////////////////////////////////////////////////////////////
char *GetDsplyAsciiStr( int16 i, char *str )
{
 int16 j;

 	j=i;
 	str[0] = (i / 100) | 0x30;
 	i -= (i /100)*100; 
 	str[1] = (i / 10) | 0x30;
 	i -= (i /10)*10;
 	str[2] = (i) | 0x30;
 	str[3] = 0;

 	return(str);
}

char *Disp_UpdtDgtDisplayImage( int16 i )
{
	Disp_Ctl_union._disp_ctl_blk.targeting = 0;
	Disp_Ctl_union._disp_ctl_blk.heading = i;
	Disp_Ctl_union._disp_ctl_blk.signal_active = TRUE;
	SignalTask( PROC_DSPLYTASK, SGNL_UPD_DISP_BIT );
 	return( GetDsplyAsciiStr( i, digDsplyAscii ));
}

void Disp_NoSignalDgtDisplayImg()
{
	Disp_Ctl_union._disp_ctl_blk.signal_active = FALSE;
	Disp_Ctl_union._disp_ctl_blk.targeting = 0;
	SignalTask( PROC_DSPLYTASK, SGNL_UPD_DISP_BIT );
}

//////////////////////////////////////////////////////////////
// Adjusts heading or wind angle according to resolution
// setting.
// Input format is XXXY
/////////////////////////////////////////////////////////////
int16 Disp_hdg_res_adjust( int16 heading )
{
 int8 i;

 	i = Btn_GetParm(BTN_PARM_RESOLUTION_IDX);

 	// adjust for bar display resolution mode
 	switch ( i )
 	{
  	case 0:   // low resolution
   		heading = heading >> (HDG_VALUE_RES_SHIFT + 2);
   		break;
  	case 1:   // med resolution
   		heading = heading >> (HDG_VALUE_RES_SHIFT + 1);
   		break;
  	case 2:   // high resolution
    	heading = heading >> (HDG_VALUE_RES_SHIFT);
  	default:
  		break;
 	}
 	return( heading );
}

void _disp_register_init()
{
	Disp_Ctl_union._disp_ctl_blk.brightness = Btn_GetParm(BTN_PARM_LVL_DGT_DISP_IDX);
}

void Disp_CnfgSweep(int16 hdg_roc)
{
 int16 temp;

	temp = hdg_roc;
	if (temp < 0)
	{
		Disp_Ctl_union._disp_ctl_blk.sweep_rt = 1;
		temp *= -1;
	}
	else
	{
		Disp_Ctl_union._disp_ctl_blk.sweep_rt = 0;
	}
	temp = MinInt16(temp, 63);
	Disp_Ctl_union._disp_ctl_blk.sweep_rate = temp ;
}

uint8 _disp_update_i2c_regs()
{
 uint8 i, i2c_reg_value;

	if (I2C_Task_Rdy() && RsrcIsLockFree( RSRC_I2C ))
	{
		RsrcSetLockOwner( RSRC_I2C, PROC_DSPLYTASK );
		// Write all the registers to the display
		for( i=DISP_REG_ADDR_0; i<DISP_REG_MAX_ADDR+1; i++)
		{
			i2c_reg_value = Disp_Ctl_union.registers[i];
			I2C_Set_RegValue( DISP_I2C_ADDR, i, i2c_reg_value);
		}
		RsrcFreeLock( RSRC_I2C );
		return( TRUE );
	}
	return( FALSE );
}

#define NO_DGT_DSPLY 0
///////////////////////////////////////////////////////////////////////////////
// void Disp_ProcTask()
//
// Updates the display
// Updates all the display registers via I2C
// 
// This is a signalled task with states so that it doesn't hog the cpu
// for too long.
///////////////////////////////////////////////////////////////////////////////
int8 Disp_ProcTask( int8 signal )
{
 int8 RtnSignal;
 
 	RtnSignal = signal;

	switch ( Disp_Ctl_union._disp_ctl_blk.ProcState )
	{
		case DISP_STT_INITIALIZING:
			
			if (signal & SGNL_TIMER_MASK)
			{
				if ( Disp_Ctl_union._disp_ctl_blk.powerup_dly )
					Disp_Ctl_union._disp_ctl_blk.powerup_dly--;
				else
				{
					if (I2C_Task_Rdy() && RsrcIsLockFree( RSRC_I2C ))
					{
						RsrcSetLockOwner( RSRC_I2C, PROC_DSPLYTASK );
						if (I2C_SlaveAttached( DISP_I2C_ADDR ))
						{
							Disp_Ctl_union._disp_ctl_blk.attached = TRUE;
							Disp_Ctl_union._disp_ctl_blk.ProcState = DISP_STT_READ_FW_REV;
							_disp_register_init();
						}
						else
						{
							Disp_Ctl_union._disp_ctl_blk.attached = FALSE;	
							Disp_Ctl_union._disp_ctl_blk.ProcState = DISP_STT_NOT_PRESENT;
							//DelTaskFrom100MsQue( PROC_DSPLYTASK );
                            Core_DelTaskFromSchedQue(_100MS_QUE, PROC_DSPLYTASK );
						}
						RsrcFreeLock( RSRC_I2C );
					}
				}	
			}
			RtnSignal &= _SGNL_TIMER_MASK;
			break;

		case DISP_STT_READ_FW_REV:

			if (signal & SGNL_TIMER_MASK)
			{
				if (I2C_Task_Rdy() && RsrcIsLockFree( RSRC_I2C ))
				{
					RsrcSetLockOwner( RSRC_I2C, PROC_DSPLYTASK );
					//uint8 I2C_Get_RegValues( int8 SlaveAddr, int8 Reg, int8 cnt, int8 TaskId, uint8 TaskSgnl, uint8 *read_ptr );
					I2C_Get_RegValues( DISP_I2C_ADDR, DISP_FW_REV_REG_ADDR, 2, PROC_DSPLYTASK, SGNL_UPD_DISP_BIT, (uint8 *)&Disp_Ctl_union._disp_ctl_blk.fw_rev );
					RsrcFreeLock( RSRC_I2C );
				}
				RtnSignal &= _SGNL_TIMER_MASK;
			}

			if (signal & SGNL_UPD_DISP_MASK)
			{
				Disp_Ctl_union._disp_ctl_blk.ProcState = DISP_STT_RUNNING;
				RtnSignal &= _SGNL_UPD_DISP_MASK;
			}
			break;

		case DISP_STT_RUNNING:

			if (signal & SGNL_UPD_DISP_MASK)
			{
				_disp_update_i2c_regs();
				RtnSignal &= _SGNL_UPD_DISP_MASK;
			}
			if (signal & SGNL_TIMER_MASK)
			{
					RtnSignal &= _SGNL_TIMER_MASK;
			}
			break;
	}
 	
 	return( RtnSignal );
}

///////////////////////////////////////////////////////////////////////////////
// void InitDsplyDimmers()
//
// Initializes the digit and bar display dimmers which are the
// PWM functions in CCP1 and CCP2 respectively.
//////////////////////////////////////////////////////////////////////
void InitDsplyDimmers()
{   
  	Disp_SetLvlDgtDsply( Btn_GetParm(BTN_PARM_LVL_DGT_DISP_IDX) );
  	Disp_SetLvlBarDsply( Btn_GetParm( BTN_PARM_LVL_BAR_DISP_IDX ) );
}

/////////////////////////////////////////////////////////////////////
// void Disp_SetLvlDgtDsply( int8 level )
//
// Sets the dimming level for the digit display
// The bigger the duty cycle ( high period ) for the PWM1 the
// dimmer the display
/////////////////////////////////////////////////////////////////////
void Disp_SetLvlDgtDsply( int8 level )
{	
 	if (level < DISP_DGT_LVL_MAX+1)
 	{
 		Disp_Ctl_union._disp_ctl_blk.brightness = level;
	}
	SignalTask( PROC_DSPLYTASK, SGNL_UPD_DISP_BIT );
}

/////////////////////////////////////////////////////////////////////
// void Disp_SetLvlBarDsply( int8 level )
//
// Sets the dimming level for the Bar display
// The bigger the duty cycle ( high period ) for the PWM2 the
// dimmer the display
/////////////////////////////////////////////////////////////////////
void Disp_SetLvlBarDsply( int8 level )
{
 	if (level < DISP_DGT_LVL_MAX+1)
 	{
 		Disp_Ctl_union._disp_ctl_blk.brightness = level;
	}
	SignalTask( PROC_DSPLYTASK, SGNL_UPD_DISP_BIT );
}

void Disp_UpdtDsplyTgtImage( int16 tgt_heading, int16 tgt_offset, int16 delta )
{
	Disp_Ctl_union._disp_ctl_blk.heading = tgt_heading;
	Disp_Ctl_union._disp_ctl_blk.targeting = 1;
	Disp_Ctl_union._disp_ctl_blk.target_offset = tgt_offset;
}



