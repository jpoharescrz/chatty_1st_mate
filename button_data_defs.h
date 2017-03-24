
#ifndef BUTTON_DATA_DEFS_H
#define BUTTON_DATA_DEFS_H

#include "button_Cnfg.h"

#define CNFG_MAX   2

enum heading_src {
 SRC_HDG_XXHDM,
 SRC_HDG_XXHDT,
 SRC_HDG_XXHDG,
 SRC_HDG_XXRMC,
 SRC_HDG_NONE
};

#define MAX_SRC_HDG SRC_HDG_NONE

enum btn_resolution_indices {
 BTN_RESOLUTION_LOW,
 BTN_RESOLUTION_MED,
 BTN_RESOLUTION_HIGH,
 BTN_RESOLUTION_TYPE_CNT
};
#define MAX_RESOLUTION_IDX (BTN_RESOLUTION_TYPE_CNT-1)

enum btn_tgt_cnfg_indices {
 BTN_PARM_TGT_CNFG_AUTOMATIC,
 BTN_PARM_TGT_CNFG_HEADING,
 BTN_PARM_TGT_CNFG_WAYPOINT,
 BTN_PARM_TGT_CNFG_TYPE_CNT
};
#define MAX_TGT_CNFG_IDX (BTN_PARM_TGT_CNFG_TYPE_CNT-1)

#define BTN_PARM_NAV_TO_WPT_EXTERNAL 0

#define MAX_TGT_MODE_IDX 1
#define MAX_VOG_DELTA_INTERVAL_IDX 7
#define MAX_VOG_MAX_PERIOD_IDX 7
#define MAX_VOG_MIN_PERIOD_IDX 7
#define MAX_NONTARGET_BAR_DISP_MODE_IDX 1
#define MAX_SPEECH_VOL_IDX 2
#define MAX_TONE_VOL_IDX 3
#define MAX_SOURCE_IDX SRC_HDG_NONE

#define MAX_BUZZER_VOL_IDX 3
#define MAX_LVL_DGT_DISP_IDX 3
#define MAX_LVL_BAR_DISP_IDX 3
#define MAX_TGT_BAR_DISP_IDX 3
#define MAX_CNFG_MENU_IDX CNFG_MAX
#define MAX_ON_OFF_IDX 1
#define MAX_FMT_FREQ_IDX 201
#define MAX_USART_SPEED_IDX 3

#define MAX_NAV_TO_WPT_IDX 7

//////////////////////////////////////////////////////////////////
// This is the packed structure as it sits in the flash
//////////////////////////////////////////////////////////////////
typedef struct _cnfg_packed_struct {
uint8 next_page_offset;
uint8 vog_delta_interval:4;     // VOG delta interval
uint8 vog_max_period:4;         // VOG max period
uint8 vog_min_period : 4;      	// VOG min period
uint8 nontarget_bar_disp_mode:2; 
uint8 target_bar_disp_mode:2; //low(0), High(1)
uint8 null_0;				// every 4th byte ignored
uint8 speech_volume:4;   	// off(0), Low(1), med(2), high(3)
uint8 tone_volume:4;     	// off(0), Low(1), med(2), high(3)
uint8 source:4;         	// compass(0), wind vane(1), gps nav xtrack(2)
uint8 res_mode:4;        	// low(0), med(1), high(2)
uint8 buzzer_volume:2;   	// off(0), low(1), med(2), high(3)
uint8 lvl_dgt_dsply:3;   	// off(0), low(1), med(2), high(3)
uint8 lvl_bar_dsply:3;   	// off(0), low(1), med(2), high(3)
uint8 null_1;				// every 4th byte ignored
uint8 fmt_freq;				// fm txmtr freq
uint8 fmt_on:1;				// fmt on air
uint8 gps_on:1;				// gps module used
uint8 gps_nav_on:1;			// gps module navigation used
uint8 xng_loop_a:1;			// switching loop A
uint8 xng_loop_b:1;			// switching loop B
uint8 xng_a2b:1;			// switching A to B
uint8 xng_b2a:1;			// switching B to A
uint8 aud_cps:1;				// audio compass on/off
uint8 aud_cps_delta:1;			// audio compass delta signal on/off
uint8 aud_cps_delta_interval:3;		// audio compass delta trigger inteval
uint8 aud_cps_rpt_max_period:3;		// audio compass report max period
uint8 per_rpt_vog:1;				// vog report switch
uint8 null_2;						// every 4th byte ignored
uint8 aud_cps_rpt_min_period:3;		// audio compass report min period
uint8 aud_cps_rpt_true_heading:1;		// report headings as True(1) or Magnetic(0)
uint8 usart_a_speed:2;				// Port A Speed configuration
uint8 usart_b_speed:2;				// Port B Speed configuration
uint8 tgt_cnfg:2;					// target configuration
uint8 nav_to_wpt:3;					// waypoint to navigate to
uint8 empty_3:3;
uint8 heading_cal_adj;				// tweaks heading for environmental offsets, range -12.7 to + 12.7 degrees
uint8 null_3;
uint8 xng_a2u:1;			// switching A to USB
uint8 xng_b2u:1;			// switching B to USB
uint8 xng_loop_usb:1;			// switching loop USB
uint8 xng_u2a:1;			// switching USB to a
uint8 xng_u2b:1;			// switching USB to b
uint8 min_max_delta_mode:1;
uint8 reserved:2;
uint8 null_4[3];
} _cnfg_packed_struct_type;

#define CNFG_STR_SIZE sizeof(_cnfg_packed_struct_type)
#ifdef BUTTON

union {
   _cnfg_packed_struct_type _cnfg_packed_structure;
   uint8 _cnfg_packed_array[ sizeof( _cnfg_packed_struct_type ) ];
} _cnfg_packed_union;

#define _cnfg_packed _cnfg_packed_union._cnfg_packed_structure
#define _cnfg_pack_uint8 _cnfg_packed_union._cnfg_packed_array


// indices used for _cnfg_unpacked structure
enum btn_cnfg_unpacked_indices {
 NEXT_PAGE_PTR_IDX,
 BTN_STT_VOG_DELTA_INTERVAL_IDX,
 BTN_STT_VOG_MAX_INTERVAL_IDX,
 NULL0_IDX,
 BTN_STT_VOG_MIN_INTERVAL_IDX,
 BTN_STT_NONTARGET_BAR_DISP_MODE_IDX,
 BTN_STT_SPEECH_VOL_IDX,
 NULL1_IDX,
 BTN_STT_TONE_VOL_IDX,
 BTN_STT_SOURCE_IDX,
 BTN_STT_RESOLUTION_IDX,
 NULL2_IDX,
 BTN_STT_BUZZER_VOL_IDX,
 BTN_STT_LVL_DGT_DISP_IDX,
 BTN_STT_LVL_BAR_DISP_IDX,
 NULL3_IDX,
 BTN_STT_TGT_BAR_DISP_IDX,
 BTN_STT_CNFG_MENU_IDX,
 BTN_STT_FMT_FREQ_IDX,
 NULL4_IDX,
 BTN_STT_FMT_ON_IDX,   
 BTN_STT_GPS_ON_IDX,
 BTN_STT_GPS_NAV_ON_IDX,
 NULL5_IDX,
 BTN_STT_XNG_LOOP_A_IDX,
 BTN_STT_XNG_LOOP_B_IDX,
 BTN_STT_XNG_A2B_IDX,
 NULL6_IDX,
 BTN_STT_XNG_B2A_IDX,        
 BTN_STT_AUD_CPS_ON_IDX,
 BTN_STT_AUD_CPS_DELTA_TONES_IDX,
 NULL7_IDX,
 BTN_STT_AUD_CPS_DELTA_INTERVAL_IDX,         
 BTN_STT_AUD_CPS_MAX_PERIOD_IDX,
 BTN_STT_PER_RPT_VOG_ON_IDX,
 NULL8_IDX,
 BTN_STT_AUD_CPS_MIN_PERIOD_IDX,       
 BTN_STT_AUD_CPS_RPT_TRUE_HEADING_IDX,
 BTN_STT_USART_A_SPEED_IDX,
 NULL9_IDX,
 BTN_STT_USART_B_SPEED_IDX, 
 BTN_STT_TGT_CNFG_IDX,
 BTN_STT_NAV_TO_WPT_IDX,
 NULL10_IDX,
 BTN_STT_HDG_CAL_ADJ_IDX,
 BTN_STT_XNG_A2U_IDX,
 BTN_STT_XNG_B2U_IDX,
 NULL11_IDX,
 BTN_STT_XNG_LOOP_USB_IDX,
 BTN_STT_XNG_U2A_IDX,
 BTN_STT_XNG_U2B_IDX,
 NULL12_IDX,
 BTN_STT_CNFG_MIN_MAX_DELTA_MODE_ITEM_IDX,
 BTN_STT_MAX_IDX
};
///////////////////////////////////////////////////////////////
// This is the unpacked structure as it sits in memory
//
// Btn_states holds the operating states resulting from 
// button operations
///////////////////////////////////////////////////////////////
typedef struct _cnfg_unpacked_struct {
uint8 next_page_offset;
uint8 vog_delta_interval;   // VOG delta interval
uint8 vog_max_period;      	// VOG max period
uint8 null0;				// every fourth ignored in flash
uint8 vog_min_period;      	// VOG min period
uint8 nontarget_bar_disp_mode; 
uint8 speech_volume;      	// off(0), Low(1), med(2), high(3)
uint8 null1;				// every fourth ignored in flash
uint8 tone_volume;      	// off(0), Low(1), med(2), high(3)
uint8 source;         		// compass(0), wind vane(1)
uint8 res_mode;         	// low(0), med(1), high(2)
uint8 null2;				// every fourth ignored in flash
uint8 buzzer_volume;        // off(0), low(1), med(2), high(3)
uint8 lvl_dgt_dsply;      	// off(0), low(1), med(2), high(3)
uint8 lvl_bar_dsply;    	// off(0), low(1), med(2), high(3)
uint8 null3;				// every fourth ignored in flash
uint8 target_bar_disp_mode; //low(0), High(1)
uint8 cnfg_menu;         	// source(0), res(1), tone(2), dgt(3), bar(4)
uint8 fmt_freq;				// fm txmtr freq
uint8 null4;
uint8 fmt_on;
uint8 gps_on;
uint8 gps_nav_on;
uint8 null5;
uint8 xng_loop_a;
uint8 xng_loop_b;
uint8 xng_a2b;
uint8 null6;
uint8 xng_b2a;
uint8 aud_cps;						// heading report on/off
uint8 aud_cps_delta;				// periodic heading report delta signal on/off
uint8 null7;
uint8 aud_cps_delta_interval;		// audio compass delta trigger inteval
uint8 aud_cps_rpt_max_period;		// audio compass report max period
uint8 per_rpt_vog;					// vog report switch
uint8 null18;
uint8 aud_cps_rpt_min_period;		// audio compass report min period
uint8 aud_cps_rpt_true_heading;			// report headings as True(1) or Magnetic(0)
uint8 usart_a_speed;				// Port A Speed configuration
uint8 null19;
uint8 usart_b_speed;				// Port B Speed configuration
uint8 tgt_cnfg;						// targeting configuration
uint8 nav_to_wpt;					// waypoint to navigate to
uint8 null10;
uint8 heading_cal_adj;				// tweaks heading for environmental offsets, range -12.7 to + 12.7 degrees
uint8 xng_a2u;                      // switches port a traffic to usb
uint8 xng_b2u;                      // switches port b traffic to usb
uint8 null11;
uint8 xng_loop_usb;                 // loop usb
uint8 xng_u2a;                      // switches port usb traffic to a
uint8 xng_u2b;                      // switches port usb traffic to b
uint8 null12;
uint8 min_max_delta_mode;
} _cnfg_unpacked_struct_type;


union {
   _cnfg_unpacked_struct_type _cnfg_unpacked_structure;
   uint8 _cnfg_unpacked_array[ sizeof( _cnfg_unpacked_struct_type ) ];
} _cnfg_unpacked_union;

#define _cnfg_unpacked _cnfg_unpacked_union._cnfg_unpacked_structure
#define _cnfg_unpacked_uint8 _cnfg_unpacked_union._cnfg_unpacked_array

#endif

// Indices used for _btn_parm_chg_profiles[]
enum btn_parm_chg_profile_indices {
 BTN_PARM_VOG_DELTA_INTERVAL_IDX, 
 BTN_PARM_VOG_MAX_PERIOD_IDX, 
 BTN_PARM_VOG_MIN_PERIOD_IDX, 
 BTN_PARM_CNFG_MIN_MAX_DELTA_MODE_IDX,
 //BTN_PARM_NONTGT_DISP_MODE_IDX, 
 BTN_PARM_SPEECH_VOL_IDX, 
 //BTN_PARM_TONE_VOL_IDX, 
 BTN_PARM_SOURCE_IDX, 
 BTN_PARM_RESOLUTION_IDX, 
 //BTN_PARM_BUZZER_VOL_IDX, 
 BTN_PARM_LVL_DGT_DISP_IDX, 
 BTN_PARM_LVL_BAR_DISP_IDX, 
 //BTN_PARM_TGT_DISP_MODE_IDX, 
 //BTN_PARM_CNFG_MENU_IDX, 
 BTN_PARM_FMT_FREQ_IDX, 
 BTN_PARM_FMT_ON_IDX, 
#ifdef ONBOARD_GPS_MODULE
 BTN_PARM_GPS_ON_IDX, 
 BTN_PARM_GPS_NAV_ON_IDX,
#endif 
 BTN_PARM_XNG_LOOP_A_IDX, 
 BTN_PARM_XNG_LOOP_B_IDX, 
 BTN_PARM_XNG_LOOP_USB_IDX,
 BTN_PARM_XNG_A2B_IDX, 
 BTN_PARM_XNG_B2A_IDX, 
 BTN_PARM_XNG_U2A_IDX,
 BTN_PARM_XNG_A2U_IDX, 
 BTN_PARM_XNG_B2U_IDX, 
 BTN_PARM_XNG_U2B_IDX, 
 BTN_PARM_AUD_CPS_IDX, 
 BTN_PARM_AUD_CPS_DELTA_TONES_IDX, 
 BTN_PARM_AUD_CPS_DELTA_INTERVAL_IDX, 
 BTN_PARM_AUD_CPS_MAX_PERIOD_IDX, 
 BTN_PARM_PER_RPT_VOG_IDX, 
 BTN_PARM_AUD_CPS_MIN_PERIOD_IDX, 
 BTN_PARM_AUD_CPS_RPT_TRUE_HEADING_IDX, 
 BTN_PARM_USART_A_SPEED_IDX, 
 BTN_PARM_USART_B_SPEED_IDX, 
 BTN_PARM_TGT_CNFG_IDX, 
 BTN_PARM_NAV_TO_WPT_IDX, 
 BTN_PARM_HDG_CAL_ADJ_IDX,
 MAX_BTN_CHG_PROFILE_IDX
};

#ifdef BUTTON

#define MAX_PARM_CHG_PROFILES (MAX_BTN_CHG_PROFILE_IDX)

// No reports for 6.5 seconds after a nav key pressed
#define	BTN_DIALOG_CNT_INIT	650


#define MAX_PARM_CHG_LEVELS 4

enum menu_item_types {
 MENU_ITEM_TYPE_UNAVAILABLE,
 MENU_ITEM_TYPE_PARM,
 MENU_ITEM_TYPE_SUBMENU,
 MENU_ITEM_TYPE_REPORT,
 MENU_ITEM_TYPE_1_TIME_FLASH_PARM,
 MENU_ITEM_TYPE_CNT
};

typedef struct menu_item_struct 
{
	uint8	type;						//parameter(0) or submenu(1) or unavailable(2)
	uint8	menuparm_idx;				//parm or menu location
} menu_item;

typedef struct menu_struct {
	uint8				max_index;
	uint8				index;
	uint8				menu_item_index;
	int8				*items;
	struct menu_struct	*parent;
} menu;

//menu *menus[] = {&main_menu, &system_menu, &gps_menu, &fmt_menu, &port_options_menu, &per_rpt_menu, &aud_cps_menu };
enum menu_indices {
 MAIN_MENU_IDX, 
 SYSTEM_MENU_IDX, 
#ifdef ONBOARD_GPS_MUDULE
 GPS_MOD_MENU_IDX, 
#endif
 FMT_MOD_MENU_IDX, 
 PORT_OPTIONS_MENU_IDX, 
 PER_RPT_MENU_IDX, 
 AUD_CPS_MENU_IDX 
};

#define MENU_ITEM_TYPE 0
#define MENU_ITEM_ADDR 1
#define MENU_ITEM_MAX_INDEX 2
#define MENU_ITEM_EVNT 3

#define MAIN_MENU_ITEM  { MENU_ITEM_TYPE_SUBMENU, MAIN_MENU_IDX}

#define RESOLUTION_PARM_ITEM {MENU_ITEM_TYPE_PARM, BTN_PARM_RESOLUTION_IDX}
#define VOLUME_PARM_ITEM { MENU_ITEM_TYPE_PARM, BTN_PARM_SPEECH_VOL_IDX }
#define SYSTEM_SUBMENU_ITEM  {MENU_ITEM_TYPE_SUBMENU, SYSTEM_MENU_IDX}
#define CNFG_STATUS_RPT_PARM_ITEM  { MENU_ITEM_TYPE_REPORT, CNFG_STATUS_RPT_PARM_ITEM_IDX }

#define  SOURCE_PARM_ITEM { MENU_ITEM_TYPE_PARM, BTN_PARM_SOURCE_IDX}
#define  PORT_OPTIONS_ITEM { MENU_ITEM_TYPE_SUBMENU, PORT_OPTIONS_MENU_IDX }
#define  FMT_MOD_SUBMENU_ITEM { MENU_ITEM_TYPE_UNAVAILABLE, FMT_MOD_MENU_IDX }
#define  GPS_MOD_SUBMENU_ITEM { MENU_ITEM_TYPE_UNAVAILABLE, GPS_MOD_MENU_IDX }
#define  PER_RPT_SUBMENU_ITEM { MENU_ITEM_TYPE_SUBMENU, PER_RPT_MENU_IDX }
#define  AUD_CPS_SUBMENU_ITEM { MENU_ITEM_TYPE_SUBMENU, AUD_CPS_MENU_IDX }

#define  GPS_ON_PARM_ITEM {MENU_ITEM_TYPE_UNAVAILABLE, BTN_PARM_GPS_ON_IDX}
#define  GPS_NAV_ON_PARM_ITEM {MENU_ITEM_TYPE_PARM, BTN_PARM_GPS_NAV_ON_IDX}
//menu_item const gps_wpt_parm_item = {MENU_ITEM_TYPE_PARM, BTN_STT_GPS_WPT_IDX, MAX_GPS_WPT_IDX, EVNT_GPS_WPT_CHG};

#define  FMT_ON_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_FMT_ON_IDX}
#define  FMT_FREQ_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_FMT_FREQ_IDX}

#define  XNG_LOOPA_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_XNG_LOOP_A_IDX}
#define  XNG_LOOPB_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_XNG_LOOP_B_IDX}
#define  XNG_LOOP_USB_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_XNG_LOOP_USB_IDX}
#define  XNG_A2B_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_XNG_A2B_IDX}
#define  XNG_B2A_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_XNG_B2A_IDX}
#define  XNG_U2A_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_XNG_U2A_IDX}
#define  XNG_A2U_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_XNG_A2U_IDX}
#define  XNG_B2U_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_XNG_B2U_IDX}
#define  XNG_U2B_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_XNG_U2B_IDX}
#define  USART_A_SPEED_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_USART_A_SPEED_IDX}
#define  USART_B_SPEED_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_USART_B_SPEED_IDX}

#define  TGT_CNFG_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_TGT_CNFG_IDX}

#define  NAV_TO_WPT_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_NAV_TO_WPT_IDX}
#define  HDG_CAL_ADJ_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_HDG_CAL_ADJ_IDX}

#define CNFG_MIN_MAX_DELTA_MODE_ITEM  { MENU_ITEM_TYPE_PARM, BTN_PARM_CNFG_MIN_MAX_DELTA_MODE_IDX }

#define  AUD_CPS_ON_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_AUD_CPS_IDX}
#define  AUD_CPS_DELTA_ON_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_AUD_CPS_DELTA_TONES_IDX}
#define  AUD_CPS_DELTA_INTERVAL_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_AUD_CPS_DELTA_INTERVAL_IDX}
#define  AUD_CPS_MAX_PERIOD_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_AUD_CPS_MAX_PERIOD_IDX}
#define  AUD_CPS_MIN_PERIOD_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_AUD_CPS_MIN_PERIOD_IDX}
#define  AUD_CPS_RPT_TRUE_HEADING_PARM_ITEM {MENU_ITEM_TYPE_PARM, BTN_PARM_AUD_CPS_RPT_TRUE_HEADING_IDX}

#define  VOG_ON_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_PER_RPT_VOG_IDX}
#define  VOG_DELTA_ON_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_VOG_DELTA_TONES_IDX}
#define  VOG_DELTA_INTERVAL_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_VOG_DELTA_INTERVAL_IDX}
#define  VOG_MAX_PERIOD_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_VOG_MAX_PERIOD_IDX}
#define  VOG_MIN_PERIOD_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_VOG_MIN_PERIOD_IDX}

#define  STA_BTN_HDG_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_STA_BTN_HDG_IDX}
#define  STA_BTN_VOG_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_STA_BTN_VOG_IDX}
#define  STA_BTN_DTW_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_STA_BTN_DTW_IDX}
#define  STA_BTN_BTW_PARM_ITEM  {MENU_ITEM_TYPE_PARM, BTN_PARM_STA_BTN_BTW_IDX}

#endif

enum menu_item_indices {
    MAIN_MENU_ITEM_IDX,
	RESOLUTION_PARM_ITEM_IDX,
	VOLUME_PARM_ITEM_IDX,				
	SYSTEM_SUBMENU_ITEM_IDX,				
	HDG_SRC_PARM_ITEM_IDX,				
	PORT_OPTIONS_ITEM_IDX,			
	FMT_MOD_SUBMENU_ITEM_IDX,		
#ifdef ONBOARD_GPS_MUDULE	
	GPS_MOD_SUBMENU_ITEM_IDX,			
	GPS_ON_PARM_ITEM_IDX,	
#endif
	FMT_ON_PARM_ITEM_IDX,				
	FMT_FREQ_PARM_ITEM_IDX,				
	XNG_LOOPA_PARM_ITEM_IDX,				
	XNG_LOOPB_PARM_ITEM_IDX,
    XNG_LOOP_USB_PARM_ITEM_IDX,
	XNG_A2B_PARM_ITEM_IDX,				
	XNG_B2A_PARM_ITEM_IDX,
    XNG_U2A_PARM_ITEM_IDX,
    XNG_A2U_PARM_ITEM_IDX,				
	XNG_B2U_PARM_ITEM_IDX,  
    XNG_U2B_PARM_ITEM_IDX,
	CNFG_STATUS_RPT_PARM_ITEM_IDX,		
	PER_RPT_SUBMENU_ITEM_IDX,	
    CNFG_MIN_MAX_DELTA_MODE_ITEM_IDX,
    VOG_ON_PARM_ITEM_IDX,		
    VOG_DELTA_INTERVAL_PARM_ITEM_IDX,	
	VOG_MAX_PERIOD_PARM_ITEM_IDX,	
	VOG_MIN_PERIOD_PARM_ITEM_IDX,	
    AUD_CPS_SUBMENU_ITEM_IDX,			
	AUD_CPS_ON_PARM_ITEM_IDX,			
    AUD_CPS_DELTA_TONES_PARM_ITEM_IDX,		
    AUD_CPS_DELTA_INTERVAL_PARM_ITEM_IDX,	
	AUD_CPS_MAX_PERIOD_PARM_ITEM_IDX,	
	AUD_CPS_MIN_PERIOD_PARM_ITEM_IDX,	
    AUD_CPS_RPT_TRUE_HEADING_PARM_ITEM_IDX, 
    USART_A_SPEED_PARM_ITEM_IDX, 
    USART_B_SPEED_PARM_ITEM_IDX, 
    TGT_CNFG_PARM_ITEM_IDX, 
    NAV_TO_WPT_PARM_ITEM_IDX, 
    HDG_CAL_ADJ_PARM_ITEM_IDX,
    MENU_ITEM_CNT
};
#define MAX_MENU_ITEM_IDX MENU_ITEM_CNT-1
	
#ifdef BUTTON

menu_item menu_items[] = { 
	MAIN_MENU_ITEM,
	RESOLUTION_PARM_ITEM, 
	VOLUME_PARM_ITEM,
	SYSTEM_SUBMENU_ITEM,
	SOURCE_PARM_ITEM,
	PORT_OPTIONS_ITEM,
	FMT_MOD_SUBMENU_ITEM,
#ifdef ONBOARD_GPS_MUDULE
	GPS_MOD_SUBMENU_ITEM,
	GPS_ON_PARM_ITEM,
#endif
	FMT_ON_PARM_ITEM,
	FMT_FREQ_PARM_ITEM,
	XNG_LOOPA_PARM_ITEM,
	XNG_LOOPB_PARM_ITEM,
    XNG_LOOP_USB_PARM_ITEM,
	XNG_A2B_PARM_ITEM,
	XNG_B2A_PARM_ITEM,
    XNG_U2A_PARM_ITEM,
	XNG_A2U_PARM_ITEM,
	XNG_B2U_PARM_ITEM, 
    XNG_U2B_PARM_ITEM,
	CNFG_STATUS_RPT_PARM_ITEM,
	PER_RPT_SUBMENU_ITEM,
    CNFG_MIN_MAX_DELTA_MODE_ITEM,
	VOG_ON_PARM_ITEM,
	VOG_DELTA_INTERVAL_PARM_ITEM,
	VOG_MAX_PERIOD_PARM_ITEM,
	VOG_MIN_PERIOD_PARM_ITEM,
	AUD_CPS_SUBMENU_ITEM,
	AUD_CPS_ON_PARM_ITEM,
	AUD_CPS_DELTA_ON_PARM_ITEM,
	AUD_CPS_DELTA_INTERVAL_PARM_ITEM,
	AUD_CPS_MAX_PERIOD_PARM_ITEM,
	AUD_CPS_MIN_PERIOD_PARM_ITEM,
	AUD_CPS_RPT_TRUE_HEADING_PARM_ITEM,
	USART_A_SPEED_PARM_ITEM,
	USART_B_SPEED_PARM_ITEM,
	TGT_CNFG_PARM_ITEM,
	NAV_TO_WPT_PARM_ITEM,
	HDG_CAL_ADJ_PARM_ITEM
	};

int8 main_menu_items[] = 	{ AUD_CPS_SUBMENU_ITEM_IDX, SYSTEM_SUBMENU_ITEM_IDX , CNFG_STATUS_RPT_PARM_ITEM_IDX };

int8 system_menu_items[] = 	{ 
    HDG_SRC_PARM_ITEM_IDX, 
    TGT_CNFG_PARM_ITEM_IDX, 
#ifdef ONBOARD_GPS_MUDULE
    GPS_MOD_SUBMENU_ITEM_IDX,
#endif
    FMT_MOD_SUBMENU_ITEM_IDX, 
    RESOLUTION_PARM_ITEM_IDX, 
    PORT_OPTIONS_ITEM_IDX, 
    HDG_CAL_ADJ_PARM_ITEM_IDX };

#ifdef ONBOARD_GPS_MODULE
int8 gps_menu_items[] = 	{ GPS_ON_PARM_ITEM_IDX, VOG_ON_PARM_ITEM_IDX,NAV_TO_WPT_PARM_ITEM_IDX  };
#endif
int8 fmt_menu_items[] = 	{ FMT_ON_PARM_ITEM_IDX, FMT_FREQ_PARM_ITEM_IDX };
int8 port_options_menu_items[] = 	{ USART_A_SPEED_PARM_ITEM_IDX, USART_B_SPEED_PARM_ITEM_IDX, XNG_LOOPA_PARM_ITEM_IDX, XNG_LOOPB_PARM_ITEM_IDX, XNG_A2B_PARM_ITEM_IDX, XNG_B2A_PARM_ITEM_IDX };
int8 per_rpt_menu_items[] = { AUD_CPS_SUBMENU_ITEM_IDX, VOG_ON_PARM_ITEM_IDX };
int8 aud_cps_menu_items[] =	{ AUD_CPS_ON_PARM_ITEM_IDX, AUD_CPS_DELTA_TONES_PARM_ITEM_IDX, AUD_CPS_DELTA_INTERVAL_PARM_ITEM_IDX,
								AUD_CPS_MAX_PERIOD_PARM_ITEM_IDX, AUD_CPS_MIN_PERIOD_PARM_ITEM_IDX, AUD_CPS_RPT_TRUE_HEADING_PARM_ITEM_IDX  };
#define MAIN_MENU_MAX_INDEX sizeof(main_menu_items)
#define SYSTEM_MENU_MAX_INDEX sizeof(system_menu_items)
#define GPS_MENU_MAX_INDEX sizeof(gps_menu_items)
#define FMT_MENU_MAX_INDEX sizeof(fmt_menu_items)
#define PORT_OPTIONS_MENU_MAX_INDEX sizeof(port_options_menu_items)
#define PER_RPT_MENU_MAX_INDEX sizeof(per_rpt_menu_items)
#define AUD_CPS_MENU_MAX_INDEX sizeof(aud_cps_menu_items)

menu main_menu = 		{ MAIN_MENU_MAX_INDEX, 0, MAIN_MENU_ITEM_IDX, main_menu_items, &main_menu };
menu system_menu = 		{ SYSTEM_MENU_MAX_INDEX, 0, SYSTEM_SUBMENU_ITEM_IDX, &(system_menu_items[0]), &main_menu };
#ifdef ONBOARD_GPS_MODULE
menu gps_menu = 		{ GPS_MENU_MAX_INDEX, 0, GPS_MOD_SUBMENU_ITEM_IDX, &(gps_menu_items[0]), &system_menu };
#endif
menu fmt_menu = 		{ FMT_MENU_MAX_INDEX, 0, FMT_MOD_SUBMENU_ITEM_IDX, &(fmt_menu_items[0]), &system_menu };
menu port_options_menu = 			{ PORT_OPTIONS_MENU_MAX_INDEX, 0, PORT_OPTIONS_ITEM_IDX, &(port_options_menu_items[0]), &system_menu };
menu per_rpt_menu = 		{ PER_RPT_MENU_MAX_INDEX, 0, PER_RPT_SUBMENU_ITEM_IDX, &(per_rpt_menu_items[0]), &system_menu };
menu aud_cps_menu = 		{ AUD_CPS_MENU_MAX_INDEX, 0, AUD_CPS_SUBMENU_ITEM_IDX, &(aud_cps_menu_items[0]), &main_menu };


menu *menus[] = {
    &main_menu, 
    &system_menu, 
#ifdef ONBOARD_GPS_MODULE
    &gps_menu, 
#endif
    &fmt_menu, 
    &port_options_menu, 
    &per_rpt_menu, 
    &aud_cps_menu };

struct Btn_Ctl_struct {
	menu		*menu_ptr;
	int8		menu_item_idx;
	uint8		new_parm_val;
	int8		kb_row;
	uint16		keystate;		// momentary buttons are always 0, switches can be 0 or 1
	uint16		keys[3];
	uint16		keysPressed;
    uint16      keysPressedCnt;
	uint16		dialog_cnt;
	int8		lastkeypressed;
	int8		speech_muted;
	int8		min_max_delta_cnfg_state;
	uint8		target_mode;
	int16 		remote_btn_state;
    uint16      CodeErrs;
} Btn_Ctl = {&main_menu,MAIN_MENU_ITEM_IDX,0,0,0x0000,{0x0000, 0x0000,0x0000},0x0000,0x0000,0x0000, 0xff,0,1,0,0,0};

enum min_max_delta_min_max_delta_cnfg_state_indices {
    MIN_MAX_DELTA_COMPASS,
    MIN_MAX_DELTA_SPEED,
    MIN_MAX_DELTA_CNT
};
#endif


#ifdef BUTTON

#define UNPACKED_ADDR_IDX 0
#define MAX_VALUE_IDX 1
#define EVENT_IDX 2
#define MENU_ITEM_IDX 3

uint8 const _btn_parm_chg_profiles[ MAX_PARM_CHG_PROFILES ][ MAX_PARM_CHG_LEVELS ] = {
{ BTN_STT_VOG_DELTA_INTERVAL_IDX , MAX_VOG_DELTA_INTERVAL_IDX, EVNT_VOG_DELTA_INTERVAL_CHG,0xFF },
{ BTN_STT_VOG_MAX_INTERVAL_IDX , MAX_VOG_MAX_PERIOD_IDX, EVNT_VOG_MAX_PERIOD_CHG,0xFF },
{ BTN_STT_VOG_MIN_INTERVAL_IDX , MAX_VOG_MIN_PERIOD_IDX, EVNT_VOG_MIN_PERIOD_CHG,0xFF },
{ BTN_STT_CNFG_MIN_MAX_DELTA_MODE_ITEM_IDX , MAX_ON_OFF_IDX, EVNT_CNFG_MIN_MAX_DELTA_MODE_CHG,0xFF },
//{ BTN_STT_NONTARGET_BAR_DISP_MODE_IDX, MAX_NONTARGET_BAR_DISP_MODE_IDX, EVNT_NONTGTBARDSPLY_MODE_CHG,0xFF },
{ BTN_STT_SPEECH_VOL_IDX, MAX_SPEECH_VOL_IDX,EVNT_VOLUME_CHG, VOLUME_PARM_ITEM_IDX },
//{ BTN_STT_TONE_VOL_IDX, MAX_TONE_VOL_IDX, EVNT_TONE_VOL_CHG,0xFF },
{ BTN_STT_SOURCE_IDX, MAX_SRC_HDG, EVNT_SRC_CHG, HDG_SRC_PARM_ITEM_IDX },
{ BTN_STT_RESOLUTION_IDX,MAX_RESOLUTION_IDX, EVNT_RES_CHG, RESOLUTION_PARM_ITEM_IDX },
//{ BTN_STT_BUZZER_VOL_IDX,MAX_BUZZER_VOL_IDX, EVNT_BUZZER_VOL_CHG,0xFF },
{ BTN_STT_LVL_DGT_DISP_IDX, MAX_LVL_DGT_DISP_IDX,EVNT_DGT_DIM,0xFF },
{ BTN_STT_LVL_BAR_DISP_IDX, MAX_LVL_BAR_DISP_IDX, EVNT_BAR_DIM,0xFF },
//{ BTN_STT_TGT_BAR_DISP_IDX, MAX_TGT_BAR_DISP_IDX, EVNT_TGTBARDSPLY_MODE_CHG,0xFF },
//{ BTN_STT_CNFG_MENU_IDX, MAX_CNFG_MENU_IDX, EVNT_CNFG_MENU_CHG,0xFF },
{ BTN_STT_FMT_FREQ_IDX, 0xFF, EVNT_FMT_FREQ_CHG, FMT_FREQ_PARM_ITEM_IDX },
{ BTN_STT_FMT_ON_IDX, MAX_ON_OFF_IDX, EVNT_FMT_ON_CHG, FMT_ON_PARM_ITEM_IDX },
#ifdef ONBOARD_GPS_MODULE
{ BTN_STT_GPS_ON_IDX, MAX_ON_OFF_IDX, EVNT_GPS_ON_CHG, GPS_ON_PARM_ITEM_IDX },
{ BTN_STT_GPS_NAV_ON_IDX, MAX_ON_OFF_IDX, EVNT_GPS_NAV_ON_CHG, 0xFF },
#endif
{ BTN_STT_XNG_LOOP_A_IDX, MAX_ON_OFF_IDX, EVNT_XNG_LOOP_A_CHG, XNG_LOOPA_PARM_ITEM_IDX },
{ BTN_STT_XNG_LOOP_B_IDX, MAX_ON_OFF_IDX, EVNT_XNG_LOOP_B_CHG, XNG_LOOPB_PARM_ITEM_IDX },
{ BTN_STT_XNG_LOOP_USB_IDX, MAX_ON_OFF_IDX, EVNT_XNG_LOOP_USB_CHG, XNG_LOOP_USB_PARM_ITEM_IDX },
{ BTN_STT_XNG_A2B_IDX, MAX_ON_OFF_IDX, EVNT_XNG_A2B_CHG, XNG_A2B_PARM_ITEM_IDX },
{ BTN_STT_XNG_B2A_IDX, MAX_ON_OFF_IDX, EVNT_XNG_B2A_CHG, XNG_B2A_PARM_ITEM_IDX },
{ BTN_STT_XNG_U2A_IDX, MAX_ON_OFF_IDX, EVNT_XNG_U2A_CHG, XNG_U2A_PARM_ITEM_IDX },
{ BTN_STT_XNG_A2U_IDX, MAX_ON_OFF_IDX, EVNT_XNG_A2U_CHG, XNG_A2U_PARM_ITEM_IDX },
{ BTN_STT_XNG_B2U_IDX, MAX_ON_OFF_IDX, EVNT_XNG_B2U_CHG, XNG_B2U_PARM_ITEM_IDX },
{ BTN_STT_XNG_U2B_IDX, MAX_ON_OFF_IDX, EVNT_XNG_U2B_CHG, XNG_U2B_PARM_ITEM_IDX },
{ BTN_STT_AUD_CPS_ON_IDX, MAX_ON_OFF_IDX, EVNT_PER_HDG_RPT_CHG, AUD_CPS_ON_PARM_ITEM_IDX },
{ BTN_STT_AUD_CPS_DELTA_TONES_IDX, MAX_ON_OFF_IDX, EVNT_AUD_CPS_DELTA_TONES_CHG, AUD_CPS_DELTA_TONES_PARM_ITEM_IDX},
{ BTN_STT_AUD_CPS_DELTA_INTERVAL_IDX, 0x07, EVNT_AUD_CPS_DELTA_INTERVAL_CHG, AUD_CPS_DELTA_INTERVAL_PARM_ITEM_IDX},
{ BTN_STT_AUD_CPS_MAX_PERIOD_IDX, 0x07, EVNT_AUD_CPS_MAX_PERIOD_CHG, AUD_CPS_MAX_PERIOD_PARM_ITEM_IDX},
{ BTN_STT_PER_RPT_VOG_ON_IDX, MAX_ON_OFF_IDX, EVNT_PER_RPT_VOG_CHG, VOG_ON_PARM_ITEM_IDX },
{ BTN_STT_AUD_CPS_MIN_PERIOD_IDX, 0x07, EVNT_AUD_CPS_MIN_PERIOD_CHG, AUD_CPS_MIN_PERIOD_PARM_ITEM_IDX},
{ BTN_STT_AUD_CPS_RPT_TRUE_HEADING_IDX, MAX_ON_OFF_IDX, EVNT_AUD_CPS_RPT_TRUE_HEADING_CHG, AUD_CPS_RPT_TRUE_HEADING_PARM_ITEM_IDX},
{ BTN_STT_USART_A_SPEED_IDX, MAX_USART_SPEED_IDX, EVNT_USART_A_SPEED_CHG, USART_A_SPEED_PARM_ITEM_IDX},
{ BTN_STT_USART_B_SPEED_IDX, MAX_USART_SPEED_IDX, EVNT_USART_B_SPEED_CHG, USART_B_SPEED_PARM_ITEM_IDX},
{ BTN_STT_TGT_CNFG_IDX, MAX_TGT_CNFG_IDX, EVNT_TGT_CNFG_CHG, TGT_CNFG_PARM_ITEM_IDX},
{ BTN_STT_NAV_TO_WPT_IDX, MAX_NAV_TO_WPT_IDX, EVNT_NAV_TO_WPT_CHG, NAV_TO_WPT_PARM_ITEM_IDX},
{ BTN_STT_HDG_CAL_ADJ_IDX, 0xFF, EVNT_HDG_CAL_ADJ_CHG, HDG_CAL_ADJ_PARM_ITEM_IDX }
};


enum btn_level_speech_profiles {
 OFF_LOW_MED_HIGH_PROFILE,
 OFF_LOW_HIGH_PROFILE,
 LOW_MED_HIGH_PROFILE,
 COMPASS_GPS_AUTO_PROFILE,
 ZERO_ONE_TWO_THREE_PROFILE,
 OFF_ON_PROFILE,
 TRUE_MAGNETIC_PROFILE,
 USART_SPEED_PROFILE,
 TGT_CNFG_PROFILE,
 BTN_LVL_SPCH_PROFILE_CNT
};
//#define MAX_PROFILES sizeof(enum btn_level_speech_profiles)
#define MAX_PROFILES BTN_LVL_SPCH_PROFILE_CNT
#define MAX_LEVELS 4

uint8 const BtnLevelSpchProfiles[ MAX_PROFILES ][ MAX_LEVELS ] = {
{ TTS_ADDR_OFF_MID_INDEX, 
    TTS_ADDR_LOW_MID_INDEX, TTS_ADDR_MEDIUM_MID_INDEX, TTS_ADDR_HIGH_MID_INDEX },
{ TTS_ADDR_OFF_MID_INDEX, 
    TTS_ADDR_LOW_MID_INDEX, TTS_ADDR_HIGH_MID_INDEX, TTS_ADDR_ERROR_INDEX },
{ TTS_ADDR_LOW_MID_INDEX, 
	TTS_ADDR_MEDIUM_MID_INDEX, TTS_ADDR_HIGH_MID_INDEX, TTS_ADDR_ERROR_INDEX },
{ TTS_ADDR_COMPASS_HDM_MID_INDEX, 
	TTS_ADDR_COMPASS_HDG_MID_INDEX, TTS_ADDR_GPS_MID_INDEX, TTS_ADDR_AUTOMATIC_MID_INDEX },
{ TTS_ADDR_ZERO_INDEX, 
	TTS_ADDR_ONE_INDEX, TTS_ADDR_TWO_INDEX, TTS_ADDR_THREE_INDEX},
{ TTS_ADDR_OFF_MID_INDEX, 
	TTS_ADDR_ON_MID_INDEX, TTS_ADDR_ERROR_INDEX, TTS_ADDR_ERROR_INDEX },
{ TTS_ADDR_MAGNETIC_MID_INDEX, 
	TTS_ADDR_TRUE_MID_INDEX, TTS_ADDR_ERROR_INDEX, TTS_ADDR_ERROR_INDEX },	
{ TTS_ADDR_4800_MID_INDEX, 
	TTS_ADDR_9600_MID_INDEX, TTS_ADDR_19_2K_MID_INDEX, TTS_ADDR_38_4K_MID_INDEX },
{ TTS_ADDR_AUTOMATIC_MID_INDEX, 
	TTS_ADDR_HDG_MID_INDEX, TTS_ADDR_WPT_MID_INDEX, TTS_ADDR_ERROR_INDEX }
	};

#define BTN_SGNL_KEYPRESSED SGNL_GENERIC_MASK
#define _BTN_SGNL_KEYPRESSED (BTN_SGNL_KEYPRESSED^0xFF)
#define BTN_SGNL_CHECK_KEYS SGNL_TIMER_MASK
#define _BTN_SGNL_CHECK_KEYS (BTN_SGNL_CHECK_KEYS^0xFF)

////////////////////////////////////////////////////////////////////////////////////

#define NO_FLASH_CLEAR FALSE
#define FLASH_CLEAR TRUE

#endif // BUTTON

////////////////////////////////////////////////////////////////////////////////////
// Below supports storage of certain parameters which wouldn't be changing
// too often. FM radio CEX table, local magnetic deviation (DMV), NMEA msg options
// It uses the last 1024 bytes of flash so the processor configuration bits, 
// last eight bytes must be preserved. 
////////////////////////////////////////////////////////////////////////////////////
#define CNFG_FLASH_2_CEX_TBL_SIZE 12
#ifdef BUTTON

#define CNFG_FLASH_2_CEX_TBL_ADDR CNFG_FLASH_2_ADDR
#define CNFG_FLASH_2_DMV_SIZE 4
#define CNFG_FLASH_2_DMV_ADDR (CNFG_FLASH_2_ADDR+CNFG_FLASH_2_CEX_TBL_SIZE)
#define CNFG_FLASH_2_RS232_MSG_OPTIONS_SIZE 4*7
#define CNFG_FLASH_2_RS232_MSG_OPTIONS_ADDR (CNFG_FLASH_2_DMV_ADDR+CNFG_FLASH_2_DMV_SIZE)

#define CNFG_FLASH_2_CNFG_BITS_WORD_0_ADDR (CNFG_FLASH_2_ADDR+0x3FC)
#define CNFG_FLASH_2_CNFG_BITS_WORD_1_ADDR (CNFG_FLASH_2_ADDR+0x3FE)
#define CNFG_FLASH_2_CNFG_BITS_WORD_SIZE 4

#endif // BUTTON

enum cnfg_flash_2_profile_indices {
    CNFG_FLASH_2_CEX_TBL_PFL_IDX,
    CNFG_FLASH_2_DEFAULT_MAG_VAR_PFL_IDX,
    CNFG_FLASH_2_RS232_MSG_OPTIONS_PFL_IDX,
    CNFG_FLASH_2_CNFG_BITS_WORD_0_PFL_IDX,
    CNFG_FLASH_2_CNFG_BITS_WORD_1_PFL_IDX,
    MAX_CNFG_FLASH_2_PFL_IDX
};

#ifdef BUTTON

#define CNFG_FLASH_2_TBL_SIZE MAX_CNFG_FLASH_2_PFL_IDX

struct cnfg_flash_2_image_struct {
	uint8 cex_tbl_image[ CNFG_FLASH_2_CEX_TBL_SIZE ];
	uint8 dmv_image[ CNFG_FLASH_2_DMV_SIZE ];
    uint8 rs232_msg_options[CNFG_FLASH_2_RS232_MSG_OPTIONS_SIZE];
	uint8 flash_cnfg_word_0_image[ CNFG_FLASH_2_CNFG_BITS_WORD_SIZE ];
	uint8 flash_cnfg_word_1_image[ CNFG_FLASH_2_CNFG_BITS_WORD_SIZE ];
}cnfg_flash_2_image;

typedef struct _cnfg_flash_2_parm_struct {
	uint8 	*image_ptr;
	uint32	cnfg_flash_addr;
	uint8	cnfg_flash_size;
} _cnfg_flash_2_parm;	

_cnfg_flash_2_parm const _cnfg_flash_2_parm_chg_profiles[ CNFG_FLASH_2_TBL_SIZE ] = {
{ cnfg_flash_2_image.cex_tbl_image , CNFG_FLASH_2_CEX_TBL_ADDR, CNFG_FLASH_2_CEX_TBL_SIZE  },
{ cnfg_flash_2_image.dmv_image, CNFG_FLASH_2_DMV_ADDR, CNFG_FLASH_2_DMV_SIZE },
{ cnfg_flash_2_image.rs232_msg_options, CNFG_FLASH_2_RS232_MSG_OPTIONS_ADDR, CNFG_FLASH_2_RS232_MSG_OPTIONS_SIZE },
{ cnfg_flash_2_image.flash_cnfg_word_0_image, CNFG_FLASH_2_CNFG_BITS_WORD_0_ADDR, CNFG_FLASH_2_CNFG_BITS_WORD_SIZE},
{ cnfg_flash_2_image.flash_cnfg_word_1_image, CNFG_FLASH_2_CNFG_BITS_WORD_1_ADDR, CNFG_FLASH_2_CNFG_BITS_WORD_SIZE}
};

#endif // BUTTON

#endif
