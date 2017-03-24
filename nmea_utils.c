
#define NMEA_UTILS 1
#define NMEA_OR_NMEA_UTILS 1

#include "Core_api.h"
#include "rs232_api.h"
#include "nmea_api.h"
#include "nmea_data_defs.h"
#include "tts_api.h"
#include "button_api.h"
#include <stdlib.h>

void Nmea_InitFldRec( uint16 rec_index )
{
 
 	nmea_records[ rec_index ].value = 0;
	nmea_records[ rec_index ].previous_value = 0;
 	nmea_records[ rec_index ].last_reported_value = 0;
 	nmea_records[ rec_index ].cnfg_status.word16 = 0;
 	nmea_records[ rec_index ].rpt_pre_post_indices = 0;
 	nmea_records[ rec_index ].rpt_max_period = 0;
 	nmea_records[ rec_index ].rpt_max_tmr = 0;
 	nmea_records[ rec_index ].rpt_min_period = 0;
 	nmea_records[ rec_index ].rpt_min_tmr = 0;
 	nmea_records[ rec_index ].rpt_idt_period = NMEA_RPT_IDT_TMR_PERIOD;
 	nmea_records[ rec_index ].rpt_idt_tmr = 0;
 	nmea_records[ rec_index ].rpt_idt_dly_tmr = NMEA_RPT_IDT_DLY_TMR_PERIOD;
    nmea_records[ rec_index ].rpt_recency_tmr = NMEA_RPT_RECENCY_TMR_PERIOD;
 	nmea_records[ rec_index ].rpt_delta = 0;
 	nmea_records[ rec_index ].modulus = 0;
 	nmea_records[ rec_index ].min_pre_decimal_digits = 3;
 	nmea_records[ rec_index ].max_post_decimal_digits = 1;
}

void Nmea_InitFldRecs()
{
 uint16 i;
 
 	for (i=0; i<MAX_NMEA_RECS; i++ )
 	{
  		Nmea_InitFldRec( i );
 	}
}

int8 Nmea_SetFldRecString( uint16 index, char *fptr )
{
 int16 len;
 char *cptr;

  	len = strlen( fptr);
  	if (len >  (MAX_NMEA_REC_FIELD_LEN-1))
  	{
   		len = (MAX_NMEA_REC_FIELD_LEN-1);
  	}

  	cptr = nmea_records[ index ].ascii_str;
  	//copy the token
  	strncpy( cptr, fptr, len );
  	cptr[len] = 0;
  	return( len );
}

char *Nmea_GetFldRecStringPtr(int16 index)
{
 	return( nmea_records[ index ].ascii_str );
}

void Nmea_SetFldRecValue( int16 index, int16 value )
{
 	if (index < MAX_NMEA_RECS) 
 	{
   		nmea_records[ index ].value = value;
 	}
}

int16 Nmea_GetFldRecValue( int16 index )
{
 	if (index < MAX_NMEA_RECS) 
   		return( nmea_records[ index ].value );
 	else
   		return( 0xFFFF );
}

uint16 _nmea_get_fld_rec_cnfg_status( int16 index )
{
 int16 i;

	if (index < MAX_NMEA_RECS)
	{
 		i = nmea_records[ index ].cnfg_status.word16;
 		return( i );
 	}
 	else
 	{
 		Core_CodeError( NMEA_FATAL_ERROR );
 		return(0);
 	}		
}

void _nmea_set_fld_rec_cnfg_status( int16 index, uint16 cnfg_status)
{

	if (index < MAX_NMEA_RECS)
	{
 		nmea_records[ index ].cnfg_status.word16 = cnfg_status;
 		return;
 	}
 	else
 	{
 		Core_CodeError( NMEA_FATAL_ERROR );
 		return;
 	}		
}


void Nmea_SetFldRecCnfgStatusBit( int16 index, int8 q )
{
 uint16 i;

	if (q < 16)
	{
  		i = 1 << q;
  		_nmea_set_fld_rec_cnfg_status( index, _nmea_get_fld_rec_cnfg_status( index ) | i);
 	}
 	else
 	 	Core_CodeError( NMEA_FATAL_ERROR );
}

void Nmea_ClrFldRecCnfgStatusBit( int16 index, int8 q )
{
 uint16 i;

	if (q < 16)
	{
 		i = (1 << q) ^ 0xFF;
 		_nmea_set_fld_rec_cnfg_status( index, _nmea_get_fld_rec_cnfg_status( index ) & i);
 	}
 	else
 		Core_CodeError( NMEA_FATAL_ERROR );	
}

int8 Nmea_GetFldRecCnfgStatusBit( int16 index, int8 q )
{
 int16 i=0;

	if (q < 16)
	{
 		i = (1 << q);
 		return( _nmea_get_fld_rec_cnfg_status( index ) & i );
 	}
 	else
 		Core_CodeError( NMEA_FATAL_ERROR );	
 		
 	return(0);
}

void Nmea_SetFldRecCnfgStatusWord( int16 index, uint16 ival )
{
	_nmea_set_fld_rec_cnfg_status( index, ival );
}

uint8 Nmea_IsNmeaRecActive( int16 index )
{
    return( IS_NMEA_REC_ACTIVE(index));
}

void Nmea_SetFldRecRptDelta( int16 index, int16 value )
{
 	NMEA_SET_FLDREC_RPT_DELTA(index, value);
} 

void Nmea_SetFldRecRptTimer( int16 index, int16 value )
{
 	NMEA_SET_FLDREC_RPT_MAX_TIMER(index, value);
} 

uint16 Nmea_GetFldRecPreIndex( int16 idx )
{
	if (idx < MAX_NMEA_RECS)
		return( (nmea_records[ idx ].rpt_pre_post_indices >> 8) & 0xFF );
	else
	{
		Core_CodeError( NMEA_FATAL_ERROR );	
		return(0);
	} 
}

uint16 Nmea_GetFldRecPostIndex( int16 idx )
{
	if (idx < MAX_NMEA_RECS)
		return( nmea_records[ idx ].rpt_pre_post_indices & 0xFF ); 
	else
	{
		Core_CodeError( NMEA_FATAL_ERROR );	
		return(0);	
	}
}

uint8 Nmea_SetFldRecParms( int16 rec_index, int16 spch_indices, int16 max_period, int16 min_period, int16 rpt_delta, 
						   int16 min_integers, int16 max_decimals, int16 mod )
{
	if (rec_index < MAX_NMEA_RECS)
	{
 		nmea_records[ rec_index ].rpt_pre_post_indices = spch_indices;
 		nmea_records[ rec_index ].rpt_max_period = max_period;
 		nmea_records[ rec_index ].rpt_min_period = min_period;
 		nmea_records[ rec_index ].rpt_delta = rpt_delta;
 		nmea_records[ rec_index ].min_pre_decimal_digits = min_integers;
 		nmea_records[ rec_index ].max_post_decimal_digits = max_decimals;
 		nmea_records[ rec_index ].rpt_delta = rpt_delta;
 		nmea_records[ rec_index ].modulus = mod;
 		return (TRUE);
 	}
 	else
 	{	
 		Core_CodeError( NMEA_FATAL_ERROR );
 		return(FALSE);
 	}
}

int8 Nmea_IsMsgActive( uint8 i)
{
#ifdef UNMATCHED_ZERO_IDX  
    if (i)
#else    
	if (i < MAX_RS232_MSGS)
#endif        
        
 		return( NmeaMsgs.msg_rec[i].cnfg_status.bit_fields.msg_active );
 	else
 	{	
 		Core_CodeError( NMEA_FATAL_ERROR );
 		return(FALSE);
 	}	
}

void Nmea_InitMsg( int index )
{
 	NmeaMsgs.msg_rec[ index ].msg_wd_cnt = NMEA_MSG_WD_INIT;
 	NmeaMsgs.msg_rec[ index ].msg_rcvd_cnt = 0;
 	NmeaMsgs.msg_rec[ index ].cnfg_status.cnfg_status_word = 0;
 	NmeaMsgs.msg_rec[ index ].expected_fields = 0;
 	NmeaMsgs.msg_rec[ index ].expected_fields_to_save = 0;
 	NmeaMsgs.msg_rec[ index ].active_fields = 0;
 	NmeaMsgs.msg_rec[ index ].msg_to_rec_map = 0;
    NmeaMsgs.msg_rec[ index ].cnfg_status.bit_fields.is_data_msg = 1;
}

void Nmea_InitMsgs()
{
 int16 i;
 
 	NmeaMsgs.msgs_active = 0;
 	NmeaMsgs.msgs_expected = 0;
#ifdef UNMATCHED_ZERO_IDX
    for( i=1; i<MAX_RS232_MSGS; i++ )
#else    
 	for( i=0; i<MAX_RS232_MSGS; i++ )
#endif        
 	{
  		Nmea_InitMsg( i );
 	}
}

void Nmea_ReportCmd( int16 NmeaCmd, int8 PreIndex )
{
 	char cmdstr[RS232_HDR_SIZE+1];

	if ( RS232_GetMsgHdrStr( NmeaCmd, cmdstr ) )
	{
 		TtsSpellString( PreIndex, cmdstr, 0, TTS_POSTDELAY_300MS);
 	}	
}

nmea_msg *_nmea_get_msg_rec_ptr( int16 index )
{
#ifdef UNMATCHED_ZERO_IDX  
    if (index)
#else    
	if (index < MAX_RS232_MSGS)
#endif     
	{
		return( &(NmeaMsgs.msg_rec[index]) );	
	}
	else
		Core_CodeError( NMEA_FATAL_ERROR );
		
	return( (nmea_msg *)0 );
}

void Nmea_SetMsgsExpectedWord( int16 ival )
{
  	NmeaMsgs.msgs_expected = ival;
}

int8 Nmea_MsgsOk()
{
 int16 Status, Expected, temp16;

 	Status = NMEA_GET_MSGS_STATUSWORD;
 	Expected = NMEA_GET_MSGS_EXPDWORD;
 	temp16 = (Status ^ Expected) & Expected;
 	if (temp16)
   		return( FALSE );
 	else
   		return( TRUE );
}

int8 Nmea_GetMsgsStatusBit( int16 index )
{
 int8 i;
#ifdef UNMATCHED_ZERO_IDX  
    if (index)
#else    
	if (index < MAX_RS232_MSGS)
#endif 
	{
 		i = 1 << index;
 		i = NmeaMsgs.msgs_active & i;
 		return( i );
 	}
 	else
 	{
	 	Core_CodeError( NMEA_FATAL_ERROR );
	 	return(0);
	}	
}

void Nmea_ClrMsgsStatusBit( int16 index )
{
 int8 i;
#ifdef UNMATCHED_ZERO_IDX  
    if (index)
#else    
	if (index < MAX_RS232_MSGS)
#endif 
	{
 		i = (1 << index) ^ 0xFF;
 		NmeaMsgs.msgs_active &= i;
 	}
 	else
 	{
	 	Core_CodeError( NMEA_FATAL_ERROR );
	 	return;	 	
	}
}

void Nmea_SetMsgExpdFlds( int16 index, int32 ival)
{
 nmea_msg *mp;
	
	mp = _nmea_get_msg_rec_ptr( index );
 	mp->expected_fields = ival;
}

int32 Nmea_GetMsgActiveFlds( int16 index)
{
 nmea_msg *mp;
 
 	mp = _nmea_get_msg_rec_ptr( index );
 	return( mp->active_fields );
}

void _nmea_set_msg_active_fld_bit( int16 index, int8 q )
{
 int32 ii,i32;
 nmea_msg *mp;
 
 	ii = 1 << q;
 	mp = _nmea_get_msg_rec_ptr( index );
 	i32 = mp->active_fields;
 	i32 = i32 | ii;
 	mp->active_fields = i32;
}

///////////////////////////////////////////////////////////////
// int8 _nmea_is_msg_valid( int16 index )
//
// Checks a msg to see if it is receiving at least the
// expected fields
///////////////////////////////////////////////////////////////
int8 _nmea_is_msg_valid( int16 index )
{
 int32 ii, Actives, Expected;
 nmea_msg *mp;

	mp = _nmea_get_msg_rec_ptr( index );
 	Actives = mp->active_fields;
 	Expected = mp->expected_fields;
 	ii = Actives & Expected;
 	
 	if ( ii == Expected ) 
   		return( TRUE );
 	else
   		return( FALSE );
}

int8 Nmea_SetMsgFields( int16 MsgIndex, int32 ExpectedFields, int32 MsgFields, int32 RecIndexFields )
{
 nmea_msg *mp;
 
	mp = _nmea_get_msg_rec_ptr( MsgIndex );
  	mp->expected_fields = ExpectedFields;
  	mp->expected_fields_to_save = MsgFields;
  	mp->msg_to_rec_map = RecIndexFields;
  	
  	return(TRUE);
}

////////////////////////////////////////////////////////////
// void Nmea_CnfgNmeaMsgIsData( int16 MsgIndex, uint8 val )
//
// Enables the spec'd msg and all associated records
////////////////////////////////////////////////////////////
void Nmea_CnfgNmeaMsgIsData( int16 MsgIndex, uint16 val )
{
    NmeaMsgs.msg_rec[MsgIndex].cnfg_status.bit_fields.is_data_msg=val;
}

////////////////////////////////////////////////////////////
// void EnableNmeaMsg( int16 MsgIndex )
//
// Enables the spec'd msg and all associated records
////////////////////////////////////////////////////////////
void Nmea_EnableMsg( int16 MsgIndex )
{
    NmeaMsgs.msg_rec[MsgIndex].cnfg_status.bit_fields.enabled=1;
    NmeaMsgs.msg_rec[MsgIndex].cnfg_status.bit_fields.msg_active=1;
}

////////////////////////////////////////////////////////////
// void DisableNmeaMsg( int16 MsgIndex )
//
// Disables the spec'd msg and all associated records
////////////////////////////////////////////////////////////
void Nmea_DisableMsg( int16 MsgIndex )
{
 int8 i;
 int32 NmeaRecFlds;
 nmea_msg *mp;
 
 	DISABLE_NMEA_MSG( MsgIndex );
 	mp = _nmea_get_msg_rec_ptr( MsgIndex );
 	NmeaRecFlds = mp->msg_to_rec_map;
 
 	for( i = 0; i<32; i++)
 	{
  		if ( NmeaRecFlds & 0x0001 )
  		{
   			DEACTIVATE_NMEA_REC( (int) i );
  		}
  		NmeaRecFlds = NmeaRecFlds >> 1;
 	}
}

void Nmea_ActivateFldRec( int16 index )
{
    ACTIVATE_NMEA_REC( index );
}

////////////////////////////////////////////////////////////
// void Nmea_DeactivateMsg( int16 MsgIndex )
//
// Clears all active status bits associated with the msg
// typically called when a watchdog fires.
////////////////////////////////////////////////////////////
void Nmea_DeactivateMsg( int16 MsgIndex )
{
 int16 i;
 int32 NmeaRecFlds;
#ifdef UNMATCHED_ZERO_IDX  
    if (MsgIndex)
#else    
	if (MsgIndex < MAX_RS232_MSGS)
#endif  
	{
 		CLR_NMEA_MSGS_ACTIVE( MsgIndex );
 		NMEA_SET_MSG_ACTIVEFLDS( MsgIndex, 0x00000000 );
 		NmeaRecFlds = NMEA_GET_MSG_FLDREC_MAP( MsgIndex );
 	}
 	else
 	{
	 	Core_CodeError( NMEA_FATAL_ERROR );
	 	return;	 	
	}	
 
 	for( i = 0; (i<MAX_NMEA_RECS) && NmeaRecFlds; i++)
 	{
  		if ( NmeaRecFlds & 0x0001 )
  		{
   			DEACTIVATE_NMEA_REC( i );
   			NMEA_SET_FLDREC_RPT_MAX_TIMER( i, 0 );	
  		}
  		NmeaRecFlds = NmeaRecFlds >> 1;
 	}
}

void Nmea_ReportMsgStatus( int16 MsgIndex )
{
 	if (Nmea_GetMsgsStatusBit( MsgIndex ))
 	{
  		Nmea_ReportCmd( MsgIndex, TTS_ADDR_RCVING_INDEX );
 	}
 	else
 	{
  		Nmea_ReportCmd( MsgIndex, TTS_ADDR_NOTRCVING_INDEX );
 	}
}

void Nmea_ReportAllMsgStatus()
{
 int16 i;
#ifdef UNMATCHED_ZERO_IDX
    for( i=1; i<MAX_RS232_MSGS; i++)
#else
 	for( i=0; i<MAX_RS232_MSGS; i++)
#endif        
 	{
  		if ( IS_NMEA_MSG_ENABLED(i) )
   			Nmea_ReportMsgStatus( i );
 	}
}

int8 Nmea_UpdMsgWd( int16 index )
{
 int8 i;
   
 	i = NMEA_MSG_GET_WD( index );
 	if (i)
 	{
  		i--;
  		NMEA_MSG_SET_WD( index, i);
 	}
 	return( i );
}

int8 Nmea_ProcMsgWd( int MsgIndex )
{
#ifdef UNMATCHED_ZERO_IDX
    if (MsgIndex)
#else    
	if (MsgIndex < MAX_RS232_MSGS)
#endif        
	{
   		if (!(IS_NMEA_MSG_ENABLED(MsgIndex)))
   		{
			return( NMEA_MSG_WD_OK );
   		}
   		// Check the NMEA msg watchdog
   		if (Nmea_UpdMsgWd( MsgIndex ) == 0) // if it timed out
   		{
     		if (IS_NMEA_MSG_ACTIVE( MsgIndex )) // and it's active
     		{
      			CLR_NMEA_MSG_ACTIVE( MsgIndex);      // clear active status bit
      			return( NMEA_MSG_WD_OK_TO_ERR );
     		}
     		else // not active, report periodically
     		{
      			return( NMEA_MSG_WD_ERR );
     		}
    	} // if (UpdNmeaMsgWd( MsgIndex ) == 0)
    	else // watchdog hasn't timed out yet
    	{
       		return( NMEA_MSG_WD_OK );
    	}
 	}   
 	else
 	{
	 	Core_CodeError( NMEA_FATAL_ERROR );
	 	return( NMEA_MSG_WD_ERR );	 	
	}	
}
void Nmea_ResetFldRecMaxPeriod( uint8 index )
{
    NMEA_RST_FLDREC_RPT_MAX_TIMER(index);
}
void Nmea_ResetFldRecMinPeriod( uint8 index )
{
    NMEA_RST_FLDREC_RPT_MIN_TIMER(index);
}

void Nmea_EnableFldRecRpt( uint8 index )
{
    ENABLE_NMEA_REC_RPT( index );
}

void Nmea_DisableFldRecRpt( uint8 index )
{
    DISABLE_NMEA_REC_RPT( index );
    DEACTIVATE_NMEA_REC( index );
}

#ifdef IDT
const unsigned char idt_lu_tbl[] = { 	
								TTS_ADDR_NOTE_D4_INDEX,
								TTS_ADDR_NOTE_EF4_INDEX,
								TTS_ADDR_NOTE_E4_INDEX,
								TTS_ADDR_NOTE_F4_INDEX,
								TTS_ADDR_NOTE_GF4_INDEX,
								TTS_ADDR_NOTE_G4_INDEX,
								TTS_ADDR_NOTE_AF4_INDEX,
								TTS_ADDR_NOTE_A4_INDEX,
								TTS_ADDR_NOTE_BF4_INDEX,
								TTS_ADDR_NOTE_B4_INDEX,
								TTS_ADDR_NOTE_C5_INDEX,
								TTS_ADDR_NOTE_DF5_INDEX,
								TTS_ADDR_NOTE_D5_INDEX,
								TTS_ADDR_NOTE_EF5_INDEX,
								TTS_ADDR_NOTE_E5_INDEX
								 };
								
uint32 _nmea_append_inter_delta_tone( int16 fld_rec_idx, int16 delta_16 )
{
 int16 idt_unit;
 int idt_idx, idt_idx_dif, idt_idx_adj;
 uint32 idt_result;
 
 	// divide the delta by 8
 	idt_unit = nmea_records[ fld_rec_idx ].rpt_delta >> 3;
 	// figure the sub-delta index
 	idt_idx = delta_16 / idt_unit;
 	
 	if (idt_idx < -7) idt_idx = -7;
 	if (idt_idx > 7) idt_idx = 7;
 	
#ifdef IDT_SMOOTH

	idt_idx_dif = nmea_records[ fld_rec_idx ].idt_idx - idt_idx;
	
	switch (idt_idx_dif)
	{
		case 0: // we've caught up, lengthen the period
			if (nmea_records[ fld_rec_idx ].rpt_idt_dly_tmr == 0)
			{
				if (nmea_records[ fld_rec_idx ].rpt_idt_period < NMEA_RPT_IDT_TMR_PERIOD)
 				{
 					nmea_records[ fld_rec_idx ].rpt_idt_period++;
 				}
 				else
 				{
 					nmea_records[ fld_rec_idx ].rpt_idt_period = NMEA_RPT_IDT_TMR_PERIOD;
 				}
 			}
 			else
 			{
	 			nmea_records[ fld_rec_idx ].rpt_idt_dly_tmr--;
	 		}
	 								
 			idt_idx_adj = 0;
 			break;
 			
 		case  1: // We are within one.
 		case -1:
 		
 			idt_idx_adj = 1;
 			switch ( nmea_records[ fld_rec_idx ].rpt_idt_period )
 			{
	 			case NMEA_RPT_FAST_IDT_TMR_PERIOD:
	 				nmea_records[ fld_rec_idx ].rpt_idt_dly_tmr = NMEA_RPT_IDT_DLY_TMR_PERIOD;
	 				break;

	 			default:
	 				if (nmea_records[ fld_rec_idx ].rpt_idt_period < NMEA_RPT_FAST_IDT_TMR_PERIOD)
 					{
	 					if (nmea_records[ fld_rec_idx ].rpt_idt_dly_tmr == 0)
	 					{
 							nmea_records[ fld_rec_idx ].rpt_idt_period++;
 						}
 						else
 						{
	 						nmea_records[ fld_rec_idx ].rpt_idt_dly_tmr--;
	 					}
 					}
 					else
 					{
	 				//	nmea_records[ fld_rec_idx ].rpt_idt_dly_tmr = NMEA_RPT_IDT_DLY_TMR_PERIOD-1;
	 				nmea_records[ fld_rec_idx ].rpt_idt_period--;
	 				}
	 				break;
	 		}	 			
 			break;
 			 			
 		case 2: // We are within two.
 		case -2:
 		case 3: // We are within three.
 		case -3:
 		
 			nmea_records[ fld_rec_idx ].rpt_idt_dly_tmr = NMEA_RPT_IDT_DLY_TMR_PERIOD;
  			switch ( nmea_records[ fld_rec_idx ].rpt_idt_period )
 			{
	 			case NMEA_RPT_FAST_IDT_TMR_PERIOD:
	 				idt_idx_adj = 1;
	 				break;

	 			default:
	 				idt_idx_adj = 1;
	 				if (nmea_records[ fld_rec_idx ].rpt_idt_period < NMEA_RPT_FAST_IDT_TMR_PERIOD)
 					{
 						nmea_records[ fld_rec_idx ].rpt_idt_period++;
 					}
 					else
 					{
	 					//nmea_records[ fld_rec_idx ].rpt_idt_period = NMEA_RPT_FAST_IDT_TMR_PERIOD-1;
	 					nmea_records[ fld_rec_idx ].rpt_idt_period--;
	 					idt_idx_adj = 1;
	 				}
	 				break;
	 		}	 		
 			break;
 		
 		default: // We are way off, adjust agressively
 		
 			nmea_records[ fld_rec_idx ].rpt_idt_dly_tmr = NMEA_RPT_IDT_DLY_TMR_PERIOD;
 			nmea_records[ fld_rec_idx ].rpt_idt_period = NMEA_RPT_FAST_IDT_TMR_PERIOD;
 			
 		 	switch ( nmea_records[ fld_rec_idx ].rpt_idt_period )
 			{
	 			case NMEA_RPT_FAST_IDT_TMR_PERIOD:
	 				idt_idx_adj = 2;
	 				break;

	 			default:
	 				if (nmea_records[ fld_rec_idx ].rpt_idt_period > NMEA_RPT_FAST_IDT_TMR_PERIOD)
 					{
	 					idt_idx_adj = 2;
 					}
 					else
 					{
	 					idt_idx_adj = 2;
	 				}
	 				break;
	 		}	 	
	 		
 			break;
 	}	
 			
	if (idt_idx_dif > 0)
	{
		nmea_records[ fld_rec_idx ].idt_idx -= idt_idx_adj;
	}
	else
	{
		nmea_records[ fld_rec_idx ].idt_idx += idt_idx_adj;
	}
	

#else //  not IDT_SMOOTH
	nmea_records[ fld_rec_idx ].idt_idx = idt_idx;
#endif
	
	idt_idx = nmea_records[ fld_rec_idx ].idt_idx + 7;
	idt_result = idt_lu_tbl[ idt_idx ] & 0x000000FF;
	return( idt_result );
}
#else // not IDT

#ifdef USE_IDT_AS_YAW_INDICATOR
uint32 _nmea_append_yaw_indicator_tone(int16 delta_16 )
{
 uint32 yit_result;

	if (delta_16 > 0)
	{
		yit_result = NMEA_RISING_TONES;
	}
	else
	{
		yit_result = NMEA_FALLING_TONES;
	}	
	return( yit_result );
}

#else // not IDT and not USE_IDT_AS_YAW_INDICATOR
uint32 _nmea_append_inter_delta_tone( int16 fld_rec_idx, int16 delta_16 )
{
	return( TTS_ADDR_NOTE_E4_2_INDEX );
}
#endif

#endif // IDT
uint32 _nmea_append_rising_tones( uint32 idx_field )
{
	return( (idx_field << 16) | NMEA_RISING_TONES);
}

uint32 _nmea_append_falling_tones( uint32 idx_field )
{
	return((idx_field << 16) | NMEA_FALLING_TONES);
}

uint16 _nmea_get_fld_rec_pre_index( int16 fld_rec_idx )
{
	return( (nmea_records[ fld_rec_idx ].rpt_pre_post_indices >> 8) & 0xFF);
}

uint16 _nmea_get_fld_rec_post_index( int16 fld_rec_idx )
{
	return( (nmea_records[ fld_rec_idx ].rpt_pre_post_indices) & 0xFF);
}

void _nmea_set_fld_rec_post_index( int16 fld_rec_idx, uint16 post_index )
{
	nmea_records[ fld_rec_idx ].rpt_pre_post_indices &= 0xFF00;
	nmea_records[ fld_rec_idx ].rpt_pre_post_indices |= post_index;
}

void Nmea_UpdRptTrueHeading( uint8 rpt_true_heading )
{
	if ( rpt_true_heading )
	{
		_nmea_set_fld_rec_post_index( FLDREC_XXHDM_HDG_IDX, TTS_ADDR_TRUE_END_INDEX );
		_nmea_set_fld_rec_post_index( FLDREC_GPRMC_CURHDG_IDX, TTS_ADDR_TRUE_END_INDEX );	
	}
	else
	{
		_nmea_set_fld_rec_post_index( FLDREC_XXHDM_HDG_IDX, TTS_ADDR_MAGNETIC_END_INDEX );
		_nmea_set_fld_rec_post_index( FLDREC_GPRMC_CURHDG_IDX, TTS_ADDR_MAGNETIC_END_INDEX );
	}
}

void _nmea_set_fld_rec_delta_interval( int16 idx, int16 val )
{
    int8 round_dir;
    int16 temp16=0;
    
	if (idx < MAX_NMEA_RECS)
	{
 		if (val)
 		{
            if (nmea_records[ idx ].rpt_delta == 0)
            {
                //nmea_records[ idx ].cnfg_status.word16 |= NMEA_CNFG_INTER_DELTA_TONE_MASK;
                nmea_records[ idx ].cnfg_status.bits.rpt_delta_on = 1;
                // Set nmea_records[ idx ].last_reported_value to be value minus 2xdelta
                nmea_records[ idx ].last_reported_value = nmea_records[ idx ].value - (nmea_records[ idx ].rpt_delta*2);
                nmea_records[ idx ].last_rptd_dir = ROUND_UP;
            }
	 	}
	 	else
	 	{
		 	//nmea_records[ idx ].cnfg_status.word16 &= NMEA_CNFG_INTER_DELTA_TONE_MASK ^ 0xFFFF;
            nmea_records[ idx ].cnfg_status.bits.rpt_delta_on = 0;
		}
        nmea_records[ idx ].rpt_delta = val;
        // adjust nmea_records[ idx ].last_rptd_dir, and nmea_records[ idx ].last_reported_value to reflect
        // the new delta interval
        round_dir = nmea_records[ idx ].last_rptd_dir; // round down or....
        temp16 = nmea_records[ idx ].last_reported_value;
   		temp16 = Core_Round( temp16, round_dir, nmea_records[ idx ].rpt_delta, nmea_records[ idx ].modulus);

   		//cptr = Core_ValueToStr( temp16, cmdstr, nmea_records[ fld_rec_idx ].min_pre_decimal_digits );   
		nmea_records[ idx ].last_rptd_dir = round_dir;		
		nmea_records[ idx ].last_reported_value = temp16;	
 	}
 	else
 	{	
 		Core_CodeError( NMEA_FATAL_ERROR );
 	}
}

void _nmea_set_fld_rec_rpt_max_period( int16 idx, int16 val )
{
	if (idx < MAX_NMEA_RECS)
	{
 		nmea_records[ idx ].rpt_max_period = val;
        nmea_records[ idx ].rpt_max_tmr = val;
 	}
 	else
 	{	
 		Core_CodeError( NMEA_FATAL_ERROR );
 	}
}

void _nmea_set_fld_rec_rpt_min_period( int16 idx, int16 val )
{
	if (idx < MAX_NMEA_RECS)
	{
 		nmea_records[ idx ].rpt_min_period = val;
        nmea_records[ idx ].rpt_min_tmr = val;
 	}
 	else
 	{	
 		Core_CodeError( NMEA_FATAL_ERROR );
 	}
}

void Nmea_SetFldRecRptCounter( int16 idx, int16 val )
{
	if (idx < MAX_NMEA_RECS)
	{
 		nmea_records[ idx ].report_counter = val;
 	}
 	else
 	{	
 		Core_CodeError( NMEA_FATAL_ERROR );
 	}
}

////////////////////////////////////////////////////////////////////
//void _nmea_init_default_mag_variation()
//
// Reads the flash memory block which contains the default magnetic
// variation (DMV) and it's checksum. If it's valid, then it will be used
// whenever the data source is the fluxgate compass. If it's invalid
// then the menu item in system options will be enabled to allow for
// a one-time configuration. An invalid flash value will set the DMW
// to zero.
///////////////////////////////////////////////////////////////////
void _nmea_init_default_mag_variation()
{
 uint8 *p;
 int16 dmv;
	
	if (Cnfg_IsCnfgFlash2ParmValid( CNFG_FLASH_2_DEFAULT_MAG_VAR_PFL_IDX ))
	{
		p = Cnfg_GetCnfgFlash2ParmPtr( CNFG_FLASH_2_DEFAULT_MAG_VAR_PFL_IDX );
		dmv = p[1] << 8;
  		dmv |= p[2];
  		nmea_ctl.default_mag_variation = dmv;
	}
	else
	{
		nmea_ctl.default_mag_variation = INITIAL_DMV;
	}
}

void Nmea_InitDefaultMagneticVariation()
{
    _nmea_init_default_mag_variation();
}

int16 Nmea_GetDefaultMagneticVariation()
{
	return( nmea_ctl.default_mag_variation );	
}

/////////////////////////////////////////////////////////////////////////////
// Converts a latitude/longitude in ascii to double long float
/////////////////////////////////////////////////////////////////////////////
double long Nmea_GetLatLonFromMsg( int MsgSourceIdx, int fld_idx)
{
 char *cptr, *dptr;
 double long result, dl_fract;
 char sense; 
 
	// Get a pointer to the GPRMC Lat Value token
    cptr = RS232_GetToken( MsgSourceIdx, fld_idx );
	sense = *cptr;

	// Get a pointer to the GPRMC Lat Value token
    cptr = RS232_GetToken( MsgSourceIdx, fld_idx+1 );
     		
    // check for decimal point, returns 0 if not there
    dptr = strchr( cptr, '.' );

    // if it's potentially a float
    if (dptr)
    {
      	dl_fract = atof( dptr-2 ) / 60;  // Convert the minutes portion to degree fraction
      	dptr[-2] = 0;					// null terminate the integer degree part
    }
    else
    	dl_fract = 0;					// no degree fraction
    
    result = atof(cptr) + dl_fract;		// construct the latitude
    // Get a pointer to the GPRMC Lat Value direction token
    if ((sense == 'W') || (sense == 'S'))
    	result = result * -1;
    	
    return(result);
    
}

#ifdef TEST
#endif
