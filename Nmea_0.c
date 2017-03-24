
#define NMEA 1
#define NMEA_OR_NMEA_UTILS 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Core_api.h"
#include "rs232_api.h"
#include "tts_api.h"
#include "isd_api.h" 
#include "button_api.h"
#include "display_api.h"

#include "Nmea_api.h"

// NMEA user event & init functions defined
#include "Nmea_Cnfg.h"

// NMEA record structure and access functions defined
#include "nmea_data_defs.h"

void Nmea_InitTask()
{
	Nmea_InitMsgs();
	Nmea_InitFldRecs();
	
	nmea_ctl.last_fld_rec_idx_rptd = MAX_NMEA_RECS;
	nmea_ctl.task_state = NMEA_STATE_INITIAL;
	nmea_ctl.task_rpt_index = 0;
	nmea_ctl.mute_delta_tones = FALSE;
	_nmea_init_default_mag_variation();

	Nmea_UserInit();

	// if the watchdog is not already queued
	if (!IsTaskInSchedQue( PROC_NMEATASK ))
	{
		// schedule the watchdog to run every 1 second
		// Put1SecSchedRtn_wSignal( (PROC_NMEATASK | 0x80), 1,SGNL_NMEA_WATCHDOG_MASK); 
        Core_SchedTask(_1SEC_QUE, (PROC_NMEATASK | 0x80), 1, SGNL_NMEA_WATCHDOG_MASK, 1);
		// schedule the NMEA field rec report timer to run every 200 Msec
		// Put100MsSchedRtn_wSignal( (PROC_NMEATASK | 0x80), 2,SGNL_NMEA_FLDREC_RPT_MASK); 
        Core_SchedTask(_100MS_QUE, (PROC_NMEATASK | 0x80), 2, SGNL_NMEA_FLDREC_RPT_MASK, 2);
	}
}

void Nmea_RptRec( int16 index )
{    
	if (IS_NMEA_REC_ACTIVE( index))
	{
		Nmea_ReportFldRec( index , NMEA_REC_RPT_MASK );
	}
}

//////////////////////////////////////////////////////////////////////////////////
// int8 _nmea_get_fld_rec_rpt( int16 index )
//
// Checks conditions and parameters to see if and what kind of report should be issued
// for the NMEA msg field spec'd in index.
// returns an int8 with bits set indicating whether and what sort of report should
// be issued.
//
// Reports can have attributes:
//
//	Minumum frequency: a field value is reported at least this often and is figured
//						using .rpt_max_period and .rpt_max_tmr in nmea_records[]
//	Maximum frequency: a field value is reported at most this often and is figured
//						using .rpt_min_period and .rpt_min_tmr in nmea_records[]
//	Delta reports:	   a field value may be reported if it changes more than a specified
//						amount. The reported value is a rounded value which is rounded to
//						the interval spec'd by the amount which the value needs to change 
//						to trigger a delta report. Delta reports cannot violate the maximum
//						frequency rules.
//	Delta direction Tones:		a field value may be reported with an appended tone pair that indicates
//						a rising or failing value
//////////////////////////////////////////////////////////////////////////////////
uint8 _nmea_get_fld_rec_rpt( int16 fld_rec_idx )
{
 int16 i16,chg_dir_threshold, crossback_amt;
 uint8 result = NMEA_REC_NO_REPORT;

 	if (IS_NMEA_REC_ACTIVE( fld_rec_idx) && IS_NMEA_REC_RPT_ON( fld_rec_idx))
    {
		if ( nmea_records[ fld_rec_idx ].rpt_max_period && !nmea_records[ fld_rec_idx ].rpt_max_tmr )       
			result |= NMEA_REC_RPT_MASK;

  		if ( nmea_records[ fld_rec_idx ].cnfg_status.bits.rpt_delta_on &&
             nmea_records[ fld_rec_idx ].rpt_delta  )    // if configured to report deltas
  		{	

			if (( result & NMEA_REC_RPT_MASK ) || ( !nmea_records[ fld_rec_idx ].rpt_min_tmr ) )
    		{	
   				// get the difference from the last value
   				i16 = Core_get_modulo_difference( nmea_records[ fld_rec_idx ].value, nmea_records[ fld_rec_idx ].last_reported_value, nmea_records[ fld_rec_idx ].modulus );
                crossback_amt = nmea_records[ fld_rec_idx ].rpt_delta / 8;
                crossback_amt = MaxInt16(crossback_amt, 0x01);
                crossback_amt = MinInt16(crossback_amt, NMEA_MIN_CROSSBACK_AMT);
                
   				// if difference greater than delta threshold
   				if (( AbsInt16( i16 ) >= nmea_records[ fld_rec_idx ].rpt_delta ) ||
   					(( nmea_records[ fld_rec_idx ].last_rptd_dir == ROUND_DOWN ) && ( i16 < crossback_amt*-1 )) ||
   					(( nmea_records[ fld_rec_idx ].last_rptd_dir == ROUND_UP ) && ( i16 > crossback_amt )))
   				{
					result |= NMEA_REC_RPT_MASK | NMEA_REC_RPT_DELTA_MASK ;
  					if (( nmea_records[ fld_rec_idx ].cnfg_status.bits.rpt_abbv_on ) &&
  				    	( nmea_ctl.last_fld_rec_idx_rptd == fld_rec_idx ) &&
  				    	( nmea_records[ fld_rec_idx ].report_counter & 0x000F)) 
  					{
      					result |= NMEA_REC_RPT_ABRV_MASK ;                  // abbreviate the report
                        if (!nmea_records[ fld_rec_idx ].rpt_recency_tmr)   // But if it's been too long
                            result |= NMEA_REC_RPT_SAY_UNITS_MASK ;         // state the units
  					}

					if (i16 < 0)
						result |= NMEA_REC_RPT_DELTA_ROUND_UP_MASK;
    			}		

				if ( IS_NMEA_INTER_DELTA_TONE_ON( fld_rec_idx ))
				{
					// get the difference from the last value
   					i16 = 
					Core_get_modulo_difference( nmea_records[ fld_rec_idx ].value, nmea_records[ fld_rec_idx ].previous_value, nmea_records[ fld_rec_idx ].modulus );

					// divide the delta by 8
					chg_dir_threshold = nmea_records[ fld_rec_idx ].rpt_delta >> 3;
					if (i16 > chg_dir_threshold)
					{
						result |= NMEA_REC_RPT_DELTA_TONE_UP_MASK;
					}
					else
					if (i16 < chg_dir_threshold*-1)
					{
						result |= NMEA_REC_RPT_DELTA_TONE_DN_MASK;
					} 
					else
						result |= NMEA_REC_RPT_DELTA_TONE_EQ_MASK;
				}
			}  				
  		}

		if (!Tts_IsMsgPlaying())
		{	
			if ( nmea_records[ fld_rec_idx ].rpt_max_tmr )                  // if configured for periodic reports
  			{
				nmea_records[ fld_rec_idx ].rpt_max_tmr--;
    		}
			// Make sure we stay quiet for the minimum period
  			if ( nmea_records[ fld_rec_idx ].rpt_min_tmr )                  // if configured for periodic reports
  			{
				nmea_records[ fld_rec_idx ].rpt_min_tmr--;
    		}
			// track how recent the last report was
  			if ( nmea_records[ fld_rec_idx ].rpt_recency_tmr )              // if configured for periodic reports
  			{
				nmea_records[ fld_rec_idx ].rpt_recency_tmr--;
    		}            
		}
    }
 	return( result );
}

//////////////////////////////////////////////////////////////////////////////////
// void Nmea_ReportFldRec( int8 fld_rec_idx, int8 RptOptions )
//
// Sets up a report for the spec'd nmea rec
// RptOptions specs whether the report is standard or abbreviated meaning just
// the number is spoken
//////////////////////////////////////////////////////////////////////////////////
void Nmea_ReportFldRec( int16 fld_rec_idx, uint8 RptOptions )
{
 int8 round_dir;
 int16 temp16,i;
 uint32 PreIndex=0,PostIndex=0;
 char *cptr, *ptr, cmdstr[10];
 
	if (Btn_DialogInProgress() || Tts_IsMsgPlaying())
		return;
	
	// if it's not the short report
	// we report the number with optional header and/or footer
    if (RptOptions & NMEA_REC_RPT_MASK)
    { 
        if (!(RptOptions & NMEA_REC_RPT_ABRV_MASK)) 
        {
            PreIndex = _nmea_get_fld_rec_pre_index( fld_rec_idx );
            PostIndex = _nmea_get_fld_rec_post_index( fld_rec_idx );
        }
        else
        {
            if (RptOptions & NMEA_REC_RPT_SAY_UNITS_MASK)
            {
                PostIndex = _nmea_get_fld_rec_post_index( fld_rec_idx );
            }
        }
    }
 
	if ( RptOptions & NMEA_REC_RPT_DELTA_TONE_MASK )
  	{
		if ( RptOptions & NMEA_REC_RPT_DELTA_TONE_UP_MASK )
		{
			PostIndex = (PostIndex << 8) | NMEA_RISING_TONES;
		}
		else
		if ( RptOptions & NMEA_REC_RPT_DELTA_TONE_DN_MASK )
		{
			PostIndex = (PostIndex << 8) | NMEA_FALLING_TONES;
		}	
		else
			PostIndex = (PostIndex << 8) | NMEA_STEADY_TONES;

		// If there is no number to report
		if (!(RptOptions & NMEA_REC_RPT_MASK)  && !nmea_ctl.mute_delta_tones)
		{
	 		if (!Btn_DialogInProgress() && !Tts_IsMsgPlaying())
			{
				TtsSaySomething( PostIndex, 0, 0, 0, TTS_OPT_CLIP_MASK );
				nmea_records[ fld_rec_idx ].rpt_min_tmr = nmea_records[ fld_rec_idx ].rpt_min_period;

				if ( RptOptions & (NMEA_REC_RPT_DELTA_TONE_DN_MASK | NMEA_REC_RPT_DELTA_TONE_UP_MASK))
					nmea_records[ fld_rec_idx ].previous_value = nmea_records[ fld_rec_idx ].value;
			}
			return;
		}		
	}
  	
	if (RptOptions & NMEA_REC_RPT_MASK) 
 	{
		temp16 = nmea_records[ fld_rec_idx ].value;

		// if this is a delta report where we report rounded values
  		if (RptOptions & NMEA_REC_RPT_DELTA_MASK )
  		{
			round_dir = ROUND_DOWN; // round down or....
   			// figure the rounding direction
   			if ( RptOptions & NMEA_REC_RPT_DELTA_ROUND_UP_MASK)
   			{
     			round_dir = ROUND_UP; // round down
     		}	
   			temp16 = Core_Round( temp16, round_dir, nmea_records[ fld_rec_idx ].rpt_delta, nmea_records[ fld_rec_idx ].modulus);
   			cptr = Core_ValueToStr( temp16, cmdstr, nmea_records[ fld_rec_idx ].min_pre_decimal_digits );   			
  		}
  		else
  		{
   			cptr = Nmea_GetFldRecStringPtr(  fld_rec_idx  );
  		}

 		// produces length of integer
  		for( ptr=cptr, i=0; *ptr && (*ptr != '.'); i++)
  		{
    		ptr++; // next char in string
  		} 
  		// trims leading zeros
  		for( ; (i > nmea_records[ fld_rec_idx ].min_pre_decimal_digits) && (*cptr = '0'); i--)
  		{
    		cptr++; // trim leading zero
  		};
  
  		// Check for post-decimal character limits
  		if ((nmea_records[ fld_rec_idx ].max_post_decimal_digits) && (*ptr == '.'))
  		{
    		for (ptr++,i=0; (i<nmea_records[ fld_rec_idx ].max_post_decimal_digits) && *ptr; i++)
    		{
      			ptr++;
    		}
  		}
  		*ptr = 0; // terminate the string

        // if we succeed in saying this...
  		if (TtsSayNumber( PreIndex, cptr, PostIndex, TTS_POSTDELAY_200MS | TTS_OPT_EXPL_DIGIT_MASK))
        {
            // We're actually reporting this
            if (RptOptions & NMEA_REC_RPT_DELTA_MASK ) //if it's a rounded delta report
            {
                nmea_records[ fld_rec_idx ].last_rptd_dir = round_dir;
                nmea_records[ fld_rec_idx ].last_reported_value = temp16;
            }
            nmea_records[ fld_rec_idx ].report_counter++;
            // re-initialize the max and min report timers
            nmea_records[ fld_rec_idx ].rpt_min_tmr = nmea_records[ fld_rec_idx ].rpt_min_period;
            nmea_records[ fld_rec_idx ].rpt_max_tmr = nmea_records[ fld_rec_idx ].rpt_max_period;   // reset the timer
            nmea_records[ fld_rec_idx ].rpt_recency_tmr = NMEA_RPT_RECENCY_TMR_PERIOD;   // reset the timer
        

            nmea_ctl.last_fld_rec_idx_rptd = fld_rec_idx;	

            // get the number
            nmea_records[ fld_rec_idx ].previous_value = nmea_records[ fld_rec_idx ].value;
        }

	}
}

//////////////////////////////////////////////////////////////////////////////////
// void _nmea_proc_fld_rec_rpt( int8 index )
//
// Scans an NMEA record for any possible reports
// returns TRUE if index >= MAX_NMEA_RECS 
//////////////////////////////////////////////////////////////////////////////////
int8 _nmea_proc_fld_rec_rpt( int16 index )
{
 uint8 i;

   	// Check the NMEA records for reports
   	if ( index<MAX_NMEA_RECS )
   	{
    	i = _nmea_get_fld_rec_rpt( index );
    	if (i != NMEA_REC_NO_REPORT)
    	{
     		Nmea_ReportFldRec( index, i );
    	}
    	return( FALSE );
   	}// if ( i<MAX_NMEA_RECS )
   	else
   	{
    	return( TRUE ); // we're done
   	}
}

//////////////////////////////////////////////////////////////////////
// char *Nmea_GetMsgCopy( int UsartIdx )
//
// Returns a copy of the last msg
// at UsartIdx which is either USARTA or USARTB
//////////////////////////////////////////////////////////////////////
char Nmea_MsgCopy[MAX_RX_CHARS];
char *Nmea_GetMsgCopy( int UsartIdx )
{
	return( RS232_GetMsgCopy( UsartIdx, Nmea_MsgCopy ));
}

////////////////////////////////////////////////////////////////////////////
// Client Event Switch
////////////////////////////////////////////////////////////////////////////
int8 NmeaEvent( int8 NmeaEventId, int8 MsgSourceIdx, int8 NmeaMsgIndex )
{
 int8 EvSw_result = 1;

	switch( NmeaEventId ) 
	{
		case NMEA_EVENT_MSG_RCVD:
			NMEA_CLIENT_MSG_EVENT_ROUTINE( MsgSourceIdx,  NmeaMsgIndex); 
			break;
		case NMEA_EVENT_MSG_WD_OK_TO_ERR:
			break;
		case NMEA_EVENT_MSG_WD_ERR_TO_OK:
			break;
		default:
			break;
	}
	return( EvSw_result );
}

/////////////////////////////////////////////////////////////////
// UsartANmeaClientReady()
// Called from the RS232 A Rx ISR to check if it may start  
// collecting characters for a new command.
/////////////////////////////////////////////////////////////////
int Nmea_UsartAClientReady()
{
 	if ( GetTaskSignal( PROC_NMEATASK, SGNL_USARTA_BIT ) )
    	return 0;
  	else
    	return 1;
}
/////////////////////////////////////////////////////////////////
// UsartBNmeaClientReady()
// Called from the RS232 B Rx ISR to check if it may start  
// collecting characters for a new command.
/////////////////////////////////////////////////////////////////
int Nmea_UsartBClientReady()
{
 	if ( GetTaskSignal( PROC_NMEATASK, SGNL_USARTB_BIT ) )
    	return 0;
  	else
    	return 1;
	
}

/////////////////////////////////////////////////////////////////
// UsartUsbNmeaClientReady()
// Called from the RS232 B Rx ISR to check if it may start  
// collecting characters for a new command.
/////////////////////////////////////////////////////////////////
int Nmea_UsartUsbClientReady()
{
 	if ( GetTaskSignal( PROC_NMEATASK, SGNL_USART_USB_BIT ) )
    	return 0;
  	else
    	return 1;
	
}

//**************************************************************
// int16 _nmea_proc_msg_field( char *token_ptr, int8 rec_index, int8 msg_index )
//
// processes a field in a msg
// sets the active bit for the record
// gets a string from the
// spec'd field in the USART buffer (token_ptr)
// copies the string to the nmea record
// if it's configured as a number
// then it tries to process it as a number
// sets the active bit for the field in the msg record
// Number format is upper 3 nybble integer, lower nybble sixteenths
//**************************************************************
int16 _nmea_proc_msg_field( char *token_ptr, int16 rec_index, int8 msg_index ) {
 int16 temp16=0;
 
 	if (strlen( token_ptr ) > 0)
    	Nmea_SetFldRecCnfgStatusBit( rec_index, NMEA_STATUS_ACTIVE_IDX );
    else
    {
	    Nmea_ClrFldRecCnfgStatusBit( rec_index, NMEA_STATUS_ACTIVE_IDX );
    	return( 0 );
    }

    // Copy the ascii field to the record's string member
    Nmea_SetFldRecString ( rec_index, token_ptr );

    // CHeck if this is supposed to be a number
    if ( IS_NMEA_REC_NUMBER( rec_index ) )
    {
		temp16 = Core_StrToValue( token_ptr );

     	// Store the numerical value
     	Nmea_SetFldRecValue ( rec_index, temp16 );
    }
  	return( temp16 );
}

/////////////////////////////////////////////////////////////////
// void ProcRs232Event( int8 port_num, int8 signal )
//
// RDA2_Interrupt() will fill the byte array RS232_B_RxBuf[] with the
// NMEA msg and char *RS232_b_RxToken[] will point to the tokens in
// RS232_B_RxBuf[]. RS232_b_RxTokenCnt will have the token count.
//
// RDA1_Interrupt() will fill the byte array RS232_A_RxBuf[] with the
// NMEA msg and char *RS232_a_RxToken[] will point to the tokens in
// RS232_A_RxBuf[]. RS232_a_RxTokenCnt will have the token count.
//
/////////////////////////////////////////////////////////////////
void ProcRs232Event( int8 UsartIdx )
{
 int8 rec_idx,token,i;
 int nmea_msg_index;
 int32 expected_fields, saved_fields, msg_to_rec_map;
 char *tptr;

#define FULL_HDR_IN_BUF
#ifndef FULL_HDR_IN_BUF
  	// Get a pointer to the first token
  	tptr = RS232_GetToken( UsartIdx, 0 );

  	// First token is the valid msg index
  	nmea_msg_index = (int8) *(tptr);      
#else    
    nmea_msg_index = RS232_GetRxRemappedMsgHdrIdx( UsartIdx );
#endif

  	// Range check
#ifdef UNMATCHED_ZERO_IDX
    if (!nmea_msg_index)
#else    
  	if (nmea_msg_index > MAX_RS232_MSGS-1)
#endif        
    	return;
    
    // Check if the msg is not enabled
    if ( !IS_NMEA_MSG_ENABLED( nmea_msg_index ) )
    	return;
    
    if (!IS_NMEA_DATA_MSG( nmea_msg_index ))
    {
        // Call any app-specific functions registered in
   		// Nmea_Cnfg.h
   		NmeaEvent( NMEA_EVENT_MSG_RCVD, UsartIdx, nmea_msg_index );
        return;
    }
    	
    // If the msg is active but is sourced from the other port, return
    //if ( IS_NMEA_MSGS_ACTIVE( nmea_msg_index ) && IS_NMEA_MSGS_EXPECTED( nmea_msg_index ))
    if ( IS_NMEA_MSG_ACTIVE( nmea_msg_index ) )
    {
        i = (int)NMEA_MSG_PORT_SRC( nmea_msg_index );
	    // Port B source
        switch (i)
        {
            case USARTA:
                if (UsartIdx != USARTA)
                    return;
                break;
                
            case USARTB:
                if (UsartIdx != USARTB)
                    return;
                break;
                
            case USART_USB:
                if (UsartIdx != USART_USB)
                    return;
                break;
                
            default:
                return;
                break;
        }                   
    }

  	NMEA_MSG_INC_MSG_RCVDCNT(nmea_msg_index);   // update NMEA msg stats

  	// clear the message's active field bits
  	NMEA_SET_MSG_ACTIVEFLDS(  nmea_msg_index , 0 );

  	// get the 32 bit expexted field map for the msg
  	expected_fields = NMEA_GET_MSG_EXPDFLDS( nmea_msg_index );

 	// get the 32 bit fields to save map for the msg
  	saved_fields = NMEA_GET_MSG_FLDSTOSAVE( nmea_msg_index );
  
  	// get the 32 bit record indices for the expected fields
  	msg_to_rec_map = NMEA_GET_MSG_FLDREC_MAP( nmea_msg_index );

  	// Now check that the expected fields in the msg are non-zero length
  	for( token=0, rec_idx = 0; (expected_fields || saved_fields); token++)
  	{
	  	// Get a pointer to the next token
    	tptr = RS232_GetToken( UsartIdx, token );
   		
   		// if this field is expected
   		if (expected_fields & 0x0001)
   		{ 
	   		// Check for a zero length field
    		i = strlen( tptr );
    		if (i)
    		{ // Set the active bit for this field
      			_nmea_set_msg_active_fld_bit( nmea_msg_index, token );
    		}
   		}
   		
   		// if this is a field to save
   		if (saved_fields & 0x0001)
   		{
    		// Ok, found a bit set in the fields to save map, saved_fields
    		// Now, find the next bit set in the record map
    		// and calculate the bit index.
    		while (!(msg_to_rec_map & 0x0001) && (rec_idx < 32))
    		{
     			rec_idx++;
     			msg_to_rec_map = msg_to_rec_map >> 1;
    		}
    		
    		_nmea_proc_msg_field( tptr, rec_idx, nmea_msg_index );
    		rec_idx++;
    		msg_to_rec_map = msg_to_rec_map >> 1;
   		}
   		saved_fields = saved_fields >> 1;
   		expected_fields = expected_fields >> 1;
  	}
 
  	// if all expected fields are active
  	if ( _nmea_is_msg_valid( nmea_msg_index ) )
  	{ 
        NmeaMsgs.msg_rec[(int)nmea_msg_index].cnfg_status.bit_fields.src_port = UsartIdx;
		
   		SET_NMEA_MSG_ACTIVE( nmea_msg_index );
   		// set the watchdog to go off after 4 seconds for this msg spec'd  nmea_msg_index
   		NMEA_MSG_SET_WD( nmea_msg_index , NMEA_MSG_WD_INIT );

   		// Call any app-specific functions registered in
   		// Nmea_Cnfg.h
   		NmeaEvent( NMEA_EVENT_MSG_RCVD, UsartIdx, nmea_msg_index );
  	}
  	else
  	{
   		CLR_NMEA_MSG_ACTIVE( nmea_msg_index );
  	}
  	return;
}

/////////////////////////////////////////////////////////////////////
// Nmea_ProcTask( int8 signal )
//
// A signalled task which gets signalled from the RS232_X task with
// SGNL_USARTA, and SGNL_USARTB signals
// Also gets watchdog timer signals SGNL_WATCHDOG
// in Usart_XXX_c.h
//
// Signal description:
//   SGNL_USARTA_MASK:    Results from a successful RDA_interrupt()
//   SGNL_USARTB_MASK:    Results from a successful RDB_interrupt()
//   SGNL_NMEA_WATCHDOG_MASK:    Results from a timed watchdog for this task, 1 sec
//
//////////////////////////////////////////////////////////////////
int8 Nmea_ProcTask( int8 signal )
{
 int8 RtnSignal, WdState;
 RtnSignal = signal;
 uint8 i;

 	// Check for a new usart msg from USARTA
    if (RtnSignal & SGNL_USARTA_MASK)
    {
        // Process an NMEA Cmd
        ProcRs232Event( USARTA );
        RtnSignal = RtnSignal ^ SGNL_USARTA_MASK;
        RS232_FreeBuffer( USARTA, PROC_NMEATASK );
        return( RtnSignal );
    }
    // Check for a new usart msg from USARTB
    if (RtnSignal & SGNL_USARTB_MASK)
    {
        // Process an NMEA Cmd
        ProcRs232Event( USARTB );
        RtnSignal = RtnSignal ^ SGNL_USARTB_MASK;
        RS232_FreeBuffer( USARTB, PROC_NMEATASK );
        return( RtnSignal );
    }
    // Check for a new usart msg from USARTB
    if (RtnSignal & SGNL_USART_USB_MASK)
    {
        // Process an NMEA Cmd
        ProcRs232Event( USART_USB );
        RtnSignal = RtnSignal ^ SGNL_USART_USB_MASK;
        RS232_FreeBuffer( USART_USB, PROC_NMEATASK );
        return( RtnSignal );
    }
 	
 	////////////////////////////////////////////////////
 	// Check the NMEA field records (every 200 ms).
 	// Possible value report.
 	////////////////////////////////////////////////////
 	if (RtnSignal & SGNL_NMEA_FLDREC_RPT_MASK)
 	{
		// returns TRUE if index >= MAX_NMEA_RECS 
  		if (_nmea_proc_fld_rec_rpt( nmea_ctl.task_rpt_index ))     // Check if we need to report anything
  		{
   			RtnSignal = RtnSignal ^ SGNL_NMEA_FLDREC_RPT_MASK;		// Clear the task bit
   			nmea_ctl.task_rpt_index = 0;
  		}
  		else
  		{
   			nmea_ctl.task_rpt_index++;
   			return( RtnSignal );		// WD not done, requeue
  		}
  	}	  	
  	
  	////////////////////////////////////////////////////
 	// Process the watchdog signal (every 1 sec)
 	// every NMEA msg has a watchdog timer
 	// if it goes to zero then it's mia and start
 	// saying this particular msg is not being received
 	////////////////////////////////////////////////////
 	if (RtnSignal & SGNL_NMEA_WATCHDOG_MASK)
 	{
	 	RtnSignal = RtnSignal ^ SGNL_NMEA_WATCHDOG_MASK;		// Clear the task bit
#ifdef UNMATCHED_ZERO_IDX
        for (i=1; i<MAX_RS232_MSGS; i++)
#else	 	
   		for (i=0; i<MAX_RS232_MSGS; i++)
#endif            
   		{
    		//if (IS_NMEA_MSGS_EXPECTED(i))
            if (IS_NMEA_MSG_ENABLED(i))
    		{
     			// Check the msg watchdog
     			WdState = Nmea_ProcMsgWd( i );
     			if (WdState == NMEA_MSG_WD_OK_TO_ERR)
     			{
      				// Clear all associated active bits
      				Nmea_DeactivateMsg( i );
     			}
    		}
   		}

  	} // SGNL_NMEA_WATCHDOG_MASK

 	return( SGNL_NO_REQUEUE );
}

//////////////////////////////////////////////////////////////////
// char *NMEA_aud_cps_delta_interval_index_to_ascii( uint8 value )
//
// Takes the audio compass interval index and returns an ascii string
// of the number it maps to
//////////////////////////////////////////////////////////////////
char *NMEA_aud_cps_delta_interval_index_to_ascii( uint8 value )
{
	return( Core_ValueToStr( (int16)aud_cps_delta_interval_idx_to_uint8[(int)(value & 0x07)], aud_cps_idx_to_uint8_str, 1));
}

//////////////////////////////////////////////////////////////////
// char *NMEA_aud_cps_max_period_index_to_ascii( uint8 value )
//
// Takes the audio compass interval index and returns an ascii string
// of the number it maps to
//////////////////////////////////////////////////////////////////
char *NMEA_aud_cps_max_period_index_to_ascii( uint8 value )
{
	return( Core_IntToAsciiStr( (long)aud_cps_max_period_idx_to_uint8[(int)(value & 0x07)], aud_cps_idx_to_uint8_str, 1));
}

//////////////////////////////////////////////////////////////////
// char *NMEA_aud_cps_min_period_index_to_ascii( uint8 value )
//
// Takes the audio compass interval index and returns an ascii string
// of the number it maps to
//////////////////////////////////////////////////////////////////
char *NMEA_aud_cps_min_period_index_to_ascii( uint8 value )
{
	return( Core_IntToAsciiStr( (long)aud_cps_min_period_idx_to_uint8[(int)(value & 0x07)], aud_cps_idx_to_uint8_str, 1));
}

//////////////////////////////////////////////////////////////////
// char *NMEA_vog_delta_interval_index_to_ascii( uint8 value )
//
// Takes the VOG (speed) interval index and returns an ascii string
// of the number it maps to
//////////////////////////////////////////////////////////////////
char *NMEA_vog_delta_interval_index_to_ascii( uint8 value )
{
	return( Core_ValueToStr( (int16)vog_delta_interval_idx_to_uint8[(int)(value & 0x07)], vog_idx_to_uint8_str, 2));
}

//////////////////////////////////////////////////////////////////
// char *NMEA_vog_max_period_index_to_ascii( uint8 value )
//
// Takes the VOG (speed) interval index and returns an ascii string
// of the number it maps to
//////////////////////////////////////////////////////////////////
char *NMEA_vog_max_period_index_to_ascii( uint8 value )
{
	return( Core_IntToAsciiStr( (long)vog_max_period_idx_to_uint8[(int)(value & 0x07)], vog_idx_to_uint8_str, 1));
}

//////////////////////////////////////////////////////////////////
// char *NMEA_vog_min_period_index_to_ascii( uint8 value )
//
// Takes the VOG (speed) interval index and returns an ascii string
// of the number it maps to
//////////////////////////////////////////////////////////////////
char *NMEA_vog_min_period_index_to_ascii( uint8 value )
{
	return( Core_IntToAsciiStr( (long)vog_min_period_idx_to_uint8[(int)(value & 0x07)], vog_idx_to_uint8_str, 1));
}


void Nmea_SetAudCpsDeltaTones( uint8 value )
{
    //Configure delta tone and inter-delta tones
    nmea_records[ FLDREC_XXHDM_HDG_IDX ].cnfg_status.bits.delta_tone = value;
    nmea_records[ FLDREC_XXHDM_HDG_IDX ].cnfg_status.bits.inter_delta_tone = value;
    
    nmea_records[ FLDREC_GPRMC_CURHDG_IDX ].cnfg_status.bits.delta_tone = value;
    nmea_records[ FLDREC_GPRMC_CURHDG_IDX ].cnfg_status.bits.inter_delta_tone = value;
    
}

void Nmea_SetVogDeltaTones( uint8 value )
{
    //Configure delta tone and inter-delta tones
    nmea_records[ FLDREC_GPRMC_CURVOG_IDX ].cnfg_status.bits.delta_tone = value;
    nmea_records[ FLDREC_GPRMC_CURVOG_IDX ].cnfg_status.bits.inter_delta_tone = value;
}

void Nmea_SetVogDeltaInterval( uint8 value )
{
	_nmea_set_fld_rec_delta_interval( FLDREC_GPRMC_CURVOG_IDX, vog_delta_interval_idx_to_uint8[ (int) (value & 0x07)]	);
}

void Nmea_SetVogMaxPeriod( uint8 value )
{
	_nmea_set_fld_rec_rpt_max_period( FLDREC_GPRMC_CURVOG_IDX, vog_max_period_idx_to_uint8[ (int) (value & 0x07)]*REPORT_RATE);
}

void Nmea_SetVogMinPeriod( uint8 value )
{
	_nmea_set_fld_rec_rpt_min_period( FLDREC_GPRMC_CURVOG_IDX, vog_min_period_idx_to_uint8[ (int) (value & 0x07)]*REPORT_RATE);
}

void Nmea_SetAudCpsDeltaInterval( uint8 value )
{
	_nmea_set_fld_rec_delta_interval( FLDREC_XXHDM_HDG_IDX, aud_cps_delta_interval_idx_to_uint8[ (int) (value & 0x07)]	);
	_nmea_set_fld_rec_delta_interval( FLDREC_GPRMC_CURHDG_IDX, aud_cps_delta_interval_idx_to_uint8[ (int) (value & 0x07)]	);
}

void Nmea_SetAudCpsMaxPeriod( uint8 value )
{
	_nmea_set_fld_rec_rpt_max_period( FLDREC_XXHDM_HDG_IDX, aud_cps_max_period_idx_to_uint8[ (int) (value & 0x07)]*REPORT_RATE);
	_nmea_set_fld_rec_rpt_max_period( FLDREC_GPRMC_CURHDG_IDX, aud_cps_max_period_idx_to_uint8[ (int) (value & 0x07)]*REPORT_RATE);
}

void Nmea_SetAudCpsMinPeriod( uint8 value )
{
	_nmea_set_fld_rec_rpt_min_period( FLDREC_XXHDM_HDG_IDX, aud_cps_min_period_idx_to_uint8[ (int) (value & 0x07)]*REPORT_RATE);
	_nmea_set_fld_rec_rpt_min_period( FLDREC_GPRMC_CURHDG_IDX, aud_cps_min_period_idx_to_uint8[ (int) (value & 0x07)]*REPORT_RATE);
}

void NMEA_MuteDeltaOnly()
{
	nmea_ctl.mute_delta_tones = TRUE;
}

void NMEA_UnMuteDeltaOnly()
{
	nmea_ctl.mute_delta_tones = FALSE;
}

