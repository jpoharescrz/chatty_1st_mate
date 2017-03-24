/////////////////////////////////////////////////////////////////////////
////                          Button_Cnfg.c                            ////
////   
#define BUTTON
#include <xc.h>

#include <p24FJ128GB204.h>
#include "Core_api.h"
#include "tts_api.h"
#include "display_api.h"
#include "fmt_api.h"
#ifdef ONBOARD_GPS_MODULE
#include "gps_api.h"
#endif
#include "nmea_api.h"
#include "button_api.h"


////////////////////////////////////////////////////////////////////////
// void Btn_InitTask()
//
// Initializes the button process
// - douse the panel leds
// - load the button configuartion from EEPROM
// - Call the Tone process init function
// - Schedule the button process to run every 100 ms
//
////////////////////////////////////////////////////////////////////////
void Btn_InitTask()
{
 	_cnfg_load_settings_from_flash();
 	_cnfg_LoadImageFromCnfgFlash2();
 	Btn_Ctl.remote_btn_state = 0;
 	Btn_Ctl.speech_muted = 0;
	Btn_Ctl.min_max_delta_cnfg_state = MIN_MAX_DELTA_COMPASS;
	Btn_Ctl.keystate = 0;
 	//Btn_Ctl.dialog_cnt = BTN_DIALOG_CNT_INIT;
    Btn_Ctl.dialog_cnt = 300;

 	// Schedule button task to run every 10 ms
 	//Put10MsSchedRtn( (PROC_BUTTONTASK | 0x80), 1 );
    Core_SchedTask(_10MS_QUE, (PROC_BUTTONTASK | 0x80), 3, SGNL_TIMER_MASK, 3);
}

int8 Btn_IsSpchMuted()
{
	return( Btn_Ctl.speech_muted );	
}

/////////////////////////////////////////////////////////////////////////
// void Btn_SetFMT_Present( )
//
// Called from fmt.c to tell button that FMT is present and can appear in
// the menu
/////////////////////////////////////////////////////////////////////////
void Btn_SetFMT_Present( )
{
	menu_items[ FMT_MOD_SUBMENU_ITEM_IDX ].type = MENU_ITEM_TYPE_SUBMENU;
}

/////////////////////////////////////////////////////////////////////////
// void Btn_SetGPS_Present( )
//
// Called from gps.c to tell button that GPS is present and can appear in
// the menu
/////////////////////////////////////////////////////////////////////////
#ifdef ONBOARD_GPS_MODULE
void Btn_SetGPS_Present( )
{
	menu_items[ GPS_MOD_SUBMENU_ITEM_IDX ].type = MENU_ITEM_TYPE_SUBMENU;
	menu_items[ GPS_ON_PARM_ITEM_IDX ].type = MENU_ITEM_TYPE_PARM;
}
#endif

/////////////////////////////////////////////////////////////////////////
// uint16 Btn_DialogInProgress()
//
// Called to check if user dialogs are happening 
// Used to silence spontaneous or periodic reports during user dialogs
//
// Tts calls it to hold off the low priority input queue if true
/////////////////////////////////////////////////////////////////////////
uint16 Btn_DialogInProgress()
{
	return( Btn_Ctl.dialog_cnt );	
}

////////////////////////////////////////////////////////////////
// uint8 Btn_SetParm(uint16 index, uint8 ival, uint8 save)
//
// Sets the parameter spec'd in index to the value spec'd
// in ival. Optionally saves it to flash using save arg.
// Returns TRUE if successful and FALSE if index is out
// of range or ival is out of range
////////////////////////////////////////////////////////////////
uint8 Btn_SetParm(uint16 index, uint8 ival, uint8 save)
{
 uint16 unpacked_idx, max_value, event_idx;

 	if (index < MAX_PARM_CHG_PROFILES)
 	{
  		// Get the parameter index for the unpacked array
  		unpacked_idx = _btn_parm_chg_profiles[ index ][UNPACKED_ADDR_IDX];

  		// Get the max index for the parameter
  		max_value = _btn_parm_chg_profiles[ index ][MAX_VALUE_IDX];

  		// Get the Event index
  		event_idx = _btn_parm_chg_profiles[ index ][EVENT_IDX];

  		if (ival <= max_value)
  		{
   			_cnfg_unpacked_uint8[ unpacked_idx ] = ival;
    
   			_btn_event_switch( event_idx );
   			if (save)
     			Cnfg_SaveSettings( FALSE );
     			
   			return( TRUE );
  		}
 	}
 	return( FALSE ); 
}

////////////////////////////////////////////////////////////////
// uint8 Btn_GetParm( uint16 index )
//
// Gets the parameter spec'd in index. 
// Returns the parm value if successful and 0xFF if index is out
// of range.
////////////////////////////////////////////////////////////////
uint8 Btn_GetParm( int index)
{
 uint8 unpacked_idx, value;

 	if (index < MAX_PARM_CHG_PROFILES)
 	{
  		// Get the parameter index for the unpacked array
  		unpacked_idx = _btn_parm_chg_profiles[ index ][UNPACKED_ADDR_IDX];
   		//change the parm, and wrap if necessary
  		value = _cnfg_unpacked_uint8[ unpacked_idx ];
 	}
 	else
  		value = 0xFF;
  
 	return( value );
}

signed int16 Btn_GetHdgCalAdj()
{
 signed int16 result;

	result = ((signed int16)Btn_GetParm( BTN_PARM_HDG_CAL_ADJ_IDX ) - HCA_ZERO_VALUE) * HCA_SCALE;
	return( result );
}

///////////////////////////////////////////////////////////////////
// void Btn_TargetOff()
//
// Shuts off targeting mode and calls any user defined routines
///////////////////////////////////////////////////////////////////
void Btn_TargetOff()
{
    _btn_event_switch( EVNT_TGT_OFF );
}

////////////////////////////////////////////////////////////////////////
// void _cnfg_pack_settings()
//
// copy the button state configuration from the unpacked structure, _cnfg_unpacked
// to the packed structure, _cnfg_packed
//
////////////////////////////////////////////////////////////////////////
// Pack the button states for writing to flash
void _cnfg_pack_settings()
{
 	_cnfg_packed.next_page_offset = _cnfg_unpacked.next_page_offset;
 	//_cnfg_packed.cnfg_menu = _cnfg_unpacked.cnfg_menu;
 	_cnfg_packed.vog_delta_interval = _cnfg_unpacked.vog_delta_interval;
	_cnfg_packed.vog_min_period = _cnfg_unpacked.vog_min_period;
    _cnfg_packed.vog_max_period = _cnfg_unpacked.vog_max_period;
 	_cnfg_packed.nontarget_bar_disp_mode = _cnfg_unpacked.nontarget_bar_disp_mode;
 	_cnfg_packed.target_bar_disp_mode = _cnfg_unpacked.target_bar_disp_mode;
 	_cnfg_packed.speech_volume = _cnfg_unpacked.speech_volume;
 	_cnfg_packed.tone_volume = _cnfg_unpacked.tone_volume;
 	_cnfg_packed.source = _cnfg_unpacked.source;
 	_cnfg_packed.res_mode = _cnfg_unpacked.res_mode;
 	_cnfg_packed.buzzer_volume = _cnfg_unpacked.buzzer_volume;
 	_cnfg_packed.lvl_dgt_dsply = _cnfg_unpacked.lvl_dgt_dsply;
 	_cnfg_packed.lvl_bar_dsply = _cnfg_unpacked.lvl_bar_dsply;
 	_cnfg_packed.fmt_freq = _cnfg_unpacked.fmt_freq;
 	_cnfg_packed.fmt_on = _cnfg_unpacked.fmt_on;
  	_cnfg_packed.gps_on = _cnfg_unpacked.gps_on;
  	_cnfg_packed.gps_nav_on = _cnfg_unpacked.gps_nav_on;
  	_cnfg_packed.xng_loop_a = _cnfg_unpacked.xng_loop_a;	
  	_cnfg_packed.xng_loop_b = _cnfg_unpacked.xng_loop_b;
    _cnfg_packed.xng_loop_usb = _cnfg_unpacked.xng_loop_usb;
  	_cnfg_packed.xng_a2b = _cnfg_unpacked.xng_a2b;	
  	_cnfg_packed.xng_b2a = _cnfg_unpacked.xng_b2a;
    _cnfg_packed.xng_a2u = _cnfg_unpacked.xng_a2u;	
  	_cnfg_packed.xng_b2u = _cnfg_unpacked.xng_b2u;  
    _cnfg_packed.xng_u2a = _cnfg_unpacked.xng_u2a;	
  	_cnfg_packed.xng_u2b = _cnfg_unpacked.xng_u2b;  
  	_cnfg_packed.aud_cps = _cnfg_unpacked.aud_cps;
  	_cnfg_packed.aud_cps_delta = _cnfg_unpacked.aud_cps_delta;
  	_cnfg_packed.aud_cps_delta_interval = _cnfg_unpacked.aud_cps_delta_interval;
  	_cnfg_packed.aud_cps_rpt_max_period = _cnfg_unpacked.aud_cps_rpt_max_period;
  	_cnfg_packed.aud_cps_rpt_min_period = _cnfg_unpacked.aud_cps_rpt_min_period;
  	_cnfg_packed.aud_cps_rpt_true_heading = _cnfg_unpacked.aud_cps_rpt_true_heading;
  	_cnfg_packed.usart_a_speed = _cnfg_unpacked.usart_a_speed;
  	_cnfg_packed.usart_b_speed = _cnfg_unpacked.usart_b_speed;
  	_cnfg_packed.per_rpt_vog = _cnfg_unpacked.per_rpt_vog;	
  	_cnfg_packed.tgt_cnfg = _cnfg_unpacked.tgt_cnfg;
  	_cnfg_packed.nav_to_wpt = _cnfg_unpacked.nav_to_wpt;
	_cnfg_packed.heading_cal_adj = _cnfg_unpacked.heading_cal_adj;
}

////////////////////////////////////////////////////////////////////////
// void _cnfg_unpack_settings()
//
// copy the button state configuration from the packed structure, _cnfg_packed
// to the unpacked structure, _cnfg_unpacked
//
////////////////////////////////////////////////////////////////////////
void _cnfg_unpack_settings()
{
 	_cnfg_unpacked.next_page_offset = _cnfg_packed.next_page_offset;
 	//_cnfg_unpacked.cnfg_menu = _cnfg_packed.cnfg_menu;
 	_cnfg_unpacked.vog_delta_interval = _cnfg_packed.vog_delta_interval;
	_cnfg_unpacked.vog_min_period = _cnfg_packed.vog_min_period;
    _cnfg_unpacked.vog_max_period = _cnfg_packed.vog_max_period;
 	Btn_Ctl.target_mode = 0; // Targeting mode is off on PUP.
 	_cnfg_unpacked.nontarget_bar_disp_mode = _cnfg_packed.nontarget_bar_disp_mode;
 	_cnfg_unpacked.target_bar_disp_mode = _cnfg_packed.target_bar_disp_mode;
 	_cnfg_unpacked.speech_volume = _cnfg_packed.speech_volume;
 	_cnfg_unpacked.tone_volume = _cnfg_packed.tone_volume;
 	_cnfg_unpacked.source = _cnfg_packed.source;
 	_cnfg_unpacked.res_mode = _cnfg_packed.res_mode;
 	_cnfg_unpacked.buzzer_volume = _cnfg_packed.buzzer_volume;
 	_cnfg_unpacked.lvl_dgt_dsply = _cnfg_packed.lvl_dgt_dsply;
 	_cnfg_unpacked.lvl_bar_dsply = _cnfg_packed.lvl_bar_dsply;
  	_cnfg_unpacked.fmt_freq = _cnfg_packed.fmt_freq;
  	_cnfg_unpacked.fmt_on = _cnfg_packed.fmt_on;
  	_cnfg_unpacked.gps_on = _cnfg_packed.gps_on;
  	_cnfg_unpacked.gps_nav_on = _cnfg_packed.gps_nav_on;
  	_cnfg_unpacked.xng_loop_a = _cnfg_packed.xng_loop_a;	
  	_cnfg_unpacked.xng_loop_b = _cnfg_packed.xng_loop_b;
  	_cnfg_unpacked.xng_a2b = _cnfg_packed.xng_a2b;	
  	_cnfg_unpacked.xng_b2a = _cnfg_packed.xng_b2a;
    _cnfg_unpacked.xng_a2u = _cnfg_packed.xng_a2u;	
  	_cnfg_unpacked.xng_b2u = _cnfg_packed.xng_b2u;    
  	_cnfg_unpacked.aud_cps = _cnfg_packed.aud_cps;
  	_cnfg_unpacked.aud_cps_delta = _cnfg_packed.aud_cps_delta;
  	_cnfg_unpacked.aud_cps = _cnfg_packed.aud_cps;
  	_cnfg_unpacked.aud_cps_delta = _cnfg_packed.aud_cps_delta;
  	_cnfg_unpacked.aud_cps_delta_interval = _cnfg_packed.aud_cps_delta_interval;
  	_cnfg_unpacked.aud_cps_rpt_max_period = _cnfg_packed.aud_cps_rpt_max_period;
  	_cnfg_unpacked.aud_cps_rpt_min_period = _cnfg_packed.aud_cps_rpt_min_period;
  	_cnfg_unpacked.aud_cps_rpt_true_heading = _cnfg_packed.aud_cps_rpt_true_heading;
  	_cnfg_unpacked.usart_a_speed = _cnfg_packed.usart_a_speed;
  	_cnfg_unpacked.usart_b_speed = _cnfg_packed.usart_b_speed;
  	_cnfg_unpacked.per_rpt_vog = _cnfg_packed.per_rpt_vog;	
  	_cnfg_unpacked.tgt_cnfg = _cnfg_packed.tgt_cnfg;
  	_cnfg_unpacked.nav_to_wpt = _cnfg_packed.nav_to_wpt;
	_cnfg_unpacked.heading_cal_adj = _cnfg_packed.heading_cal_adj;
    _cnfg_unpacked.min_max_delta_mode = 0;
}

////////////////////////////////////////////////////////////////////////
// void _cnfg_load_settings_from_flash()
//
// Load the button state configuration from the FLASH
//  - Scan the cnfg block in flash to see if there is a valid cnfg
//  - Load it if it's there (2 instructions = 8 bytes)
//  - Otherwise erase the cnfg block and initiaze the first one
// Unpack the cnfg to the unpacked structure
// 
// Each cnfg page in flash has the following structure
// byte 0: cnfg page #(1-254)
// byte 1: cnfg byte 0, part of packed structure
// byte 2: cnfg byte 1, part of packed structure
// byte 3: null_0, ignored in flash, read as 0
// byte 4: cnfg byte 2, part of packed structure
// byte 5: cnfg byte 3, part of packed structure
// byte 6: cnfg byte 4, part of packed structure
// byte 7: null_1, ignored in flash, read as 0
// byte 8: cnfg byte 5, part of packed structure
// byte 9: cnfg byte 6, part of packed structure
// byte 10: cnfg byte 7, part of packed structure
// byte 11: null_2, ignored in flash, read as 0
//
// See the typedef Btn_stt_str2 above. This is the packed cnfg structure
// Also see typedef Btn_stt_str above. This is the unpacked cnfg structure
////////////////////////////////////////////////////////////////////////
void _cnfg_load_settings_from_flash()
{
 uint8 next_page_offset;
 unsigned int16 i;
 uint8 buffer[CNFG_STR_SIZE+4];
 unsigned int32 bptr;
 int16 temp;

 	// Search the flash cnfg block for the valid data
 	// Erased flash will read as 0xFF
 	buffer[0] = 0x00;
 	// CNFG_FLASH_PAGE_SIZE is the number of words (16 bit)
 	// CNFG_STR_SIZE is the size of the packed cnfg structure in bytes
 	for (i=(CNFG_STR_SIZE/2); ((i<CNFG_FLASH_PAGE_SIZE) && (buffer[0] != 0xFF)); i += (CNFG_STR_SIZE/2))
 	{
  		bptr = (CNFG_FLASH_ADDR+i);		// PCD compiler needs simple stuff to work
  		bptr &= CNFG_FLASH_ADDR_MASK;

  		// reads 2 + 1 = 3 instructions, we want to look at the current page and the start
  		// of the next one
  		// dumps CNFG_STR_SIZE+4 bytes into buffer[]
  		read_program_memory( bptr, buffer, CNFG_STR_SIZE+4 );
  		// Since there are 4 flash words per cnfg page, next_page_offset is figured below
  		next_page_offset = ( ((i-(CNFG_STR_SIZE/2)) / (CNFG_STR_SIZE/2)) + 1);

  		// if this block is valid and the next blank
  		if (( buffer[0] == next_page_offset ) && (buffer[CNFG_STR_SIZE] == 0xFF))
  		{
   			// copy the configuration to the packed structure
   			for( i = 0; i < CNFG_STR_SIZE; i++ )
     			_cnfg_pack_uint8[ i ] = buffer[i];

   			_cnfg_unpack_settings();
   			_cnfg_unpacked.next_page_offset++;
   			break;
  		}
 	}
 
 	// if no valid data in flash
 	if (( buffer[0] == 0xFF ) || ( i == CNFG_FLASH_PAGE_SIZE ))
 	{
  		// init the default button operating mode states
  		_cnfg_unpacked.next_page_offset = 1;
  		//_cnfg_unpacked.cnfg_menu = 1;
		temp = INITIAL_DMV;
		//temp = ((14 << 4)+1)*-1;
  	 	_cnfg_unpacked.vog_delta_interval = 0;      // No VOG Delta
		_cnfg_unpacked.vog_min_period = 0;
        _cnfg_unpacked.vog_max_period = 0;          // No VOG Report
  		_cnfg_unpacked.res_mode = 0;				// low resolution
  		_cnfg_unpacked.buzzer_volume = 0;
  		_cnfg_unpacked.source = SRC_HDG_NONE;		//automatically find a source
  		_cnfg_unpacked.lvl_dgt_dsply = 3;
  		_cnfg_unpacked.lvl_bar_dsply = 3;
  		_cnfg_unpacked.speech_volume = 0;
  		_cnfg_unpacked.tone_volume = 2;
  		_cnfg_unpacked.nontarget_bar_disp_mode = 1;
  		_cnfg_unpacked.target_bar_disp_mode = 1;
  		_cnfg_unpacked.null0 = 0x55;
  		_cnfg_unpacked.null0 = 0xAA;
  		_cnfg_unpacked.fmt_freq = 973-879;			// 97.3 MHz
  		_cnfg_unpacked.fmt_on = 0;					// FM Transmitter off
  		_cnfg_unpacked.gps_on = 0;					// GPS Module off
  		_cnfg_unpacked.gps_nav_on = 0;				// GPS Module Nav off
  		_cnfg_unpacked.xng_loop_a = 1;				// Loop Port a
  		_cnfg_unpacked.xng_loop_b = 0;				// Loop Port b
  		_cnfg_unpacked.xng_a2b = 0;					// don't switch a to b 
  		_cnfg_unpacked.xng_b2a = 0;					// don't switch b to a 
        _cnfg_unpacked.xng_a2u = 0;					// don't switch a to usb 
  		_cnfg_unpacked.xng_b2u = 0;					// don't switch b to usb        
  		_cnfg_unpacked.aud_cps = 1;					// periodic heading report on 
  		_cnfg_unpacked.aud_cps_delta = 1;			// periodic heading report delta on 
  		_cnfg_unpacked.aud_cps_delta_interval = 3;	// periodic heading report delta inteval (5 degrees) 
  		_cnfg_unpacked.aud_cps_rpt_max_period = 4;	// periodic heading report max period (30 seconds) 
		_cnfg_unpacked.aud_cps_rpt_min_period = 1;		// periodic heading report min period (5 seconds)
		_cnfg_unpacked.aud_cps_rpt_true_heading = 0;	// True Heading report switch
		_cnfg_unpacked.usart_a_speed = 0;			// Usart A speed cnfg
		_cnfg_unpacked.usart_b_speed = 0;			// Usart B Speed cnfg
  		_cnfg_unpacked.per_rpt_vog = 1;				//  
  		_cnfg_unpacked.tgt_cnfg = 1;				// target heading 
		_cnfg_unpacked.heading_cal_adj = 127;			// heading cal tweak
        _cnfg_unpacked.min_max_delta_mode = 0;      // Configure compass with min/max period and delta interval

  		Cnfg_SaveSettings( TRUE ); // Save and force a re-init of cnfg flash
  		//Core_SetProcToneOff();
 	}

 	// Override saved target mode
 	Btn_Ctl.target_mode = 0;
 	
 	Btn_Ctl.menu_ptr = menus[ (int)MAIN_MENU_IDX ];
	_btn_new_menu_item( Btn_Ctl.menu_ptr->index );
 	
 	//TtsQueSpeech2( TTS_CMD_SET_SPEECH_VOLUME, _cnfg_unpacked.speech_volume + 1, TTS_OPT_HIGH_PRIORITY_QUEUE_MASK );
}

#define FLASH_CNFG_WORDS_BUFFER_SIZE 8
///////////////////////////////////////////////////////////////////
// void _cnfg_EraseCnfgFlash2()
//
// Reads the flash configuration words at the end of the block
// then erases the block
// then restores the flash configuration words
///////////////////////////////////////////////////////////////////
void _cnfg_EraseCnfgFlash2()
{	
	//_cnfg_LoadImageFromCnfgFlash2();
	erase_flash_memory_block( (uint32)CNFG_FLASH_2_ADDR );
	_cnfg_WriteImageToCnfgFlash2();
}

///////////////////////////////////////////////////////////////////
// void _cnfg_EraseCnfgFlash()
//
// Reads the flash configuration words at the end of the block
// then erases the block
// then restores the flash configuration words
///////////////////////////////////////////////////////////////////
void _cnfg_EraseCnfgFlash()
{	
	erase_flash_memory_block( (uint32)CNFG_FLASH_ADDR );
}

uint8 *Cnfg_GetCnfgFlash2ParmPtr( uint8 parm_idx )
{
 uint8 *p = 0;
 	
 	if (parm_idx < CNFG_FLASH_2_TBL_SIZE)
 	{
	 	p = _cnfg_flash_2_parm_chg_profiles[ parm_idx ].image_ptr;
	}
	else
		Core_CodeError( BTN_FATAL_ERROR );
		
	return( p );
}

void _cnfg_LoadImageFromCnfgFlash2()
{
 int i;
 
	for ( i=0; i<CNFG_FLASH_2_TBL_SIZE; i++)
	{
		read_program_memory( 	_cnfg_flash_2_parm_chg_profiles[ i ].cnfg_flash_addr, 
								_cnfg_flash_2_parm_chg_profiles[ i ].image_ptr,
								_cnfg_flash_2_parm_chg_profiles[ i ].cnfg_flash_size);
	}
}

void _cnfg_WriteImageToCnfgFlash2()
{
 int i;
 
	for ( i=0; i<CNFG_FLASH_2_TBL_SIZE; i++)
	{
		write_flash_buffer( _cnfg_flash_2_parm_chg_profiles[ i ].cnfg_flash_addr, 
							_cnfg_flash_2_parm_chg_profiles[ i ].image_ptr,
							_cnfg_flash_2_parm_chg_profiles[ i ].cnfg_flash_size);
	}
}

uint8 Cnfg_GetCnfgFlash2ParmChecksum( uint8 parm_idx )
{
 uint8 *p;
 uint8 parm_size, checksum=0;
 int i;
 
 	if (parm_idx < CNFG_FLASH_2_TBL_SIZE)
 	{
 		p = _cnfg_flash_2_parm_chg_profiles[ parm_idx ].image_ptr;
 		parm_size = _cnfg_flash_2_parm_chg_profiles[ parm_idx ].cnfg_flash_size;
 		
 		for( i = 1; i < parm_size; i++)
 		{
	 		if ((i&0x3) != 0x03)
	 			checksum ^= p[i];

	 	}
	 	checksum &= 0x7F;
 	}
 	else
 		Core_CodeError( BTN_FATAL_ERROR );
 		
 	return( checksum);
} 


uint8 Cnfg_IsCnfgFlash2ParmValid( uint8 parm_idx )
{
 uint8 *p;
 uint8 checksum;
 
 	checksum = Cnfg_GetCnfgFlash2ParmChecksum( parm_idx );
 	p = Cnfg_GetCnfgFlash2ParmPtr( parm_idx );
 		
 	if (p[0] == checksum)
	{
		return(TRUE);
	}
	else
	{
		return(FALSE);	
	}
}

void Cnfg_WriteCnfgFlash2Parm( uint8 parm_idx, uint8 *pp )
{
 uint8 *p;
 uint8 checksum, parm_size;	
 int i;
 	
 	_cnfg_EraseCnfgFlash2();
 	
    if (pp)
    {
        p = Cnfg_GetCnfgFlash2ParmPtr( parm_idx );
        parm_size = _cnfg_flash_2_parm_chg_profiles[ parm_idx ].cnfg_flash_size;
 		
        for( i = 1; i < parm_size; i++)
        {
            if ((i&0x3) != 0x03)
                p[i] = pp[i];
        }
    }
	
	checksum = Cnfg_GetCnfgFlash2ParmChecksum( parm_idx );
	p[0] = checksum;	
	
	
	_cnfg_WriteImageToCnfgFlash2();
}

void Cnfg_SaveCnfgFlash2Settings()
{
    Cnfg_WriteCnfgFlash2Parm(0,0);
}
	
#define CNFG_FLASH_MAX_PAGES ((CNFG_FLASH_PAGE_SIZE/(CNFG_STR_SIZE))*2)-2
////////////////////////////////////////////////////////////////////////
// void Cnfg_SaveSettings()
//
// Save for the button states configuration 
// structure to FLASH
//
////////////////////////////////////////////////////////////////////////
void Cnfg_SaveSettings(int8 clear_cnfg_flash)
{
 uint8 next_page_offset;
 uint32 flash_addr;

 	// if at the end of flash config block
 	if (( _cnfg_unpacked.next_page_offset >= CNFG_FLASH_MAX_PAGES ) || clear_cnfg_flash )
 	{
	 	_cnfg_EraseCnfgFlash();
  		_cnfg_unpacked.next_page_offset = 1;
 	}
 	_cnfg_pack_settings();
 	next_page_offset = ((_cnfg_unpacked.next_page_offset-1) * (CNFG_STR_SIZE/2));
 	flash_addr = (CNFG_STR_SIZE/2) + (CNFG_FLASH_ADDR + next_page_offset);
 	flash_addr &= CNFG_FLASH_ADDR_MASK;
 	write_flash_buffer( flash_addr, _cnfg_pack_uint8, CNFG_STR_SIZE );
 	_cnfg_unpacked.next_page_offset++;
}

////////////////////////////////////////////////////////////////////////
// void Btn_report_hw_cnfg_status()
//
// Reports the Product, version #, and state of the FMT, GPS, and Data source
//
////////////////////////////////////////////////////////////////////////
void Btn_report_hw_cnfg_status()
{
	TTS_SayVersion();
#ifdef ONBOARD_GPS_MODULE
	GPS_status_report();
#endif
	FMT_status_report();
	//Adsc_report_status();
}


////////////////////////////////////////////////////////////////////////
// void _btn_say_level(uint16 pre_msgs, uint16 post_msgs, uint8 state, uint8 profile, uint8 priority, uint8 end_profile)
//
// Used by _btn_announce_item_value() to announce the values of parameters
////////////////////////////////////////////////////////////////////////
void _btn_say_level(uint16 pre_msgs, uint16 post_msgs, uint8 state, uint8 profile, uint8 priority, uint8 end_profile)
{
 uint16 state_msg, options = TTS_POSTDELAY_200MS;
 	
 	if ((profile < MAX_PROFILES) && (state < MAX_LEVELS))
 	{
  		state_msg = (uint16)BtnLevelSpchProfiles[ profile ][ state ] + end_profile;
 	}
 	else
  		state_msg = TTS_ADDR_ERROR_INDEX;
  	
  	if (priority)
  		options |= TTS_OPT_HIGH_PRIORITY_QUEUE_MASK;
 	
 	TtsSaySomething(pre_msgs, state_msg, post_msgs, 0, options);
}

////////////////////////////////////////////////////////////////////////
// _btn_announce_item_value( int ItemIndex, int8 value )
//
// Announces the specific configuration.
//
////////////////////////////////////////////////////////////////////////
void _btn_announce_item_value(int8 ItemIndex, uint8 value, uint8 keypressed, int8 force  )
{		
 char *p, buf[4];
 uint16 	parm_item_post_value_spch_index;
 uint16 	menu_item_post_value_spch_index;
 uint8		end_profile;
 
#ifdef USE_NAV_KEYS 
 	if ((keypressed == BTN_INPUTS_NAV_SEL_BIT) || (keypressed == BTN_INPUTS_NAV_EXIT_BIT))
 	{
	 	parm_item_post_value_spch_index = TTS_ADDR_SELECTED_INDEX;
	 	menu_item_post_value_spch_index = TTS_ADDR_MENU_INDEX;
	 	end_profile = 0;
	}
	else
#endif        
	{
        // No menus. So everything is a parameter
	 	parm_item_post_value_spch_index = 0;
	 	menu_item_post_value_spch_index = 0;
	 	end_profile = 1;
	}
	
 	switch( ItemIndex )
   	{
    	default:
 
 		case MAIN_MENU_ITEM_IDX:
 			TtsSaySomething( TTS_ADDR_MAIN_MENU_INDEX,0,0,0,TTS_POSTDELAY_400MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
			break;
			
    	case HDG_SRC_PARM_ITEM_IDX:
      		_btn_say_level(  (TTS_ADDR_HDG_MID_INDEX << 8) | TTS_ADDR_SOURCE_INDEX, parm_item_post_value_spch_index, value, COMPASS_GPS_AUTO_PROFILE, force, end_profile);
      		break;
      		
    	case RESOLUTION_PARM_ITEM_IDX:
        	_btn_say_level( TTS_ADDR_RESOLUTION_INDEX, parm_item_post_value_spch_index, value, 2, force, end_profile);
      		break;
      		
      	case VOLUME_PARM_ITEM_IDX:
        	_btn_say_level( TTS_ADDR_VOLUME_INDEX, parm_item_post_value_spch_index, value, 2, force, end_profile);
      		break;
      		
		case SYSTEM_SUBMENU_ITEM_IDX:
			TtsSaySomething( TTS_ADDR_SYSTEM_OPTS_MID_INDEX + end_profile,menu_item_post_value_spch_index,0,0,TTS_POSTDELAY_200MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
			break;
			
		case PORT_OPTIONS_ITEM_IDX:
			TtsSaySomething( ((TTS_ADDR_PORT_INDEX << 8) | TTS_ADDR_OPTIONS_INDEX),menu_item_post_value_spch_index,0,0,TTS_POSTDELAY_200MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
			break;
			
		case FMT_MOD_SUBMENU_ITEM_IDX:
			TtsSaySomething( TTS_ADDR_FMT_OPTS_MID_INDEX  + end_profile, menu_item_post_value_spch_index,0,0,TTS_POSTDELAY_200MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
			break;
#ifdef ONBOARD_GPS_MODULE
		case GPS_MOD_SUBMENU_ITEM_IDX:
			TtsSaySomething( TTS_ADDR_GPS_OPTS_MID_INDEX  + end_profile, menu_item_post_value_spch_index,0,0,TTS_POSTDELAY_200MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
			break;
			
		case GPS_ON_PARM_ITEM_IDX:
			_btn_say_level( TTS_ADDR_GPS_MOD_INDEX, parm_item_post_value_spch_index, value, OFF_ON_PROFILE, force, end_profile);
			break;
#endif	
		case FMT_ON_PARM_ITEM_IDX:
			_btn_say_level( TTS_ADDR_FMT_INDEX, parm_item_post_value_spch_index, value, OFF_ON_PROFILE, force, end_profile);
			break;
			
		case FMT_FREQ_PARM_ITEM_IDX:
			// FM transmitter parameter is the difference between the frequency and 87.9 in 
			// 0.1s.
			p = FMT_index_to_ascii( value );
#ifdef USE_NAV_KEYS            
			if (( keypressed == BTN_INPUTS_NAV_UP_BIT) || ( keypressed == BTN_INPUTS_NAV_DN_BIT) )
				TtsSayNumber( 0, p, 0, TTS_OPT_HIGH_PRIORITY_QUEUE_MASK|TTS_OPT_CLIP_MASK);
			else
#endif                
				TtsSayNumber( (TTS_ADDR_FMT_INDEX << 16) | (TTS_ADDR_FREQ_INDEX << 8) | parm_item_post_value_spch_index, p, 0, TTS_POSTDELAY_300MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
			break;
			
		case XNG_LOOPA_PARM_ITEM_IDX:
#ifdef USE_NAV_KEYS            
			if (( keypressed == BTN_INPUTS_NAV_UP_BIT) || ( keypressed == BTN_INPUTS_NAV_DN_BIT) )
				_btn_say_level( (TTS_ADDR_LOOP_INDEX << 8) | TTS_ADDR_A_INDEX, parm_item_post_value_spch_index, value, OFF_ON_PROFILE, force, end_profile);

			else
#endif                
				_btn_say_level( (TTS_ADDR_LOOP_INDEX << 8) | TTS_ADDR_A_INDEX, parm_item_post_value_spch_index, value, OFF_ON_PROFILE, force, end_profile);
			break;
			
		case XNG_LOOPB_PARM_ITEM_IDX:
#ifdef USE_NAV_KEYS            
			if (( keypressed == BTN_INPUTS_NAV_UP_BIT) || ( keypressed == BTN_INPUTS_NAV_DN_BIT) )
				_btn_say_level( (TTS_ADDR_LOOP_INDEX << 8) | TTS_ADDR_B_INDEX, parm_item_post_value_spch_index, value, OFF_ON_PROFILE, force, end_profile);

			else
#endif                
				_btn_say_level( (TTS_ADDR_LOOP_INDEX << 8) | TTS_ADDR_B_INDEX, parm_item_post_value_spch_index, value, OFF_ON_PROFILE, force, end_profile);
			break;
			
		case XNG_A2B_PARM_ITEM_IDX:
#ifdef USE_NAV_KEYS            
			if (( keypressed == BTN_INPUTS_NAV_UP_BIT) || ( keypressed == BTN_INPUTS_NAV_DN_BIT) )
				_btn_say_level( (TTS_ADDR_SWITCH_INDEX << 8) | TTS_ADDR_A2B_MID_INDEX, parm_item_post_value_spch_index, value, OFF_ON_PROFILE, force, end_profile);
			else
#endif                
				_btn_say_level( (TTS_ADDR_SWITCH_INDEX << 8) | TTS_ADDR_A2B_MID_INDEX, parm_item_post_value_spch_index, value, OFF_ON_PROFILE, force, end_profile);
			break;

		case XNG_B2A_PARM_ITEM_IDX:
#ifdef USE_NAV_KEYS            
			if (( keypressed == BTN_INPUTS_NAV_UP_BIT) || ( keypressed == BTN_INPUTS_NAV_DN_BIT) )
				_btn_say_level( (TTS_ADDR_SWITCH_INDEX << 8) | TTS_ADDR_B2A_MID_INDEX, parm_item_post_value_spch_index, value, OFF_ON_PROFILE, force, end_profile);
			else
#endif                
				_btn_say_level( (TTS_ADDR_SWITCH_INDEX << 8) | TTS_ADDR_B2A_MID_INDEX, parm_item_post_value_spch_index, value, OFF_ON_PROFILE, force, end_profile);
			break;
			
		case USART_A_SPEED_PARM_ITEM_IDX:
#ifdef USE_NAV_KEYS		
			if (( keypressed == BTN_INPUTS_NAV_UP_BIT) || ( keypressed == BTN_INPUTS_NAV_DN_BIT) )
				_btn_say_level(0, 0, value, USART_SPEED_PROFILE, force, end_profile);
			else
#endif                
				_btn_say_level( (TTS_ADDR_PORTA_INDEX << 8) | TTS_ADDR_SPEED_MID_INDEX, parm_item_post_value_spch_index, value, USART_SPEED_PROFILE, force, end_profile);
			break;

		case USART_B_SPEED_PARM_ITEM_IDX:
#ifdef USE_NAV_KEYS		
			if (( keypressed == BTN_INPUTS_NAV_UP_BIT) || ( keypressed == BTN_INPUTS_NAV_DN_BIT) )
				_btn_say_level( 0, 0, value, USART_SPEED_PROFILE, force, end_profile);
			else
#endif                
				_btn_say_level( (TTS_ADDR_PORTB_INDEX << 8) | TTS_ADDR_SPEED_MID_INDEX, parm_item_post_value_spch_index, value, USART_SPEED_PROFILE, force, end_profile);
			break;
			

		case CNFG_STATUS_RPT_PARM_ITEM_IDX:
		
			TtsSaySomething( TTS_ADDR_SYSTEM_RPT_INDEX, 0,0,0,TTS_POSTDELAY_300MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
#ifdef USE_NAV_KEYS            
			if (keypressed == BTN_INPUTS_NAV_SEL_BIT)
			{
				Btn_report_hw_cnfg_status();
			}
#endif            
			break;
			
		case PER_RPT_SUBMENU_ITEM_IDX:
			TtsSaySomething( TTS_ADDR_PER_RPT_MID_INDEX + end_profile, menu_item_post_value_spch_index,0,0,TTS_POSTDELAY_200MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
			break;
		
        case CNFG_MIN_MAX_DELTA_MODE_ITEM_IDX:

            if (value)
                TtsSaySomething( (TTS_ADDR_SPEED_MID_INDEX<<8)|TTS_ADDR_CONFIG_INDEX, 0,0,0,TTS_POSTDELAY_200MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
            else
                TtsSaySomething( (TTS_ADDR_COMPASS_MID_INDEX<<8)|TTS_ADDR_CONFIG_INDEX, 0,0,0,TTS_POSTDELAY_200MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);

            break;
            
		case AUD_CPS_SUBMENU_ITEM_IDX:
			TtsSaySomething( TTS_ADDR_AUD_CPS_OPTS_MID_INDEX + end_profile, menu_item_post_value_spch_index,0,0,TTS_POSTDELAY_200MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
			break;
		
		case AUD_CPS_ON_PARM_ITEM_IDX:
#ifdef USE_NAV_KEYS            
			if (( keypressed == BTN_INPUTS_NAV_UP_BIT) || ( keypressed == BTN_INPUTS_NAV_DN_BIT) )
				_btn_say_level( 0, 0, value, OFF_ON_PROFILE, force, end_profile);
			else
#endif                
				_btn_say_level( TTS_ADDR_AUDIO_COMPASS_MID_INDEX, parm_item_post_value_spch_index, value, OFF_ON_PROFILE, force, end_profile);
			break;

					break;
			
		case AUD_CPS_DELTA_TONES_PARM_ITEM_IDX:
#ifdef USE_NAV_KEYS            
			if (( keypressed == BTN_INPUTS_NAV_UP_BIT) || ( keypressed == BTN_INPUTS_NAV_DN_BIT) )
				_btn_say_level( 0, 0, value, OFF_ON_PROFILE, force, end_profile);
			else
#endif                
				_btn_say_level( TTS_ADDR_DELTA_SIGNAL_MID_INDEX, parm_item_post_value_spch_index, value, OFF_ON_PROFILE, force, end_profile);
			break;
			
		case AUD_CPS_RPT_TRUE_HEADING_PARM_ITEM_IDX:
#ifdef USE_NAV_KEYS            
			if (( keypressed == BTN_INPUTS_NAV_UP_BIT) || ( keypressed == BTN_INPUTS_NAV_DN_BIT) )
				_btn_say_level( 0, 0, value, TRUE_MAGNETIC_PROFILE, force, end_profile);
			else
#endif                
				_btn_say_level( (TTS_ADDR_REPORT_INDEX << 8) | TTS_ADDR_HDG_MID_INDEX, parm_item_post_value_spch_index, value, TRUE_MAGNETIC_PROFILE, force, end_profile);
			break;			
			
		case AUD_CPS_DELTA_INTERVAL_PARM_ITEM_IDX:
            if (value)
            {
                p = NMEA_aud_cps_delta_interval_index_to_ascii( value );
#ifdef USE_NAV_KEYS           
			if (( keypressed == BTN_INPUTS_NAV_UP_BIT) || ( keypressed == BTN_INPUTS_NAV_DN_BIT) )
				TtsSayNumber( 0, p, TTS_ADDR_DEGREES_END_INDEX, TTS_OPT_HIGH_PRIORITY_QUEUE_MASK|TTS_OPT_CLIP_MASK);
			else
#endif               
                TtsSayNumber( (TTS_ADDR_COMPASS_MID_INDEX << 8) | TTS_ADDR_DELTA_INTERVAL_MID_INDEX, p, ((TTS_ADDR_DEGREES_MID_INDEX + end_profile) << 8) | parm_item_post_value_spch_index, TTS_POSTDELAY_300MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
            }
            else
                TtsSaySomething( (TTS_ADDR_COMPASS_MID_INDEX << 8) | TTS_ADDR_DELTA_INTERVAL_MID_INDEX, TTS_ADDR_OFF_END_INDEX, 0,0, TTS_POSTDELAY_300MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);

			break;
			
		case AUD_CPS_MAX_PERIOD_PARM_ITEM_IDX:
            if (value)
            {
                p = NMEA_aud_cps_max_period_index_to_ascii( value );
#ifdef USE_NAV_KEYS
			if (( keypressed == BTN_INPUTS_NAV_UP_BIT) || ( keypressed == BTN_INPUTS_NAV_DN_BIT) )
				TtsSayNumber( 0, p, TTS_ADDR_SECONDS_END_INDEX, TTS_OPT_HIGH_PRIORITY_QUEUE_MASK|TTS_OPT_CLIP_MASK);
			else
#endif
				TtsSayNumber( (TTS_ADDR_COMPASS_MID_INDEX << 8) | TTS_ADDR_MAX_PERIOD_MID_INDEX, p, ((TTS_ADDR_SECONDS_MID_INDEX + end_profile) << 8) | parm_item_post_value_spch_index, TTS_POSTDELAY_300MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
            }
            else
            {
                TtsSaySomething( (TTS_ADDR_COMPASS_MID_INDEX << 8) | TTS_ADDR_MAX_PERIOD_MID_INDEX, TTS_ADDR_OFF_END_INDEX, 0,0, TTS_POSTDELAY_300MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
            }
			break;
		
		case AUD_CPS_MIN_PERIOD_PARM_ITEM_IDX:
			p = NMEA_aud_cps_min_period_index_to_ascii( value );
#ifdef USE_NAV_KEYS
			if (( keypressed == BTN_INPUTS_NAV_UP_BIT) || ( keypressed == BTN_INPUTS_NAV_DN_BIT) )
				TtsSayNumber( 0, p, TTS_ADDR_SECONDS_END_INDEX, TTS_OPT_HIGH_PRIORITY_QUEUE_MASK|TTS_OPT_CLIP_MASK);
			else
#endif
				TtsSayNumber( (TTS_ADDR_COMPASS_MID_INDEX << 8) | TTS_ADDR_MIN_PERIOD_MID_INDEX, p, ((TTS_ADDR_SECONDS_MID_INDEX+ end_profile) << 8) | parm_item_post_value_spch_index, TTS_POSTDELAY_300MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
			break;
			
		case VOG_ON_PARM_ITEM_IDX:
			_btn_say_level( ((TTS_ADDR_REPORT_INDEX << 8) | TTS_ADDR_VOG_MID_INDEX), parm_item_post_value_spch_index, value, OFF_ON_PROFILE, force, end_profile);
			break;

		case VOG_DELTA_INTERVAL_PARM_ITEM_IDX:
            if (value)
            {
                p = NMEA_vog_delta_interval_index_to_ascii( value );
#ifdef USE_NAV_KEYS           
			if (( keypressed == BTN_INPUTS_NAV_UP_BIT) || ( keypressed == BTN_INPUTS_NAV_DN_BIT) )
				TtsSayNumber( 0, p, TTS_ADDR_DEGREES_END_INDEX, TTS_OPT_HIGH_PRIORITY_QUEUE_MASK|TTS_OPT_CLIP_MASK);
			else
#endif               
                TtsSayNumber( (TTS_ADDR_SPEED_MID_INDEX << 8) | TTS_ADDR_DELTA_INTERVAL_MID_INDEX, p, ((TTS_ADDR_KNOTS_END_INDEX) << 8) | parm_item_post_value_spch_index, TTS_POSTDELAY_300MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
            }
            else
                TtsSaySomething( (TTS_ADDR_SPEED_MID_INDEX << 8) | TTS_ADDR_DELTA_INTERVAL_MID_INDEX, TTS_ADDR_OFF_END_INDEX, 0,0, TTS_POSTDELAY_300MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);

			break;
			
		case VOG_MAX_PERIOD_PARM_ITEM_IDX:
            if (value)
            {
                p = NMEA_vog_max_period_index_to_ascii( value );
#ifdef USE_NAV_KEYS
			if (( keypressed == BTN_INPUTS_NAV_UP_BIT) || ( keypressed == BTN_INPUTS_NAV_DN_BIT) )
				TtsSayNumber( 0, p, TTS_ADDR_SECONDS_END_INDEX, TTS_OPT_HIGH_PRIORITY_QUEUE_MASK|TTS_OPT_CLIP_MASK);
			else
#endif
				TtsSayNumber( (TTS_ADDR_SPEED_MID_INDEX << 8) | TTS_ADDR_MAX_PERIOD_MID_INDEX, p, ((TTS_ADDR_SECONDS_MID_INDEX + end_profile) << 8) | parm_item_post_value_spch_index, TTS_POSTDELAY_300MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
            }
            else
            {
                TtsSaySomething( (TTS_ADDR_SPEED_MID_INDEX << 8) | TTS_ADDR_MAX_PERIOD_MID_INDEX, TTS_ADDR_OFF_END_INDEX, 0,0, TTS_POSTDELAY_300MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
            }
			break;
		
		case VOG_MIN_PERIOD_PARM_ITEM_IDX:
			p = NMEA_vog_min_period_index_to_ascii( value );
#ifdef USE_NAV_KEYS
			if (( keypressed == BTN_INPUTS_NAV_UP_BIT) || ( keypressed == BTN_INPUTS_NAV_DN_BIT) )
				TtsSayNumber( 0, p, TTS_ADDR_SECONDS_END_INDEX, TTS_OPT_HIGH_PRIORITY_QUEUE_MASK|TTS_OPT_CLIP_MASK);
			else
#endif
				TtsSayNumber( (TTS_ADDR_SPEED_MID_INDEX << 8) | TTS_ADDR_MIN_PERIOD_MID_INDEX, p, ((TTS_ADDR_SECONDS_MID_INDEX+ end_profile) << 8) | parm_item_post_value_spch_index, TTS_POSTDELAY_300MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
			break;
			
			
		case TGT_CNFG_PARM_ITEM_IDX:
			_btn_say_level( ((TTS_ADDR_TARGET_INDEX << 8) | TTS_ADDR_MODE_INDEX), parm_item_post_value_spch_index, value, TGT_CNFG_PROFILE, force, end_profile);
			break;
			
		case NAV_TO_WPT_PARM_ITEM_IDX:
		
			// values 1-7 indicate using internal waypoints 0-6
			if (value)
			{
				buf[0] = (value-1) + 0x30; buf[1] = 0;
				TtsSayNumber( (TTS_ADDR_USE_MID_INDEX << 8) | (TTS_ADDR_WPT_MID_INDEX) , buf, 0, TTS_OPT_HIGH_PRIORITY_QUEUE_MASK|TTS_OPT_CLIP_MASK);	
			}
			else // use the external waypoint info
			{
				TtsSaySomething( (TTS_ADDR_USE_MID_INDEX << 8) | (TTS_ADDR_EXT_MID_INDEX) ,TTS_ADDR_WPT_MID_INDEX+end_profile, 0,0,TTS_POSTDELAY_200MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
			}
			break;

		case HDG_CAL_ADJ_PARM_ITEM_IDX:
			// Heading calibration adjustment
			// 0.1s.
			p = Adsc_Hca_index_to_ascii( (((signed int16) value) - HCA_ZERO_VALUE) * HCA_SCALE );
#ifdef USE_NAV_KEYS
			if (( keypressed == BTN_INPUTS_NAV_UP_BIT) || ( keypressed == BTN_INPUTS_NAV_DN_BIT) )
				TtsSayNumber( 0, p, 0, TTS_OPT_HIGH_PRIORITY_QUEUE_MASK|TTS_OPT_CLIP_MASK);
			else
#endif
            if (Btn_Ctl.keysPressedCnt)
            {
                TtsSayNumber( 0, p, 0, TTS_OPT_HIGH_PRIORITY_QUEUE_MASK|TTS_OPT_CLIP_MASK);
            }
            else
            {
				TtsSayNumber( (TTS_ADDR_HDG_MID_INDEX << 16) | (TTS_ADDR_ADJUST_MID_INDEX << 8) | parm_item_post_value_spch_index, p, 0, TTS_POSTDELAY_300MS|TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
            }    
			break;

   	}
}

////////////////////////////////////////////////////////////////////////
// Btn_AnnounceSpfcMode( int CnfgMode )
//
// Announces the specific configuration.
//
//
////////////////////////////////////////////////////////////////////////
void Btn_AnnounceSpfcMode(int8 ItemIndex, int8 force )
{
 int8 value;
 
 	if (ItemIndex <= MAX_MENU_ITEM_IDX)
 		value = _cnfg_unpacked_uint8[ (int)_btn_parm_chg_profiles[(int)ItemIndex][UNPACKED_ADDR_IDX] ];
 	else
 		value = 0;
 		
   	_btn_announce_item_value( _btn_parm_chg_profiles[(int)ItemIndex][MENU_ITEM_IDX], value, 0xff, force  );
}


////////////////////////////////////////////////////////////////////////
// void Btn_SetRemoteButtonState( int8 Button )
//
// Called from testing message handler to set the button state 
// for testing purposes
///////////////////////////////////////////////////////////////////////
void Btn_SetRemoteButtonState( uint16 Button )
{
 	Btn_Ctl.remote_btn_state = Button | 0x8000;
}

int8 _btn_event_switch( int8 event )
{
 int8 EvSw_result = 1;

   switch( event ) {
    case EVNT_TGT_UP: USER_TGTUP(); break;
    case EVNT_TGT_DN: USER_TGTDN(); break;
    
    case EVNT_TGT_ON: 
        if (USER_TGTON())
        {
    		Btn_Ctl.target_mode = 1;
    		DISP_TGT_LED = 1;
    	}
    	else
    	{
	    	Btn_Ctl.target_mode = 0;
	    	DISP_TGT_LED = 0;
	    }
    	break;
    
    case EVNT_TGT_OFF: 
    	USER_TGTOFF(); 
    	// turn off targeting
    	Btn_Ctl.target_mode = 0;
    	DISP_TGT_LED = 0;
    	Disp_ClearBarDisplay();
    	
    	break;
       
    case EVNT_MUTE_CHG:
    
        if ( !Btn_Ctl.speech_muted )
    	{
	    	//i = Btn_GetParm( BTN_PARM_SPEECH_VOL_IDX );
			Tts_UnMute();
	    	//TtsQueSpeech2( TTS_CMD_SET_SPEECH_VOLUME, i + 1, TTS_OPT_HIGH_PRIORITY_QUEUE_MASK );
	    	TtsSaySomething(TTS_ADDR_MUTE_INDEX, TTS_ADDR_OFF_END_INDEX, 0, 0, TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
    	}
    	else
   		{ 	
	   		// speech is being muted so we say 'muted' before doing so
			Btn_Ctl.speech_muted = 0; //unmute temporarily to say we are 'muted'
    		TtsSaySomething(TTS_ADDR_MUTE_INDEX, TTS_ADDR_ON_END_INDEX, 0, 0, TTS_OPT_HIGH_PRIORITY_QUEUE_MASK);
			Tts_Mute(0);
    		//TtsQueSpeech2( TTS_CMD_SET_SPEECH_VOLUME, 0, TTS_OPT_HIGH_PRIORITY_QUEUE_MASK );
			Btn_Ctl.speech_muted = 1; //we are 'muted'
    	} 
        break;
        
    case EVNT_RES_CHG: 
    	USER_RESCHG(); 
    	break;
        
    case EVNT_HEADING_RPT:
        USER_STATUS_RPT();
        break;
    	
    case EVNT_SRC_CHG: 
		USER_SRCCHG();
		// turn off targeting
        _btn_event_switch( EVNT_TGT_OFF );	         
        
		break;
    		
	case EVNT_PER_HDG_RPT_CHG:
	
		USER_AUD_CPS_CHG( );
		break;
		
	case EVNT_AUD_CPS_DELTA_TONES_CHG:
		USER_AUD_CPS_DELTA_CHG( Btn_GetParm(BTN_PARM_AUD_CPS_DELTA_TONES_IDX) );
		break;
		
	case EVNT_PER_RPT_VOG_CHG:
		USER_PER_RPT_VOG_CHG( );
		break;
        
	case EVNT_VOG_DELTA_INTERVAL_CHG:
		USER_VOG_DELTA_INTERVAL_CHG( Btn_GetParm(BTN_PARM_VOG_DELTA_INTERVAL_IDX) );
		break;
		
	case EVNT_VOG_MAX_PERIOD_CHG:
		USER_VOG_MAX_PERIOD_CHG( Btn_GetParm(BTN_PARM_VOG_MAX_PERIOD_IDX) );
		break;
		
	case EVNT_VOG_MIN_PERIOD_CHG:
		USER_VOG_MIN_PERIOD_CHG( Btn_GetParm(BTN_PARM_VOG_MIN_PERIOD_IDX) );
		break;        
		
	case EVNT_AUD_CPS_DELTA_INTERVAL_CHG:
		USER_AUD_CPS_DELTA_INTERVAL_CHG( Btn_GetParm(BTN_PARM_AUD_CPS_DELTA_INTERVAL_IDX) );
		break;
		
	case EVNT_AUD_CPS_MAX_PERIOD_CHG:
		USER_AUD_CPS_MAX_PERIOD_CHG( Btn_GetParm(BTN_PARM_AUD_CPS_MAX_PERIOD_IDX) );
		break;
		
	case EVNT_AUD_CPS_MIN_PERIOD_CHG:
		USER_AUD_CPS_MIN_PERIOD_CHG( Btn_GetParm(BTN_PARM_AUD_CPS_MIN_PERIOD_IDX) );
		break;
		
	case EVNT_AUD_CPS_RPT_TRUE_HEADING_CHG:
		USER_AUD_CPS_RPT_TRUE_HEADING( Btn_GetParm( BTN_PARM_AUD_CPS_RPT_TRUE_HEADING_IDX ) );
		break;
	
    case EVNT_XNG_LOOP_A_CHG:
    case EVNT_XNG_LOOP_B_CHG:
    case EVNT_XNG_LOOP_USB_CHG:
    case EVNT_XNG_A2B_CHG:
    case EVNT_XNG_B2A_CHG:   
    case EVNT_XNG_U2A_CHG:
    case EVNT_XNG_A2U_CHG:
    case EVNT_XNG_B2U_CHG:
    case EVNT_XNG_U2B_CHG:
        USER_UPDATE_SWITCHING();
        break;
        
    case EVNT_TGT_CNFG_CHG:
        break;
        
    case EVNT_USART_A_SPEED_CHG:
        USER_UPDATE_PORT_A_SPEED();
        break;
        
    case EVNT_USART_B_SPEED_CHG:
        USER_UPDATE_PORT_B_SPEED();
		break;

    case EVNT_HDG_CAL_ADJ_CHG: 
    	USER_HDG_CAL_ADJ_CHG( Btn_GetParm(BTN_PARM_HDG_CAL_ADJ_IDX) );
    	break;

    case EVNT_CNFG_MIN_MAX_DELTA_MODE_CHG:
        break;
        
	default:
        Btn_Ctl.CodeErrs++;
		//Core_CodeError( BTN_FATAL_ERROR );
		break;
		
   }
 return( EvSw_result );
}

// Loads up Btn_Ctl with info re menu items spec'd with index
void _btn_new_menu_item( int8 index )
{
 int8 i;
#ifdef USE_NAV_KEYS 	  			
	i = Btn_Ctl.menu_ptr->items[ Btn_Ctl.menu_ptr->index ];
#else
    i = index;
#endif
    Btn_Ctl.menu_item_idx = i;
    i = menu_items[ (int)i ].menuparm_idx;
    Btn_Ctl.new_parm_val = _cnfg_unpacked_uint8[ (int)_btn_parm_chg_profiles[ (int)i ][ (int)UNPACKED_ADDR_IDX ] ];
}

//
// Increments parameter spec'd with parm-index in menu_items[]
// Saves to flash image
// executes any callbacks
// Announces new value
//
void _btn_toggle_parm( int parm_index )
{
    int i;
                    
    _btn_new_menu_item(parm_index);
    //change the parm
    i = menu_items[ (int)Btn_Ctl.menu_item_idx ].menuparm_idx;
    if ( Btn_Ctl.new_parm_val )
    	Btn_Ctl.new_parm_val = 0;
    else
        Btn_Ctl.new_parm_val = 1;
    
    //Store the new value and update the system
	_cnfg_unpacked_uint8[ _btn_parm_chg_profiles[ i ][ UNPACKED_ADDR_IDX ] ] = Btn_Ctl.new_parm_val;	
					
	_btn_event_switch( _btn_parm_chg_profiles[ i ][ EVENT_IDX ] );
	_btn_announce_item_value( Btn_Ctl.menu_item_idx, Btn_Ctl.new_parm_val, Btn_Ctl.lastkeypressed, TRUE );
}
//
// Increments parameter spec'd with parm-index in menu_items[]
// Saves to flash image
// executes any callbacks
// Announces new value
//
void _btn_increment_parm( int parm_index )
{
    int i;
                    
    _btn_new_menu_item(parm_index);
    //change the parm
    i = menu_items[ (int)Btn_Ctl.menu_item_idx ].menuparm_idx;
    if ( Btn_Ctl.new_parm_val < _btn_parm_chg_profiles[ i ][ MAX_VALUE_IDX ] )
    	Btn_Ctl.new_parm_val++;
    
    //Store the new value and update the system
	_cnfg_unpacked_uint8[ _btn_parm_chg_profiles[ i ][ UNPACKED_ADDR_IDX ] ] = Btn_Ctl.new_parm_val;	
					
	_btn_event_switch( _btn_parm_chg_profiles[ i ][ EVENT_IDX ] );
	_btn_announce_item_value( Btn_Ctl.menu_item_idx, Btn_Ctl.new_parm_val, Btn_Ctl.lastkeypressed, TRUE );
}
//
// Decrements parameter spec'd with parm-index in menu_items[]
// Saves to flash image
// executes any callbacks
// Announces new value
void _btn_decrement_parm( int parm_index )
{          
    int i;
    
    _btn_new_menu_item( parm_index );
    i = menu_items[ (int)Btn_Ctl.menu_item_idx ].menuparm_idx;
    //change the parm
    if (Btn_Ctl.new_parm_val)
    	Btn_Ctl.new_parm_val--;
    				
       //Store the new value and update the system
	_cnfg_unpacked_uint8[ _btn_parm_chg_profiles[ i ][ UNPACKED_ADDR_IDX ] ] = Btn_Ctl.new_parm_val;	
					
	_btn_event_switch( _btn_parm_chg_profiles[ i ][ EVENT_IDX ] );
    _btn_announce_item_value( Btn_Ctl.menu_item_idx, Btn_Ctl.new_parm_val, Btn_Ctl.lastkeypressed,TRUE );
}

//////////////////////////////////////////////////////////////////////////////
// uint16 _btn_get_raw_keys()
//
// Description: Called every 10ms from _btn_check_keys. 
//	Returns the 4 raw key inputs for the current key row
//  in bits 0-3. Pressed keys are 1s.
//
//*****************************************************************************
uint16 _btn_get_raw_keys()
{
 uint16 temp16;
 
 	// Get the raw button states. Active low
 	temp16 = (PORTB & PORTB_BTN_MASK) >> PORTB_BTN_SHIFT;
 	return( temp16 ^ 0x000F);
}

//////////////////////////////////////////////////////////////////////////////
// void _btn_set_key_row( uint8 row )
//
// Description: Called every 10ms from _btn_check_keys. 
//	Sets the key row.
//
//*****************************************************************************
void _btn_set_key_row( uint8 row )
{
	PORTC |= PORTC_ROW_MASK;
	
	switch( row )
	{
		case 0:
			BTN_ROW_0 = 0;
			break;	

		case 1:
			BTN_ROW_1 = 0;
			break;	

		case 2:
			BTN_ROW_2 = 0;
			break;
							
		case 3:
			BTN_ROW_3 = 0;
			break;	
	}
}

//////////////////////////////////////////////////////////////////////////////
// int8 _btn_check_keys()
//
// Description: Called every 10ms from Btn_ProcTask. 
//	Scans all 4 key rows and updates the key bitmasks in
//	Btn_Ctl. Returns BTN_SGNL_KEYPRESSED when a key
//	press is detected and returns SGNL_NO_REQUEUE otherwise.
//
// Bit definitions for Btn_inputs and Btn_inputs0
//#define       BTN_INPUTS_HDG_BIT          0
//#define       BTN_INPUTS_MUTE_BIT 		1
//#define       BTN_INPUTS_CNFG_SAVE_BIT    2
//#define       BTN_INPUTS_F0_BIT           3
//
//#define   	BTN_INPUTS_TGT_DN_BIT   	4
//#define   	BTN_INPUTS_TGT_TGL_BIT   	5
//#define   	BTN_INPUTS_TGT_UP_BIT   	6
//#define   	BTN_INPUTS_ERR_BIT          7
//
//#define   	BTN_INPUTS_PER_DN_BIT   	8
//#define   	BTN_INPUTS_PER_UP_BIT   	9
//#define   	BTN_INPUTS_RES_DN_BIT   	10
//#define   	BTN_INPUTS_RES_UP_BIT   	11
//
//*****************************************************************************
int8 _btn_check_keys(int8 signal)
{
 int8 RtnSignal;
 uint16 i, temp;
 
		RtnSignal = signal & _BTN_SGNL_CHECK_KEYS;
		
		// Clear the bits for this row
		Btn_Ctl.keys[0] &= (0x000F << (Btn_Ctl.kb_row << 2)) ^ 0xFFFF;
		
		// Or in the new raw bits. key pressed is a 1.
		Btn_Ctl.keys[0] |= (_btn_get_raw_keys() << (Btn_Ctl.kb_row << 2));
		
		// Set up for the next row
		Btn_Ctl.kb_row++;
		Btn_Ctl.kb_row = Btn_Ctl.kb_row & 0x03;
		_btn_set_key_row( Btn_Ctl.kb_row );		
		
		// if we've scanned all rows
		if (!Btn_Ctl.kb_row)
		{
			Btn_Ctl.keys[0] ^= Btn_Ctl.keystate;		// negate any active switches

			// Debounce for two samples
			temp = Btn_Ctl.keys[0] & Btn_Ctl.keys[1];   // pressed keys are 1
            temp &= BTN_INPUT_KEY_MASK;
			
			//i will hold map of newly pressed key(s)
			i = (temp ^ Btn_Ctl.keys[2]) & temp;

			Btn_Ctl.keys[1] = Btn_Ctl.keys[0];
			Btn_Ctl.keys[2] = temp & _BTN_INPUTS_F0_MASK; // always detect F0 depressed
 			
 			// Check the remote button state variable
 			if ( Btn_Ctl.remote_btn_state )
 			{
  				i = Btn_Ctl.remote_btn_state &0x7FFF;
  				Btn_Ctl.remote_btn_state = 0;
 			}
 			
 			if (i && (i != BTN_INPUTS_F0_MASK))
 			{
                if (Btn_Ctl.keysPressed != i)
                    Btn_Ctl.keysPressedCnt = 0;
                else
                    Btn_Ctl.keysPressedCnt++;
                
                Btn_Ctl.keysPressed = i;

				Btn_Ctl.lastkeypressed = i;
 
                // No signal if only F0 pressed
                return( RtnSignal | BTN_SGNL_KEYPRESSED );
	 		}
	 	}	
 		return( RtnSignal );
}

//////////////////////////////////////////////////////////////////////////////
// Btn_ProcTask
//
// Description: Called every 10ms from Core.c. Reads the button inputs from
//    and updates variables and LEDs. 
//
//*****************************************************************************
int8 Btn_ProcTask( int8 signal )
{
 int8 RtnSignal;
 uint16 i;

 static int _btn_power_up = 500;
 
 	if (_btn_power_up)
 	{
	  	if (_btn_power_up == 1)
	  	{
			Btn_report_hw_cnfg_status();
		}
		_btn_power_up--;	
#ifdef USE_HOTKEYS
		// if coming out of power-up, check the button states
		// and set the ketstate so that the hotkeys are announced
		if (_btn_power_up == 0)
		{
			_btn_check_keys( signal );
			Btn_Ctl.keystate = Btn_Ctl.keys[0] ^ (HOTKEY_0_BTN_MASK);
		}
#endif
		return(0);
	}

 	RtnSignal = signal;

	// Happens every 10 ms
	if (signal & BTN_SGNL_CHECK_KEYS)
	{
		if (Btn_Ctl.dialog_cnt)
			Btn_Ctl.dialog_cnt--;
		else
			i =0;	// something to break on
			
		return( _btn_check_keys( signal ) );
 
 	} // if (signal & BTN_SGNL_CHECK_KEYS_MASK)

	if (signal & BTN_SGNL_KEYPRESSED)
	{
		RtnSignal &= _BTN_SGNL_KEYPRESSED;
 		
        // Unmute if muted and non-mute key pressed
		if ((BTN_INPUTS_MUTE_MASK != Btn_Ctl.keysPressed) && Btn_Ctl.speech_muted)
		{
			Btn_Ctl.speech_muted = 0;
			Tts_UnMute();
		}  
		
        // non-zero Btn_Ctl.dialog_cnt suppresses announces for a time
		if ( Btn_Ctl.keysPressed & DLG_KEY_MASK )
		{
			Btn_Ctl.dialog_cnt = BTN_DIALOG_CNT_INIT;
		}
  			
 		// process button 
 		switch (Btn_Ctl.keysPressed) 
 		{
  			// Target UP button pushed
  			case BTN_INPUTS_TGT_UP_MASK:   
   				_btn_event_switch( EVNT_TGT_UP );
   				break;

  			// Target DOWN button pushed
  			case BTN_INPUTS_TGT_DN_MASK:   
   				_btn_event_switch( EVNT_TGT_DN ); 
   				break;

  			// Targeting mode toggle button pushed
  			case BTN_INPUTS_TGT_TGL_MASK:
   
   				if ( Btn_Ctl.target_mode == 1)
    			{
	 				Btn_TargetOff();
    			}
    			else
    			{
     				// turn on targeting
    				_btn_event_switch( EVNT_TGT_ON );
    			}
    			break;
    
   			
    		case BTN_INPUTS_CNFG_SAVE_MASK:
    		
    			//_btn_event_switch( BtnParmEvtIdx );
    			Cnfg_SaveSettings( NO_FLASH_CLEAR );
    			TtsSaySomething( TTS_ADDR_CNFG_SAVED_INDEX, 0, 0, 0, TTS_OPT_HIGH_PRIORITY_QUEUE_MASK | TTS_POSTDELAY_300MS);

  				break;

   			// Speech volume mute pushed
  			case BTN_INPUTS_MUTE_MASK :   
  			
   				Btn_Ctl.speech_muted ^= 1;
         
   				_btn_event_switch( EVNT_MUTE_CHG ); 
    			break;

			case BTN_INPUTS_HDG_MASK:
				_btn_event_switch( EVNT_HEADING_RPT ); 
				break;
                
            case BTN_INPUTS_MIN_MAX_DELTA_CNFG_CHG_MASK:
                
                _btn_toggle_parm( CNFG_MIN_MAX_DELTA_MODE_ITEM_IDX );
                break;
                
            case BTN_INPUTS_MIN_PER_DN_MASK:
                
                
                switch (Btn_GetParm(BTN_PARM_CNFG_MIN_MAX_DELTA_MODE_IDX)) {
                    
                    default:
                    case MIN_MAX_DELTA_COMPASS:
                        _btn_decrement_parm( AUD_CPS_MIN_PERIOD_PARM_ITEM_IDX );
                        break;
                        
                    case MIN_MAX_DELTA_SPEED:
                        _btn_decrement_parm( VOG_MIN_PERIOD_PARM_ITEM_IDX );
                        break;
                }
                break;
                
            case BTN_INPUTS_MIN_PER_UP_MASK:
                switch (Btn_GetParm(BTN_PARM_CNFG_MIN_MAX_DELTA_MODE_IDX)) {
                    
                    default:
                    case MIN_MAX_DELTA_COMPASS:                
                        _btn_increment_parm( AUD_CPS_MIN_PERIOD_PARM_ITEM_IDX );
                        break;
                        
                    case MIN_MAX_DELTA_SPEED:
                        _btn_increment_parm( VOG_MIN_PERIOD_PARM_ITEM_IDX );
                        break;
                }                          
                break;
                
            case BTN_INPUTS_MAX_PER_DN_MASK:
                switch (Btn_GetParm(BTN_PARM_CNFG_MIN_MAX_DELTA_MODE_IDX)) {
                    
                    default:
                    case MIN_MAX_DELTA_COMPASS:                  
                        _btn_decrement_parm( AUD_CPS_MAX_PERIOD_PARM_ITEM_IDX );
                        break;
                        
                    case MIN_MAX_DELTA_SPEED:
                        _btn_decrement_parm( VOG_MAX_PERIOD_PARM_ITEM_IDX );
                        break;
                }               
                break;
                
            case BTN_INPUTS_MAX_PER_UP_MASK:
                switch (Btn_GetParm(BTN_PARM_CNFG_MIN_MAX_DELTA_MODE_IDX)) {
                    
                    default:
                    case MIN_MAX_DELTA_COMPASS:                  
                        _btn_increment_parm( AUD_CPS_MAX_PERIOD_PARM_ITEM_IDX );
                        break;
                        
                    case MIN_MAX_DELTA_SPEED:
                        _btn_increment_parm( VOG_MAX_PERIOD_PARM_ITEM_IDX );
                        break;
                }
                break;
                
            case BTN_INPUTS_DELTA_DN_MASK:
                switch (Btn_GetParm(BTN_PARM_CNFG_MIN_MAX_DELTA_MODE_IDX)) {
                    
                    default:
                    case MIN_MAX_DELTA_COMPASS:                 
                        _btn_decrement_parm( AUD_CPS_DELTA_INTERVAL_PARM_ITEM_IDX );
                        break;
                        
                    case MIN_MAX_DELTA_SPEED:
                        _btn_decrement_parm( VOG_DELTA_INTERVAL_PARM_ITEM_IDX );
                        break;
                }        
                break;
                
            case BTN_INPUTS_DELTA_UP_MASK:
                switch (Btn_GetParm(BTN_PARM_CNFG_MIN_MAX_DELTA_MODE_IDX)) {
                    
                    default:  
                    case MIN_MAX_DELTA_COMPASS:
                        _btn_increment_parm( AUD_CPS_DELTA_INTERVAL_PARM_ITEM_IDX );
                        break;
                        
                    case MIN_MAX_DELTA_SPEED:
                        _btn_increment_parm( VOG_DELTA_INTERVAL_PARM_ITEM_IDX );
                        break;
                }
                break;
                
            case BTN_INPUTS_YAW_TONE_MASK:
                
                _btn_toggle_parm( AUD_CPS_DELTA_TONES_PARM_ITEM_IDX );
                break;        
                
            case BTN_INPUTS_HDG_CAL_ADJ_DN_MASK:
                
                _btn_decrement_parm( HDG_CAL_ADJ_PARM_ITEM_IDX );
                break;
                
            case BTN_INPUTS_HDG_CAL_ADJ_UP_MASK:

                _btn_increment_parm( HDG_CAL_ADJ_PARM_ITEM_IDX );
                break;                
				
			case BTN_INPUTS_FACTORY_RESET_MASK:
			
				_cnfg_EraseCnfgFlash();
				_cnfg_EraseCnfgFlash2();
   				reset_cpu();
    			break;
#ifdef USE_NAV_KEYS
                
  			case BTN_INPUTS_NAV_UP_BIT:

    			//change the parm, and wrap if necessary
    			i = menu_items[ (int)Btn_Ctl.menu_item_idx ].menuparm_idx;
    			if ( Btn_Ctl.new_parm_val < _btn_parm_chg_profiles[ i ][ MAX_VALUE_IDX ] )
    				Btn_Ctl.new_parm_val++;
    			
    			_btn_announce_item_value( Btn_Ctl.menu_item_idx, Btn_Ctl.new_parm_val, Btn_Ctl.lastkeypressed, TRUE );			 
    
  				break;
  			
    		// configuration setting down nav button pushed
  			case BTN_INPUTS_NAV_DN_BIT:

    			//change the parm, and wrap if necessary
    			if (Btn_Ctl.new_parm_val)
    				Btn_Ctl.new_parm_val--;
    				
    			_btn_announce_item_value( Btn_Ctl.menu_item_idx, Btn_Ctl.new_parm_val, Btn_Ctl.lastkeypressed,TRUE );
    			
  				break;
  			

  			// Right configuration mode button pushed
  			case BTN_INPUTS_NAV_RT_BIT :
  			
  				i = TRUE;
  				do 
  				{
  					Btn_Ctl.menu_ptr->index++;
  					if ( Btn_Ctl.menu_ptr->index > Btn_Ctl.menu_ptr->max_index )
  					{
	  					Btn_Ctl.menu_ptr->index = 0;
	  				}
	  				
	  				_btn_new_menu_item( Btn_Ctl.menu_ptr->index );
	  				if ( menu_items[ (int)Btn_Ctl.menu_item_idx ].type != MENU_ITEM_TYPE_UNAVAILABLE)
	  				{
      					_btn_announce_item_value( Btn_Ctl.menu_item_idx, Btn_Ctl.new_parm_val, Btn_Ctl.lastkeypressed, TRUE );
      					i = FALSE;
      				}			
      				
      			} while( i );			

    			break;
    		
  			// Left configuration mode button pushed
  			case BTN_INPUTS_NAV_LF_BIT :

				i = TRUE;
				do
				{
  					if ( Btn_Ctl.menu_ptr->index == 0 )
  					{
	  					Btn_Ctl.menu_ptr->index = Btn_Ctl.menu_ptr->max_index ;
	  				}
	  				else
	  				{
						Btn_Ctl.menu_ptr->index--;		  			
		  			}
		  			
		  			_btn_new_menu_item( Btn_Ctl.menu_ptr->index );
		  			if ( menu_items[ (int)Btn_Ctl.menu_item_idx ].type != MENU_ITEM_TYPE_UNAVAILABLE)
	  				{		
      					_btn_announce_item_value( Btn_Ctl.menu_item_idx, Btn_Ctl.new_parm_val, Btn_Ctl.lastkeypressed, TRUE );  		
      					i = FALSE;
      				}			
      				
    			} while( i );			

    			break;    
    		
  		  	// configuration setting change select button pushed

 		  	// configuration setting change select button pushed
  			case BTN_INPUTS_NAV_SEL_BIT:

				// Get the item type
				i = menu_items[ (int)Btn_Ctl.menu_item_idx ].type;
				switch( i )
				{
					case MENU_ITEM_TYPE_PARM:
						//Store the new value and update the system
						i = menu_items[ (int)Btn_Ctl.menu_item_idx ].menuparm_idx;
						_cnfg_unpacked_uint8[ _btn_parm_chg_profiles[ i ][ UNPACKED_ADDR_IDX ] ] = Btn_Ctl.new_parm_val;	
					
						_btn_event_switch( _btn_parm_chg_profiles[ i ][ EVENT_IDX ] );
						_btn_announce_item_value( Btn_Ctl.menu_item_idx, Btn_Ctl.new_parm_val, Btn_Ctl.lastkeypressed, TRUE );
						break;

					case MENU_ITEM_TYPE_SUBMENU:
					
						i = menu_items[ (int)Btn_Ctl.menu_item_idx ].menuparm_idx;
						Btn_Ctl.menu_ptr = menus[ menu_items[ (int)Btn_Ctl.menu_item_idx ].menuparm_idx ];
					
						//Announce the menu
      					_btn_announce_item_value( Btn_Ctl.menu_ptr->menu_item_index, 0, Btn_Ctl.lastkeypressed, TRUE ); 
      			
						_btn_new_menu_item( Btn_Ctl.menu_ptr->index );
      			
      					_btn_announce_item_value( Btn_Ctl.menu_item_idx, Btn_Ctl.new_parm_val, 0xFF, TRUE ); 
      					break;
      									
      				case MENU_ITEM_TYPE_REPORT:

						_btn_announce_item_value( Btn_Ctl.menu_item_idx, Btn_Ctl.new_parm_val, Btn_Ctl.lastkeypressed, TRUE );
						break;
						
					default:
						Core_CodeError( BTN_FATAL_ERROR );
						break;
				}
    			break;
                
                case BTN_INPUTS_STATUS_RPT_BIT:
			
				_btn_event_switch( EVNT_STATUS_RPT ); 
				break;

#endif

  			default:   break;
  			
  		} //switch(i)	
 	}
 	return( SGNL_NO_REQUEUE ); // No Requeue
}





