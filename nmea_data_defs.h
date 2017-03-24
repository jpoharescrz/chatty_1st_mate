#ifndef NMEA_DATA_DEFS_H
#define NMEA_DATA_DEFS_H 1

enum minmax_idxs {
 ADSC_NO_MINMAX_PER_IDX,
 ADSC_MINMAX_PER_5_IDX,
 ADSC_MINMAX_PER_10_IDX,
 ADSC_MINMAX_PER_20_IDX,
 ADSC_MINMAX_PER_30_IDX,
 ADSC_MINMAX_PER_60_IDX,
 ADSC_MINMAX_PER_120_IDX,
 ADSC_MINMAX_PER_240_IDX
};

enum aud_cps_delta_interval_idxs {
 ADSC_NO_AUD_CPS_DELTA_INTERVAL_IDX,
 ADSC_AUD_CPS_DELTA_INTERVAL_1_IDX,
 ADSC_AUD_CPS_DELTA_INTERVAL_2_IDX,
 ADSC_AUD_CPS_DELTA_INTERVAL_5_IDX,
 ADSC_AUD_CPS_DELTA_INTERVAL_10_IDX,
 ADSC_AUD_CPS_DELTA_INTERVAL_20_IDX,
 ADSC_AUD_CPS_DELTA_INTERVAL_30_IDX,
 ADSC_AUD_CPS_DELTA_INTERVAL_50_IDX
};    

enum vog_delta_interval_idxs {
 ADSC_NO_VOG_DELTA_INTERVAL_IDX,
 ADSC_VOG_DELTA_INTERVAL_10TH_IDX,
 ADSC_VOG_DELTA_INTERVAL_4TH_IDX,
 ADSC_VOG_DELTA_INTERVAL_HALF_IDX,
 ADSC_VOG_DELTA_INTERVAL_1_IDX,
 ADSC_VOG_DELTA_INTERVAL_2_IDX,
 ADSC_VOG_DELTA_INTERVAL_5_IDX,
 ADSC_VOG_DELTA_INTERVAL_10_IDX
};  

enum adsc_max_period_indices {
    ADSC_MAX_PERIOD_DISABLE,
    ADSC_MAX_PERIOD_5_SECS,
    ADSC_MAX_PERIOD_10_SECS,
    ADSC_MAX_PERIOD_20_SECS,
    ADSC_MAX_PERIOD_30_SECS,
    ADSC_MAX_PERIOD_60_SECS,
    ADSC_MAX_PERIOD_120_SECS,
    ADSC_MAX_PERIOD_240_SECS,
};

#ifdef NMEA
const int16 aud_cps_max_period_idx_to_uint8[] = {0,5,10,20,30,60,120,240};
#define vog_max_period_idx_to_uint8 aud_cps_max_period_idx_to_uint8
#endif

enum adsc_min_period_indices {
    ADSC_MIN_PERIOD_3_SECS,
    ADSC_MIN_PERIOD_5_SECS,
    ADSC_MIN_PERIOD_10_SECS,
    ADSC_MIN_PERIOD_20_SECS,
    ADSC_MIN_PERIOD_30_SECS,
    ADSC_MIN_PERIOD_60_SECS,
    ADSC_MIN_PERIOD_120_SECS,
    ADSC_MIN_PERIOD_240_SECS,
};
#ifdef NMEA
const int16 aud_cps_min_period_idx_to_uint8[] = {3,5,10,20,30,60,120,240};
#define vog_min_period_idx_to_uint8 aud_cps_min_period_idx_to_uint8


const int16 aud_cps_delta_interval_idx_to_uint8[] = {0,1<<4,2<<4,5<<4,10<<4,20<<4,30<<4,50<<4};
char aud_cps_idx_to_uint8_str[5];

// Delta VOG values: None(0), .1, .25, .5, 1, 2, 5, 10
const int16 vog_delta_interval_idx_to_uint8[] = {0,1,4,8,1<<4,2<<4,5<<4,10<<4};
#define vog_idx_to_uint8_str aud_cps_idx_to_uint8_str
#endif

#ifdef NMEA_OR_NMEA_UTILS
#define NMEA_STATE_INITIAL 	0
#define NMEA_STATE_MSG_WD	1

typedef struct nmea_ctl_blk_str {
	int8 	last_fld_rec_idx_rptd;
	int8 	task_state;
	int8 	task_rpt_index;
	int8	mute_delta_tones;
	int16	default_mag_variation;	//fallback variation from flash
}nmea_ctl_blk;

#endif

#ifdef NMEA

nmea_ctl_blk nmea_ctl;

#endif 
 
#ifdef NMEA_UTILS
extern nmea_ctl_blk nmea_ctl;
#endif

#ifdef NMEA_OR_NMEA_UTILS

//#define NMEA_MSG_ENABLED        	0
//#define NMEA_DATA_MSG    1
//#define NMEA_MSG_PORT_SRC			2
//#define NMEA_MSG_ACTIVE         	8

typedef union {
    int16 cnfg_status_word;
    struct {
        int16 enabled:1;
        int16 is_data_msg:1;
        int16 src_port:2;
        int16 reserved0:4;
        int16 msg_active:1;
        int16 reserved1:7;
    }bit_fields;
}msg_cnfg_status_type;

typedef struct nmea_msg_str {
	
	int8                    msg_wd_cnt;      		// message watchdog counter
   	int8                    msg_rcvd_cnt;      		// messages rcvd this cycle
    msg_cnfg_status_type   	cnfg_status;            // configuration and status
   	int32                   expected_fields;    	// bit map of expected non-zero fields to get
   	int32                   active_fields;      	// bit map of active fields received
   	int32                   expected_fields_to_save;    // bit map of expected non-zero fields to save
   	int32                   msg_to_rec_map;     	// bit map to value recs for 
                                        // the above fields
}nmea_msg;

#endif

#ifdef NMEA

struct nmea_msgs_str {
	
   	int16      	msgs_active;      // fields of active messages
   	int16		msgs_expected;

   	nmea_msg   	msg_rec[ MAX_RS232_MSGS ];
   	
}NmeaMsgs;

#endif

#ifdef NMEA_UTILS
extern struct nmea_msgs_str {
   	int16      msgs_active;      // fields of active messages
   	int16       msgs_expected;
   	nmea_msg   msg_rec[ MAX_RS232_MSGS ];
}NmeaMsgs;
#endif

#define MAX_NMEA_REC_FIELD_LEN 10

typedef union {
    uint16 word16;
    struct {
        uint16 res0:1;
        uint16 rpt_on:1;
        uint16 rpt_abbv_on:1;
        uint16 rpt_delta_on:1;
        uint16 is_number:1;
        uint16 delta_tone:1;
        uint16 inter_delta_tone:1;
        uint16 src_port:2;
        uint16 is_active:1;
        uint16 res1:6;
    }bits;
}nmea_rec_cnfg_status_type;

#define NMEA_CNFG_RPT_ON_IDX 1
#define NMEA_CNFG_RPT_ABRV_ON_IDX 2
#define NMEA_CNFG_RPT_DELTA_ON_IDX 3
#define NMEA_CNFG_NUMBER_IDX 4
#define NMEA_CNFG_DELTA_TONE_IDX 5
#define NMEA_CNFG_INTER_DELTA_TONE_IDX 6
#define NMEA_CNFG_PORT_SOURCE 7
#define NMEA_STATUS_ACTIVE_IDX  9

#define NMEA_CNFG_RPT_ON_MASK (1 << NMEA_CNFG_RPT_ON_IDX )
#define NMEA_CNFG_RPT_ABRV_ON_MASK ( 1 << NMEA_CNFG_RPT_ABRV_ON_IDX )
#define NMEA_CNFG_RPT_DELTA_ON_MASK ( 1 << NMEA_CNFG_RPT_DELTA_ON_IDX )
#define NMEA_CNFG_NUMBER_MASK ( 1 << NMEA_CNFG_NUMBER_IDX )
#define NMEA_CNFG_DELTA_TONE_MASK ( 1 << NMEA_CNFG_DELTA_TONE_IDX )
#define NMEA_CNFG_INTER_DELTA_TONE_MASK ( 1 << NMEA_CNFG_INTER_DELTA_TONE_IDX )
#define NMEA_CNFG_PORT_SOURCE_MASK ( 1 << NMEA_CNFG_PORT_SOURCE_IDX )
#define NMEA_STATUS_ACTIVE_MASK ( 1 << NMEA_STATUS_ACTIVE_IDX )

typedef struct nmea_rec_str {
    char ascii_str[MAX_NMEA_REC_FIELD_LEN+1];
#ifdef NMEA_FLDREC_HIRES_VALUE    
    int32 value;
	int32 previous_value;
    int32 modulus;					// zero for infinity
    int32 last_reported_value; 		// last value reported
#else
    int16 value;
	int16 previous_value;
    int16 modulus;					// zero for infinity
    int16 last_reported_value; 		// last value reported
#endif    
    int8  last_rptd_dir;			// last reported delta direction
    int16 rpt_pre_post_indices;		// Speech indices; PreIndex(8 bits) : PostIndex(8 bits)
    int16 rpt_max_tmr;           	// Seconds left until next report
    int16 rpt_max_period;         	// Seconds between periodic reports
    int16 rpt_min_tmr;           	// Seconds left until next report
    int16 rpt_min_period;         	// Seconds between periodic reports
    int16 rpt_delta;          		// change required to trigger a report
    uint8 rpt_idt_tmr;				// inter delta timer
    uint8 rpt_idt_period;			// inter delta period
    uint8 rpt_idt_dly_tmr;			// inter delta delay timer
    int8 idt_idx;					// inter delta tone index
    uint8 rpt_recency_tmr;          // used to determine recency of last report
    uint8 min_pre_decimal_digits;	// minimum integer digits reported
    uint8 max_post_decimal_digits;	// maximum decimals reported
    uint16 report_counter;			//
    nmea_rec_cnfg_status_type cnfg_status;
} nmea_rec;

#ifdef NMEA
nmea_rec nmea_records[ MAX_NMEA_RECS ];
#endif

#ifdef NMEA_UTILS
extern nmea_rec nmea_records[ MAX_NMEA_RECS ];
#endif

#define NMEA_REC_NO_REPORT 0
enum nmea_report_opt_indicies {
    NMEA_REC_RPT,
    NMEA_REC_RPT_ABRV,
    NMEA_REC_RPT_DELTA,
    NMEA_REC_RPT_SAY_UNITS,
    NMEA_REC_RPT_DELTA_TONE_UP,
    NMEA_REC_RPT_DELTA_TONE_DN,
    NMEA_REC_RPT_DELTA_TONE_EQ,
    NMEA_REC_RPT_DELTA_ROUND_UP
};

#define NMEA_REC_RPT_MASK (1 << NMEA_REC_RPT)
#define NMEA_REC_RPT_ABRV_MASK (1 << NMEA_REC_RPT_ABRV)
#define NMEA_REC_RPT_DELTA_MASK (1 << NMEA_REC_RPT_DELTA)
#define NMEA_REC_RPT_SAY_UNITS_MASK (1 << NMEA_REC_RPT_SAY_UNITS)

#define NMEA_REC_RPT_DELTA_TONE_UP_MASK (1 << (NMEA_REC_RPT_DELTA_TONE_UP))
#define NMEA_REC_RPT_DELTA_TONE_DN_MASK (1 << (NMEA_REC_RPT_DELTA_TONE_DN))
#define NMEA_REC_RPT_DELTA_TONE_EQ_MASK (1 << (NMEA_REC_RPT_DELTA_TONE_EQ))
#define NMEA_REC_RPT_DELTA_TONE_MASK (NMEA_REC_RPT_DELTA_TONE_UP_MASK+NMEA_REC_RPT_DELTA_TONE_DN_MASK+NMEA_REC_RPT_DELTA_TONE_EQ_MASK)

#define NMEA_REC_RPT_DELTA_ROUND_UP_MASK (1 << (NMEA_REC_RPT_DELTA_ROUND_UP))

#define NMEA_DELTA_TONE_UP_THRESHOLD 12
#define NMEA_DELTA_TONE_DN_THRESHOLD NMEA_DELTA_TONE_UP_THRESHOLD*-1

// 
#define NMEA_MIN_CROSSBACK_AMT 8

#define NMEA_RISING_TONES (TTS_ADDR_DELTA_STBD_INDEX)
#define NMEA_FALLING_TONES (TTS_ADDR_DELTA_PORT_INDEX)
#define NMEA_STEADY_TONES (TTS_ADDR_HEADING_STEADY_INDEX)

//#define NMEA_RISING_TONES ((TTS_ADDR_NOTE_G4_INDEX << 8) | TTS_ADDR_NOTE_A4_INDEX)
//#define NMEA_FALLING_TONES ((TTS_ADDR_NOTE_B4_INDEX << 8) | TTS_ADDR_NOTE_A4_INDEX)

//#define NMEA_RISING_TONES ((TTS_ADDR_NOTE_BF4_2_INDEX << 8) | TTS_ADDR_NOTE_B4_2_INDEX)
//#define NMEA_FALLING_TONES ((TTS_ADDR_NOTE_C5_2_INDEX << 8) | TTS_ADDR_NOTE_B4_2_INDEX)

//#define NMEA_RISING_TONES 0
//#define NMEA_FALLING_TONES 0

#ifdef NMEA_OR_NMEA_UTILS

#define ACTIVATE_NMEA_REC(index) nmea_records[index].cnfg_status.bits.is_active=1;
#define DEACTIVATE_NMEA_REC(index) nmea_records[index].cnfg_status.bits.is_active=0;

#define NMEA_SET_FLDREC_RPT_DELTA(index, value) (nmea_records[ index ].rpt_delta = value)
#define NMEA_GET_FLDREC_RPT_DELTA(index) (nmea_records[ index ].rpt_delta)

#define NMEA_SET_FLDREC_RPT_MAX_TIMER(index, value) (nmea_records[ index ].rpt_max_tmr = value)
#define NMEA_GET_FLDREC_RPT_MAX_TIMER(index) (nmea_records[ index ].rpt_max_tmr)
#define NMEA_RST_FLDREC_RPT_MAX_TIMER(index) (nmea_records[ index ].rpt_max_tmr = nmea_records[ index ].rpt_max_period)

#define NMEA_SET_FLDREC_RPT_MIN_TIMER(index, value) (nmea_records[ index ].rpt_min_tmr = value)
#define NMEA_GET_FLDREC_RPT_MIN_TIMER(index) (nmea_records[ index ].rpt_min_tmr)
#define NMEA_RST_FLDREC_RPT_MIN_TIMER(index) (nmea_records[ index ].rpt_min_tmr = nmea_records[ index ].rpt_min_period)

#define NMEA_SET_FLDREC_RPT_IDT_TIMER(index, value) (nmea_records[ index ].rpt_idt_tmr = value)
#define NMEA_GET_FLDREC_RPT_IDT_TIMER(index) (nmea_records[ index ].rpt_idt_tmr)
#define NMEA_RST_FLDREC_RPT_IDT_TIMER(index) (nmea_records[ index ].rpt_idt_tmr = nmea_records[ index ].rpt_idt_period)

#define NMEA_SET_FLDREC_RPT_MIN_INTEGERS(index, value) (nmea_records[ index ].min_pre_decimal_digits = value)
#define NMEA_GET_FLDREC_RPT_MIN_INTEGERS(index) (nmea_records[ index ].min_pre_decimal_digits)

#define NMEA_SET_FLDREC_RPT_MAX_DECIMALS(index, value) (nmea_records[ index ].max_post_decimal_digits = value)
#define NMEA_GET_FLDREC_RPT_MAX_DECIMALS(index) (nmea_records[ index ].max_post_decimal_digits)

#define IS_NMEA_REC_ACTIVE( index) (nmea_records[ index ].cnfg_status.bits.is_active)
#define IS_NMEA_REC_NUMBER( index) (nmea_records[ index ].cnfg_status.bits.is_number)
#define IS_NMEA_REC_RPT_ON( index) (nmea_records[ index ].cnfg_status.bits.rpt_on)
#define ENABLE_NMEA_REC_RPT( index) (nmea_records[ index ].cnfg_status.bits.rpt_on=1)
#define DISABLE_NMEA_REC_RPT( index) (nmea_records[ index ].cnfg_status.bits.rpt_on=0)
#define IS_NMEA_DELTA_TONE_ON( index) (nmea_records[ index ].cnfg_status.bits.delta_tone)
#define IS_NMEA_INTER_DELTA_TONE_ON( index) (nmea_records[ index ].cnfg_status.bits.inter_delta_tone)

#define NMEA_SET_FLDREC_RPT_SPCH_INDICES(index, value) (nmea_records[ index ].rpt_pre_post_indices = value)
#define NMEA_GET_FLDREC_RPT_SPCH_INDICES(index) (nmea_records[ index ].rpt_pre_post_indices)

#define NMEA_SET_FLDREC_RPT_LASTVALUE(index, value) (nmea_records[ index ].last_reported_value = value)
#define NMEA_GET_FLDREC_RPT_LASTVALUE(index) (nmea_records[ index ].last_reported_value)

#define NMEA_SET_FLDREC_RPT_PERIOD(index, value) (nmea_records[ index ].rpt_max_period = value)
#define NMEA_GET_FLDREC_RPT_PERIOD(index) (nmea_records[ index ].rpt_max_period)

#define NMEA_EVENT_MSG_RCVD 0
#define NMEA_EVENT_MSG_WD_OK_TO_ERR 1
#define NMEA_EVENT_MSG_WD_ERR_TO_OK 2

#define NMEA_MSG_WD_OK 				0
#define NMEA_MSG_WD_OK_TO_ERR 		1
#define NMEA_MSG_WD_ERR 			2
#define NMEA_MSG_WD_ERR_TO_OK 		3

#define NMEA_MSG_ENABLED        	0
#define NMEA_DATA_MSG    1
#define NMEA_MSG_PORT_SRC_FLD			2
#define NMEA_MSG_ACTIVE         	8

//#define IDT_SMOOTH
//#define IDT
#undef IDT
#undef IDT_SMOOTH
#define USE_IDT_AS_YAW_INDICATOR

#define NMEA_MSG_ENABLED_MASK      	(1 << NMEA_MSG_ENABLED)
#define NMEA_DATA_MSG_MASK  (1 << NMEA_DATA_MSG )
#define NMEA_MSG_PORT_SRC_MASK		( 3 << NMEA_MSG_PORT_SRC_FLD )
#define _NMEA_MSG_PORT_SRC_MASK		( NMEA_MSG_PORT_SRC_MASK ^ 0xFFFF )
#define NMEA_MSG_ACTIVE_MASK      	(1 << NMEA_MSG_ACTIVE)

// 5 second watchdog
#define NMEA_MSG_WD_INIT 5

#define SGNL_NMEA_FLDREC_RPT_BIT	SGNL_GENERIC_BIT
#define SGNL_NMEA_FLDREC_RPT_MASK	SGNL_GENERIC_MASK

// 120 second reporting when commands missing
#define NMEA_CMD_RPT_INIT 120

#define NMEA_RPT_IDT_TMR_PERIOD 25
#define NMEA_RPT_IDT_DLY_TMR_PERIOD 12
#define NMEA_RPT_FAST_IDT_TMR_PERIOD 12

// After eight seconds we declare units after the number
#define NMEA_RPT_RECENCY_TMR_PERIOD 40

#define NMEA_MSG_SET_WD( index, ival )  NmeaMsgs.msg_rec[ index ].msg_wd_cnt = ival
#define NMEA_MSG_GET_WD( index )  NmeaMsgs.msg_rec[ index ].msg_wd_cnt  

#define NMEA_MSG_INC_MSG_RCVDCNT( index ) NmeaMsgs.msg_rec[ index ].msg_rcvd_cnt++
#define NMEA_MSG_GET_MSG_RCVDCNT( index )  NmeaMsgs.msg_rec[ index ].msg_rcvd_cnt 
#define NMEA_MSG_SET_MSG_RCVDCNT( index, ival )  NmeaMsgs.msg_rec[ index ].msg_rcvd_cnt = ival 

#define IS_NMEA_MSG_ACTIVE(i) NmeaMsgs.msg_rec[i].cnfg_status.bit_fields.msg_active
#define IS_NMEA_MSG_ENABLED(i) NmeaMsgs.msg_rec[i].cnfg_status.bit_fields.enabled
#define IS_NMEA_DATA_MSG(i) NmeaMsgs.msg_rec[i].cnfg_status.bit_fields.is_data_msg
#define NMEA_MSG_PORT_SRC(i) ((NmeaMsgs.msg_rec[i].cnfg_status.cnfg_status_word & NMEA_MSG_PORT_SRC_MASK) >> NMEA_MSG_PORT_SRC_FLD)

#define SET_NMEA_MSG_ACTIVE(i) NmeaMsgs.msg_rec[i].cnfg_status.bit_fields.msg_active=1
#define CLR_NMEA_MSG_ACTIVE(i) NmeaMsgs.msg_rec[i].cnfg_status.bit_fields.msg_active=0
#define ENABLE_NMEA_MSG(i) NmeaMsgs.msg_rec[i].cnfg_status.bit_fields.enabled=1

#define NMEA_GET_MSGS_STATUSWORD NmeaMsgs.msgs_active
#define NMEA_GET_MSGS_EXPDWORD NmeaMsgs.msgs_expected 
#define NMEA_SET_MSGS_EXPDWORD( ival )  NmeaMsgs.msgs_expected = ival 

#define IS_NMEA_MSGS_ACTIVE(i) NmeaMsgs.msgs_active & (1 << i)
#define SET_NMEA_MSGS_ACTIVE(i) NmeaMsgs.msgs_active |= (1 << i)
#define CLR_NMEA_MSGS_ACTIVE(i) NmeaMsgs.msgs_active &= ((1 << i)^0xFFFF)
#define SET_NMEA_MSGS_EXPECTED(i) NmeaMsgs.msgs_expected |= (1 << i)
#define CLR_NMEA_MSGS_EXPECTED(i) NmeaMsgs.msgs_expected &= ((1 << i)^0xFFFF)
#define IS_NMEA_MSGS_EXPECTED(i) (NmeaMsgs.msgs_expected & (1 << i))

#define DISABLE_NMEA_MSG(i) NmeaMsgs.msg_rec[i].cnfg_status.bit_fields.enabled=0

#define NMEA_GET_MSG_EXPDFLDS( index ) NmeaMsgs.msg_rec[ index ].expected_fields
#define NMEA_SET_MSG_EXPDFLDS( index, ival ) NmeaMsgs.msg_rec[ index ].expected_fields = ival 

#define NMEA_GET_MSG_FLDSTOSAVE( index )  NmeaMsgs.msg_rec[ index ].expected_fields_to_save 
#define NMEA_SET_MSG_FLDSTOSAVE( index, ival )  NmeaMsgs.msg_rec[ index ].expected_fields_to_save = ival 

#define NMEA_GET_MSG_ACTIVEFLDS( index ) ( NmeaMsgs.msg_rec[ index ].active_fields )
#define NMEA_SET_MSG_ACTIVEFLDS( index, ival )  NmeaMsgs.msg_rec[ index ].active_fields = ival 

#define NMEA_SET_MSG_FLDREC_MAP( index, ival ) NmeaMsgs.msg_rec[ index ].msg_to_rec_map = ival
#define NMEA_GET_MSG_FLDREC_MAP(index) NmeaMsgs.msg_rec[index].msg_to_rec_map

void _nmea_set_msg_active_fld_bit( int16 index, int8 q );
int8 Nmea_GetMsgsStatusBit( int16 index );
void Nmea_SetMsgExpdFlds( int16 index, int32 ival);

int8 _nmea_is_msg_valid( int16 index );
void Nmea_DeactivateMsg( int16 MsgIndex );
void Nmea_ReportMsgStatus( int16 MsgIndex );
void Nmea_ReportAllMsgStatus();
int8 Nmea_UpdMsgWd( int16 index );
int8 Nmea_ProcMsgWd( int MsgIndex );
void Nmea_InitMsg( int index );
void Nmea_InitMsgs();
void Nmea_ReportCmd( int16 NmeaCmd, int8 PreIndex );
void Nmea_InitFldRec( uint16 rec_index );
void Nmea_InitFldRecs();
int8 Nmea_GetFldRecCnfgStatusBit( int16 index, int8 q );
uint16 _nmea_get_fld_rec_cnfg_status( int16 index);
uint16 _nmea_get_fld_rec_pre_index( int16 idx );
uint16 _nmea_get_fld_rec_post_index( int16 idx );
uint32 _nmea_append_rising_tones( uint32 idx_field );
uint32 _nmea_append_falling_tones( uint32 idx_field );
void _nmea_set_fld_rec_delta_interval( int16 idx, int16 val );
void _nmea_set_fld_rec_rpt_max_period( int16 idx, int16 val );
void _nmea_set_fld_rec_rpt_min_period( int16 idx, int16 val );
void _nmea_init_default_mag_variation();
void _nmea_set_fld_rec_post_index( int16 fld_rec_idx, uint16 post_index );


#endif


#endif

