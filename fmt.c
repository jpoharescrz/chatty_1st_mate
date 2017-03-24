
/////////////////////////////////////////////////////////////////////////
////                          fmt.c                              ////
////   This file handles the FM transmitter
////   
/////////////////////////////////////////////////////////////////
#define FMT
#include <xc.h>

#include <p24FJ128GB204.h>
#include <math.h>
#include "Core_api.h"
#include "tts_api.h"
#include "button_api.h"
#include "i2c_api.h"
#include "fmt_api.h"
#include "fmt_data_defs.h"

///////////////////////////////////////////////////////////////////
// void FMT_Init()
// 
// Initializes the FM transmitter
///////////////////////////////////////////////////////////////////
void FMT_Init()
{
	FMT_CtlBlk.ProcState = FMT_STT_POWERUP;
	// Schedule task to run every 20 ms
 	//Put10MsSchedRtn( (PROC_FMT_TASK | 0x80), 2 );
    Core_SchedTask(_10MS_QUE, (PROC_FMT_TASK | 0x80), 2, SGNL_TIMER_MASK, 2);
}

uint16 _fmt_index_to_freq( uint8 freq_idx );
void FMT_Register_Init()
{
 uint16 freq;	
 uint8 *p;
 
	if (I2C_SlaveAttached( FM_XMTR_ADDR ))
	{
		freq = _fmt_index_to_freq( Btn_GetParm(BTN_PARM_FMT_FREQ_IDX) );
		
		I2C_Set_RegValue( FM_XMTR_ADDR, 0x0E, FM_XMTR_REGE_INIT); // SW reset
		I2C_Set_RegValue( FM_XMTR_ADDR, 0x01, FM_XMTR_REG1_INIT); // Pilot On, Forced Carrier
		I2C_Set_RegValue( FM_XMTR_ADDR, 0x02, FM_XMTR_REG2_INIT); //
		FMT_CtlBlk.Attached = TRUE;
		
		if (Cnfg_IsCnfgFlash2ParmValid( CNFG_FLASH_2_CEX_TBL_PFL_IDX ))
		{
			p = Cnfg_GetCnfgFlash2ParmPtr( CNFG_FLASH_2_CEX_TBL_PFL_IDX );
		
  			//copy the table to our structure, creating the checksum as we go
 
  			FMT_CtlBlk.CEX_tbl[0] = p[1] << 8;
  			FMT_CtlBlk.CEX_tbl[0] |= p[2];
  		
  			FMT_CtlBlk.CEX_tbl[1] = p[4] << 8;
  			FMT_CtlBlk.CEX_tbl[1] |= p[5];
  		
  			FMT_CtlBlk.CEX_tbl[2] = p[6] << 8;
  			FMT_CtlBlk.CEX_tbl[2] |= p[8];
  		
  			FMT_CtlBlk.CEX_tbl[3] = p[9] << 8;
  			FMT_CtlBlk.CEX_tbl[3] |= p[10];  	
  			
  			FMT_CtlBlk.CnfgFlash = FALSE;
			FMT_SetFreq( freq );
		}
  		else // CEX Table in flash is not initialized
  		{
		
	  		FMT_CtlBlk.CnfgFlash = TRUE;
			freq = FMT_MIN_FREQ;
			I2C_Set_RegValue( FM_XMTR_ADDR, 0x03, (uint8)((freq)&0xFF)); //Lower Freq Byte; 97.3 Mhz
			I2C_Set_RegValue( FM_XMTR_ADDR, 0x04, (uint8)((freq>>8)&0xFF)); 	//Upper Freq Byte; 97.3 Mhz
			I2C_Set_RegValue( FM_XMTR_ADDR, 0x08, FM_XMTR_REG8_INIT); //CEX Band 3

		}
		FMT_CtlBlk.Status = FMT_STATUS_INITIALIZING;
		FMT_CtlBlk.TEB_cnt = 0;
		FMT_CtlBlk.Pup_cnt = 8;
		
		I2C_Set_RegValue( FM_XMTR_ADDR, 0x00, FM_XMTR_REG0_INIT | 0x01); // Power-on, 200mV 100% Modu
		I2C_Set_RegValue( FM_XMTR_ADDR, 0x0E, FM_XMTR_REGE_INIT); // SW reset
		I2C_Set_RegValue( FM_XMTR_ADDR, 0x06, FM_XMTR_REG6_INIT); // Set the charge pumps
	
	}
	else
	{
		FMT_CtlBlk.Attached = FALSE;	
		FMT_CtlBlk.Status = FMT_STATUS_NOT_PRESENT;
		//DelTaskFrom10MsQue( PROC_FMT_TASK );
        Core_DelTaskFromSchedQue(_10MS_QUE, PROC_FMT_TASK );
	}
}

char *FMT_index_to_ascii( uint8 freq_idx )
{
	int i,j;
	static char freq_str[6];
	
	i = freq_idx + 879;
	j = i / 10;
	i = i - (j*10);
	int8_to_ascii10( (int8)j, freq_str );
	strcat( freq_str, "." );
	freq_str[ (int)strlen(freq_str) ] = (char)(i|0x30); 
	freq_str[ (int)strlen(freq_str)+1 ] = 0;
	return( freq_str );
}

uint16 _fmt_index_to_freq( uint8 freq_idx )
{
	float 	freq;
	
	freq = floorf(((87.9 + (freq_idx * 0.1)) + 0.304)/0.008192);
	return( (uint16)freq );
}

void _SetFreq( uint16 freq, uint8 cex )
{
	I2C_Set_RegValue( FM_XMTR_ADDR, 0x03, (uint8)(freq & 0xFF));
	I2C_Set_RegValue( FM_XMTR_ADDR, 0x04, (uint8)((freq & 0xFF00) >> 8));	
	I2C_Set_RegValue( FM_XMTR_ADDR, 0x08, 0x18 | cex );
}

void FMT_SetFreq( uint8 freq )
{
	int i;
	uint16 freq16;
	
	freq16 = _fmt_index_to_freq( freq );
	FMT_CtlBlk.freq = freq16;
	
	for (i=0; i<4; i++)
	{
		if (freq16 < FMT_CtlBlk.CEX_tbl[i])
		{
			FMT_CtlBlk.CEX = 3-i;
			break;	
		}
	}
	_SetFreq( freq16, FMT_CtlBlk.CEX );
}

void FMT_PowerOn( int FMT_on )
{
	if ( FMT_on )
		I2C_Set_RegValue( FM_XMTR_ADDR, 0x00, FM_XMTR_REG0_INIT | 0x01);
	else
		I2C_Set_RegValue( FM_XMTR_ADDR, 0x00, FM_XMTR_REG0_INIT);
}

int8 FMT_ProcTask( int8 signal )
{
 int8 RtnSignal;
 int i;
 uint8 buffer[CNFG_FLASH_2_CEX_TBL_SIZE];
	
	RtnSignal = SGNL_NO_REQUEUE;
	
	switch ( FMT_CtlBlk.ProcState )
	{
		case FMT_STT_POWERUP:
			
			if (I2C_Task_Rdy() && RsrcIsLockFree( RSRC_I2C ))
			{
				RsrcSetLockOwner( RSRC_I2C, PROC_FMT_TASK );
				FMT_Register_Init();
				RsrcFreeLock( RSRC_I2C );
				FMT_CtlBlk.ProcState = FMT_STT_CEX_INIT;
			}
			break;
			
		case FMT_STT_CEX_INIT:
		
			// Wait for initial setup to kick
			if ( FMT_CtlBlk.Pup_cnt )
			{
				FMT_CtlBlk.Pup_cnt--;
				break;
			}
			
			// TtsSaySomething((TTS_ADDR_FMT_INDEX << 8) | TTS_ADDR_ON_INITIALIZING_INDEX,0,0,0,TTS_POSTDELAY_200MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
			// Wait for the initial setup to kick
			if (FMT_TEB)
			{
				if (FMT_CtlBlk.CnfgFlash)
				{
					FMT_CtlBlk.freq = FMT_MIN_FREQ;
					FMT_CtlBlk.CEX = 3;
			
					for( i=0; i<4; i++)
					{
						FMT_CtlBlk.CEX_tbl[i] = FMT_MAX_FREQ;
					}
			
					_SetFreq( (FMT_CtlBlk.freq >> 2) | 0x8000, FMT_CtlBlk.CEX );
					FMT_CtlBlk.TEB_cnt = 0;
					FMT_CtlBlk.ProcState = FMT_STT_CEX_1MHZ_STEP;
									}
				else
				{
					FMT_CtlBlk.ProcState = FMT_STT_RDY;
				}
				//TtsSaySomething((TTS_ADDR_FMT_INDEX << 8) | TTS_ADDR_PRESENT_INDEX,0,0,0,TTS_POSTDELAY_200MS);

			}
			else
			{
				FMT_CtlBlk.TEB_cnt--;
				if (FMT_CtlBlk.TEB_cnt < FMT_TEB_NEG)
					FMT_CtlBlk.ProcState = FMT_STT_ERR;
			}
			
			break;
			
		case FMT_STT_CEX_1MHZ_STEP:
			
			if (FMT_TEB)
			{
				FMT_CtlBlk.TEB_cnt++;
				if (FMT_CtlBlk.TEB_cnt > FMT_TEB_POS)
				{
					FMT_CtlBlk.freq += FMT_1MHZ_STEP;
					FMT_CtlBlk.TEB_cnt = 0;
				
					if ((FMT_CtlBlk.freq > FMT_MAX_FREQ) && (FMT_CtlBlk.CEX == 0))
					{
						FMT_CtlBlk.ProcState = FMT_STT_RDY;
						break;
					}
				}
			}
			else
			{
				FMT_CtlBlk.TEB_cnt--;
				if (FMT_CtlBlk.TEB_cnt < FMT_TEB_NEG)
				{
					FMT_CtlBlk.freq -= (FMT_1MHZ_STEP + (FMT_100KHZ_STEP*3));

					FMT_CtlBlk.ProcState = FMT_STT_CEX_100KHZ_PreSTEP;
					FMT_CtlBlk.TEB_cnt = 0;
				}
			}
			
			if (!FMT_CtlBlk.TEB_cnt)
				_SetFreq( (FMT_CtlBlk.freq >> 2) | 0x8000, FMT_CtlBlk.CEX );
				
			break;
	
		case FMT_STT_CEX_100KHZ_PreSTEP:
		
 			if (FMT_TEB)
			{
				FMT_CtlBlk.TEB_cnt++;
				//RtnSignal = signal;		//stay in run queue
				if (FMT_CtlBlk.TEB_cnt > FMT_TEB_POS)
				{
					FMT_CtlBlk.ProcState = FMT_STT_CEX_100KHZ_STEP;
					FMT_CtlBlk.TEB_cnt = 0;
				}
			}
			else
			{
				FMT_CtlBlk.TEB_cnt--;
				if (FMT_CtlBlk.TEB_cnt < FMT_TEB_NEG)
					FMT_CtlBlk.ProcState = FMT_STT_ERR;
			}
			break;
			
		case FMT_STT_CEX_100KHZ_STEP:
		
			if (FMT_TEB)
			{
				FMT_CtlBlk.TEB_cnt++;
				if (FMT_CtlBlk.TEB_cnt > FMT_TEB_POS)
				{
					FMT_CtlBlk.freq += FMT_100KHZ_STEP;
					FMT_CtlBlk.TEB_cnt = 0;
					if ((FMT_CtlBlk.freq > FMT_MAX_FREQ) && (FMT_CtlBlk.CEX == 0))
					{
						FMT_CtlBlk.ProcState = FMT_STT_RDY;
						break;
					}
				}
			}
			else
			{	
				FMT_CtlBlk.TEB_cnt--;
				if (FMT_CtlBlk.TEB_cnt < FMT_TEB_NEG)
				{
					if ( FMT_CtlBlk.CEX )
					{
						FMT_CtlBlk.CEX--;
						FMT_CtlBlk.freq -= (FMT_1MHZ_STEP*2);
						FMT_CtlBlk.TEB_cnt = 0;
					}
					else //Lost lock on CEX=0
					{
						FMT_CtlBlk.ProcState = FMT_STT_ERR;
						break;
					}
					FMT_CtlBlk.ProcState = FMT_STT_CEX_100KHZ_PostSTEP;
					FMT_CtlBlk.TEB_cnt = 0;
				}
			}
			if (!FMT_CtlBlk.TEB_cnt)
				_SetFreq( (FMT_CtlBlk.freq >> 2) | 0x8000, FMT_CtlBlk.CEX );
				
			break;
			
		case FMT_STT_CEX_100KHZ_PostSTEP:
		
			if (FMT_TEB)
			{
				FMT_CtlBlk.TEB_cnt++;
				//RtnSignal = signal;		//stay in run queue
				if (FMT_CtlBlk.TEB_cnt > FMT_TEB_POS)
				{
					FMT_CtlBlk.ProcState = FMT_STT_CEX_1MHZ_STEP;
					FMT_CtlBlk.TEB_cnt = 0;
					FMT_CtlBlk.CEX_tbl[2 - FMT_CtlBlk.CEX] = FMT_CtlBlk.freq + FMT_1MHZ_STEP;
					_SetFreq( (FMT_CtlBlk.freq >> 2) | 0x8000, FMT_CtlBlk.CEX );
				}
			}
			else
			{
				FMT_CtlBlk.TEB_cnt--;
				if (FMT_CtlBlk.TEB_cnt < FMT_TEB_NEG)
					FMT_CtlBlk.ProcState = FMT_STT_ERR;	
			}
 			break;
						
		case FMT_STT_RDY:
			
			if (Btn_GetParm(BTN_PARM_FMT_ON_IDX))
				{
					FMT_CtlBlk.Status = FMT_STATUS_ON_AIR;
					//TtsSaySomething((TTS_ADDR_FMT_INDEX << 8) | TTS_ADDR_ON_END_INDEX,0,0,0,TTS_POSTDELAY_400MS);
				}
				else
				{
					FMT_CtlBlk.Status = FMT_STATUS_OFF_AIR;
					// Power off FMT
					FMT_PowerOn( FALSE );
					//TtsSaySomething((TTS_ADDR_FMT_INDEX << 8) | TTS_ADDR_OFF_END_INDEX,0,0,0,TTS_POSTDELAY_400MS);
				}
				Btn_SetFMT_Present();
			
				Btn_SetParm( BTN_PARM_FMT_FREQ_IDX, Btn_GetParm( BTN_PARM_FMT_FREQ_IDX ), FALSE);
				
				// DelTaskFrom10MsQue( PROC_FMT_TASK );
                Core_DelTaskFromSchedQue(_10MS_QUE, PROC_FMT_TASK );
				
				if (FMT_CtlBlk.CnfgFlash)
				{
					//copy the CEX table to the buffer structure, creating the checksum as we go
  					
  					buffer[1] = FMT_CtlBlk.CEX_tbl[0] >> 8; 
  					buffer[2] = FMT_CtlBlk.CEX_tbl[0] & 0xFF;
  					
  					buffer[4] = FMT_CtlBlk.CEX_tbl[1] >> 8;
  					buffer[5] = FMT_CtlBlk.CEX_tbl[1] & 0xFF;
  					
  					buffer[6] = FMT_CtlBlk.CEX_tbl[2] >> 8;
  					buffer[8] = FMT_CtlBlk.CEX_tbl[2] & 0xFF;
  					
  					buffer[9] = FMT_CtlBlk.CEX_tbl[3] >> 8;
  					buffer[10] = FMT_CtlBlk.CEX_tbl[3] & 0xFF;
  					
  					Cnfg_WriteCnfgFlash2Parm( CNFG_FLASH_2_CEX_TBL_PFL_IDX, buffer );
				}
			
			break;
			
		case FMT_STT_CMD:
		
			if ( (FMT_CtlBlk.Status == FMT_STATUS_ON_AIR) || (FMT_CtlBlk.Status == FMT_STATUS_OFF_AIR) )
			{
				if ( RsrcIsLockFree( RSRC_I2C ) && I2C_Task_Rdy())
				{
				switch ( FMT_CtlBlk.Cmd )
				{
					case FMT_CMD_MUTE:
						I2C_Set_RegValue( FM_XMTR_ADDR, 0x00, (FM_XMTR_REG0_INIT | 0x01) | 0x04);
						break;
						
					case FMT_CMD_UNMUTE:
						I2C_Set_RegValue( FM_XMTR_ADDR, 0x00, (FM_XMTR_REG0_INIT | 0x01) & 0xFB);	
						break;
						
				}
				FMT_CtlBlk.ProcState = FMT_STT_RDY;
				// DelTaskFrom10MsQue( PROC_FMT_TASK );
                Core_DelTaskFromSchedQue(_10MS_QUE, PROC_FMT_TASK );
				}	
				
			}
			break;
			
		case FMT_STT_ERR:
		
			FMT_CtlBlk.Status = FMT_STATUS_ERROR;
			// Power off
			I2C_Set_RegValue( FM_XMTR_ADDR, 0x00, FM_XMTR_REG0_INIT);
			// DelTaskFrom10MsQue( PROC_FMT_TASK );
            Core_DelTaskFromSchedQue(_10MS_QUE, PROC_FMT_TASK );
			TtsSaySomething((TTS_ADDR_FMT_INDEX << 8) | TTS_ADDR_ERROR_INDEX,0,0,0,TTS_POSTDELAY_400MS);


			break;
		}	

	return( RtnSignal );	
}

void FMT_Mute( int8 Mute )
{
	if (Btn_GetParm(BTN_PARM_FMT_ON_IDX) && FMT_CtlBlk.Attached)
	{
		if (Mute == TRUE)
		{
			FMT_CtlBlk.Cmd = FMT_CMD_MUTE;
		}
		else
		{
			FMT_CtlBlk.Cmd = FMT_CMD_UNMUTE;
		}
		FMT_CtlBlk.ProcState = FMT_STT_CMD;
		//Put10MsSchedRtn( (PROC_FMT_TASK | 0x80), 2 );
        Core_SchedTask(_10MS_QUE, (PROC_FMT_TASK | 0x80), 2, SGNL_TIMER_MASK, 2);
	}	
}

void FMT_set_reg( int8 addr, int8 value )
{
	I2C_Set_RegValue( FM_XMTR_ADDR, addr, value);
}

void FMT_status_report()
{
	if (FMT_CtlBlk.Attached)
	{
		
		switch( FMT_CtlBlk.Status )
		{
			case FMT_STATUS_INITIALIZING:
				TtsSaySomething((TTS_ADDR_FMT_INDEX << 8) | TTS_ADDR_INITIALIZING_INDEX,0,0,0,TTS_POSTDELAY_600MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
				break;
			
			case FMT_STATUS_ON_AIR:
				TtsSaySomething((TTS_ADDR_FMT_INDEX << 8) | TTS_ADDR_ON_END_INDEX,0,0,0,TTS_POSTDELAY_600MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
				Btn_AnnounceSpfcMode( BTN_PARM_FMT_FREQ_IDX, FALSE );
				break;
				
			case FMT_STATUS_OFF_AIR:
				TtsSaySomething((TTS_ADDR_FMT_INDEX << 8) | TTS_ADDR_OFF_END_INDEX,0,0,0,TTS_POSTDELAY_600MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
				break;
			
			case FMT_STATUS_ERROR:
				TtsSaySomething((TTS_ADDR_FMT_INDEX << 8) | TTS_ADDR_ERROR_INDEX,0,0,0,TTS_POSTDELAY_600MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
				return;
				break;
		}
	}
	//else
	//	TtsSaySomething( (TTS_NO_MID_INDEX << 8) | (TTS_ADDR_FMT_INDEX) , TTS_ADDR_PRESENT_INDEX,0,0,TTS_POSTDELAY_200MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);

		
			
}

/////////////////////////////////////////////////////////////////////////
// char *FMT_GetStats( int8 Clear, char *buf  )
//
// Returns a status report for the FMT.
// Stats get cleared if Clear is non-zero.
// The report is composed of eight numbers coverted to
// ascii separated by commas.
//	Status: (OnAir, OffAir, Errored, NotPresent)
//	Frequency: (16 bit int)
//	CEX: (8 bit int)
//	Power Level: (0.5, 1.0, 2.0 mW)
//	Input Gain: (50, 100, 200 mV %100 modulation)
//
// char *Core_IntToAsciiStr( int16 i, char *str, int8 min_digs )
/////////////////////////////////////////////////////////////////////////
char *FMT_GetStats( int8 Clear, char *buf )
{
	_stat_append( (unsigned long)FMT_CtlBlk.Status, buf, TRUE,3 );
	_stat_append( (unsigned long)FMT_CtlBlk.freq, buf, TRUE,3 );
	_stat_append( (unsigned long)FMT_CtlBlk.CEX, buf, TRUE,3 );
	_stat_append( (unsigned long)FMT_CtlBlk.PwrLvl, buf, TRUE,3 );
	_stat_append( 0, buf, FALSE,3 ); 					// Input gain at 50 mV full mod

 	return( buf );
}

int8 FMT_GetStatus()
{	
	return( FMT_CtlBlk.Status );
}
