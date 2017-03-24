#ifndef BUTTON_CNFG_H
#define BUTTON_CNFG_H 1

// Bit definitions for Btn_inputs and Btn_inputs0
#define     BTN_INPUTS_HDG_BIT          0
#define 	BTN_INPUTS_MUTE_BIT 		1
#define     BTN_INPUTS_CNFG_SAVE_BIT    2
#define     BTN_INPUTS_F0_BIT           3

#define   	BTN_INPUTS_TGT_DN_BIT   	4
#define   	BTN_INPUTS_TGT_TGL_BIT   	5
#define   	BTN_INPUTS_RES_UP_BIT   	6
#define   	BTN_INPUTS_ERR_BIT          7

#define   	BTN_INPUTS_PER_DN_BIT   	8
#define   	BTN_INPUTS_PER_UP_BIT   	9
#define   	BTN_INPUTS_RES_DN_BIT   	10
#define   	BTN_INPUTS_TGT_UP_BIT   	11


#define     BTN_INPUT_KEY_MASK          0x0FFF

#define		BTN_INPUTS_FACTORY_RESET_CPU_BIT	12
#define		BTN_INPUTS_RESET_CPU_BIT	13

#define		BTN_INPUTS_MAX_INDEX		13
#define		BTN_INPUTS_MAX_INDEX_MASK	0x000F

#define     BTN_INPUTS_HDG_MASK         make_bit_mask(BTN_INPUTS_HDG_BIT)
#define 	BTN_INPUTS_MUTE_MASK 		make_bit_mask(BTN_INPUTS_MUTE_BIT)
#define   	BTN_INPUTS_YAW_TONE_MASK   	make_bit_mask(BTN_INPUTS_MUTE_BIT)|BTN_INPUTS_F0_MASK
#define     BTN_INPUTS_F0_MASK          make_bit_mask(BTN_INPUTS_F0_BIT)
#define     BTN_INPUTS_CNFG_SAVE_MASK   make_bit_mask(BTN_INPUTS_CNFG_SAVE_BIT)

#define   	BTN_INPUTS_TGT_DN_MASK   	make_bit_mask(BTN_INPUTS_TGT_DN_BIT)
#define   	BTN_INPUTS_TGT_TGL_MASK   	make_bit_mask(BTN_INPUTS_TGT_TGL_BIT)
#define   	BTN_INPUTS_TGT_UP_MASK   	make_bit_mask(BTN_INPUTS_TGT_UP_BIT)
#define   	BTN_INPUTS_ERR_MASK         make_bit_mask(BTN_INPUTS_ERR_BIT)

#define   	BTN_INPUTS_MIN_PER_DN_MASK   	make_bit_mask(BTN_INPUTS_PER_DN_BIT)
#define   	BTN_INPUTS_MIN_PER_UP_MASK   	make_bit_mask(BTN_INPUTS_PER_UP_BIT)
#define   	BTN_INPUTS_MAX_PER_DN_MASK   	(make_bit_mask(BTN_INPUTS_PER_DN_BIT)|BTN_INPUTS_F0_MASK)
#define   	BTN_INPUTS_MAX_PER_UP_MASK   	(make_bit_mask(BTN_INPUTS_PER_UP_BIT)|BTN_INPUTS_F0_MASK)
#define   	BTN_INPUTS_DELTA_DN_MASK        make_bit_mask(BTN_INPUTS_RES_DN_BIT)
#define   	BTN_INPUTS_DELTA_UP_MASK        make_bit_mask(BTN_INPUTS_RES_UP_BIT)
#define     BTN_INPUTS_HDG_CAL_ADJ_DN_MASK  ((BTN_INPUTS_DELTA_DN_MASK)|BTN_INPUTS_F0_MASK)
#define     BTN_INPUTS_HDG_CAL_ADJ_UP_MASK  ((BTN_INPUTS_DELTA_UP_MASK)|BTN_INPUTS_F0_MASK)

#define     BTN_INPUTS_MIN_MAX_DELTA_CNFG_CHG_MASK (BTN_INPUTS_TGT_DN_MASK|BTN_INPUTS_F0_MASK)

#define     _BTN_INPUTS_F0_MASK         (make_bit_mask(BTN_INPUTS_F0_BIT)^0xFFFF)

#define		BTN_INPUTS_FACTORY_RESET_MASK   (BTN_INPUTS_F0_MASK|BTN_INPUTS_CNFG_SAVE_MASK)

#define		DLG_KEY_MASK (BTN_INPUTS_TGT_DN_MASK | BTN_INPUTS_TGT_TGL_MASK | BTN_INPUTS_TGT_UP_MASK | BTN_INPUTS_MIN_PER_DN_MASK | BTN_INPUTS_MIN_PER_UP_MASK | BTN_INPUTS_MAX_PER_DN_MASK | BTN_INPUTS_MAX_PER_UP_MASK | BTN_INPUTS_DELTA_DN_MASK | BTN_INPUTS_DELTA_UP_MASK | BTN_INPUTS_HDG_CAL_ADJ_DN_MASK | BTN_INPUTS_HDG_CAL_ADJ_UP_MASK)

// Bit definitions for led_outputs
#define 	LED_OUTP_TGT_BIT   			0

// Button i/o map defines
#define		BTN_IN_0					PORTBbits.RB0
#define		BTN_IN_1					PORTBbits.RB1
#define		BTN_IN_2					PORTBbits.RB2
#define		BTN_IN_3					PORTBbits.RB3
#define		BTN_ROW_0					PORTCbits.RC0
#define		BTN_ROW_1					PORTCbits.RC1
#define		BTN_ROW_2					PORTCbits.RC2
#define		BTN_ROW_3					PORTCbits.RC3

// Heading Cal Adjust defines
#define HCA_SCALE	5
#define HCA_ZERO_VALUE 127

//
//Button i/o masks and shifts
#define		PORTC_BTN_MASK				0x0L
#define		PORTC_BTN_SHIFT				0
#define		PORTB_BTN_MASK				0x000FL
#define		PORTB_BTN_SHIFT				(0)

#define 	PORTC_ROW_MASK				0x000F
//
//	Display LED i/o map:
#define		DISP_TGT_LED				PORTAbits.RA7

////////////////////////////////////////////////////////////////////////////
// Client Event Switch
////////////////////////////////////////////////////////////////////////////

enum btn_events {
 EVNT_TGT_UP,
 EVNT_TGT_DN,
 EVNT_TGT_ON,
 EVNT_TGT_OFF,
 EVNT_RES_CHG,
 EVNT_HEADING_RPT,
 EVNT_AUD_CPS_PERIOD_CHG,
 EVNT_AUD_CPS_RESOLUTION_CHG,
 EVNT_MUTE_CHG,
 EVNT_TGT_CNFG_CHG,
 EVNT_HDG_CAL_ADJ_CHG,
 EVNT_CNFG_MIN_MAX_DELTA_MODE_CHG,
 EVNT_VOG_DELTA_INTERVAL_CHG,
 EVNT_VOG_MAX_PERIOD_CHG,
 EVNT_VOG_MIN_PERIOD_CHG,
 EVNT_SRC_CHG,
 EVNT_PER_HDG_RPT_CHG,
 EVNT_AUD_CPS_DELTA_TONES_CHG,
 EVNT_PER_RPT_VOG_CHG,
 EVNT_AUD_CPS_DELTA_INTERVAL_CHG,
 EVNT_AUD_CPS_MAX_PERIOD_CHG,
 EVNT_AUD_CPS_MIN_PERIOD_CHG,
 EVNT_AUD_CPS_RPT_TRUE_HEADING_CHG,
 EVNT_VOLUME_CHG,
 EVNT_DGT_DIM,
 EVNT_FMT_FREQ_CHG,
 EVNT_FMT_ON_CHG,
 EVNT_XNG_LOOP_A_CHG,
 EVNT_XNG_LOOP_B_CHG,
 EVNT_XNG_LOOP_USB_CHG,
 EVNT_XNG_A2B_CHG,
 EVNT_XNG_B2A_CHG,
 EVNT_XNG_U2A_CHG,
 EVNT_XNG_A2U_CHG,
 EVNT_XNG_B2U_CHG,
 EVNT_XNG_U2B_CHG,
 EVNT_USART_A_SPEED_CHG,
 EVNT_USART_B_SPEED_CHG,
 EVNT_NAV_TO_WPT_CHG,
 EVNT_BAR_DIM,
 
 MAX_BTN_EVNT_IDX
};

// External prototypes
void Adsc_TgtUp();
void Adsc_TgtDn();
int8 Adsc_TgtOn();
void Adsc_TgtOff();
void Adsc_SourceChange();
void Adsc_ResChg();
void Adsc_report_status();
void ADSC_SetPeriodicHeadingReportChg( );
void ADSC_SetPerVogReportChg( );
void Adsc_StatusBtnRpt();
void Adsc_HdgCalAdj( uint8 value );
char *Adsc_Hca_index_to_ascii( signed int16 hdg_cal_adj_idx );
void Adsc_ResHotKeyDn();
void Adsc_ResHotKeyUp();
void cnfg_panel_led( int8 led, int8 state );
void Nmea_SetVogMaxPeriod( uint8 value );
void Nmea_SetVogMinPeriod( uint8 value );
void Nmea_SetVogDeltaInterval( uint8 value );
void Nmea_SetAudCpsMaxPeriod( uint8 value );
void Nmea_SetAudCpsMinPeriod( uint8 value );
void Nmea_SetAudCpsDeltaInterval( uint8 value );
void Nmea_SetAudCpsDeltaTones( uint8 value );
void Adsc_SetAudCpsDeltaIntervalChg();
void Nmea_UpdRptTrueHeading( uint8 value );
void RS232_UpdateSwitching();
void RS232_UpdatePortASpeed();
void RS232_UpdatePortBSpeed();

#define USER_TGTUP Adsc_TgtUp
#define USER_TGTDN Adsc_TgtDn
#define USER_TGTON Adsc_TgtOn
#define USER_TGTOFF Adsc_TgtOff
#define USER_SRCCHG Adsc_SourceChange
#define USER_RESCHG Adsc_ResChg
#define USER_FMT_FREQ_CHG FMT_SetFreq
#define USER_FMT_ON_CHG FMT_PowerOn
#define USER_GPS_ON_CHG GPS_on_off_chg
#define USER_STATUS_RPT Adsc_StatusBtnRpt
#define USER_AUD_CPS_CHG ADSC_SetPeriodicHeadingReportChg
#define ALT_FLDREC_UPDATE
#ifdef ALT_FLDREC_UPDATE
#define USER_AUD_CPS_DELTA_CHG Nmea_SetAudCpsDeltaTones
#else
#define USER_AUD_CPS_DELTA_CHG ADSC_SetPeriodicHeadingReportChg
#endif
#define USER_PER_RPT_VOG_CHG ADSC_SetPerVogReportChg
#define USER_AUD_CPS_DELTA_INTERVAL_CHG Adsc_SetAudCpsDeltaIntervalChg
#define USER_AUD_CPS_MAX_PERIOD_CHG Nmea_SetAudCpsMaxPeriod
#define USER_AUD_CPS_MIN_PERIOD_CHG Nmea_SetAudCpsMinPeriod
#define USER_VOG_DELTA_INTERVAL_CHG Nmea_SetVogDeltaInterval
#define USER_VOG_MAX_PERIOD_CHG Nmea_SetVogMaxPeriod
#define USER_VOG_MIN_PERIOD_CHG Nmea_SetVogMinPeriod
#define USER_AUD_CPS_RPT_TRUE_HEADING Nmea_UpdRptTrueHeading
#define USER_UPDATE_SWITCHING RS232_UpdateSwitching
#define USER_UPDATE_PORT_A_SPEED RS232_UpdatePortASpeed
#define USER_UPDATE_PORT_B_SPEED RS232_UpdatePortBSpeed
#define USER_HDG_CAL_ADJ_CHG Adsc_HdgCalAdj


#endif
