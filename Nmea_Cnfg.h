#ifndef NMEA_CNFG_H
#define NMEA_CNFG_H	

#define INITIAL_DMV (((13<<4)+3))
/////////////////////////////////////////////////////////////////////////////////////////            
//HDM/HDT - Heading - Magnetic/True
//Vessel heading in degrees with respect to magnetic north produced by any device or system producing magnetic heading.
//
//        1   2 3
//        |   | |
// $--HDM,x.x,M*hh<CR><LF>
//Field Number:
//
//1 - Heading Degrees, magnetic/true
//
//2 - M/T = magnetic/true
//
//3 - Checksum
//////////////////////////////////////////////////////////
enum xxhdm_sentence_field_indices {
    XXHDM_CMD_IDX,
    XXHDM_HDG_IDX,
    XXHDM_MAG_TRUE_IND_IDX
};
enum xxhdt_sentence_field_indices {
    XXHDT_CMD_IDX,
    XXHDT_HDG_IDX,
    XXHDT_MAG_TRUE_IND_IDX
};

// define the expected position of the magnetic heading in the HCHDM msg
#define XXHDM_HDG_IDX 1
#define HCHDM_EXPECTED_FIELD_MASK (1 << XXHDM_HDG_IDX)

// NMEA record instance defines
enum fldrec_xxhdm_indices {
 FLDREC_XXHDM_HDG_IDX,
 FLDREC_HCHDM_CNT
};

#define HCHDM_SAVED_FIELD_MASK (1 << XXHDM_HDG_IDX)
#define HCHDM_REC_SAVED_FIELD_MASK (1 << FLDREC_XXHDM_HDG_IDX)

//////////////////////////////////////////////////////////
//HDG - Heading - Deviation & Variation
//        1   2   3 4   5 6
//        |   |   | |   | |
// $--HDG,x.x,x.x,a,x.x,a*hh<CR><LF>
//Field Number:
//
// 1 - Magnetic Sensor heading in degrees
//
// 2 - Magnetic Deviation, degrees
//
// 3 - Magnetic Deviation direction, E = Easterly, W = Westerly
//
// 4 - Magnetic Variation degrees
//
// 5 - Magnetic Variation direction, E = Easterly, W = Westerly
//
// 6 - Checksum            
////////////////////////////////////////////////////////////////
enum xxhdg_sentence_field_indices {
    XXHDG_CMD_IDX,
    XXHDG_HDG_IDX,
    XXHDG_MAG_DEV_IDX,
    XXHDG_MAG_DEV_DIR_IDX,
    XXHDG_MAG_VAR_IDX,
    XXHDG_MAG_VAR_DIR_IDX
};
// NMEA record instance defines
enum fldrec_xxhdg_indices {
 FLDREC_XXHDG_MAG_VAR_IDX = FLDREC_HCHDM_CNT,
 FLDREC_HCHDG_CNT
};
// Piggy back magnetic heading info with HCHDG
// define the expected position of the magnetic heading in the HCHDG msg
#define HCHDG_HDG_IDX XXHDM_HDG_IDX 
#define HCHDG_EXPECTED_FIELD_MASK ((1 << XXHDG_HDG_IDX)+(1<<XXHDG_MAG_VAR_IDX))

// NMEA record instance defines
#define FLDREC_HCHDG_HDG_IDX FLDREC_XXHDM_HDG_IDX 
#define HCHDG_SAVED_FIELD_MASK ((1 << XXHDG_HDG_IDX)+(1<<XXHDG_MAG_VAR_IDX))
#define HCHDG_REC_SAVED_FIELD_MASK ((1 << FLDREC_XXHDM_HDG_IDX)+(1<<FLDREC_XXHDG_MAG_VAR_IDX))

// Piggy back magnetic heading info with HCHDG
// define the expected position of the magnetic heading in the HCHDG msg
#define HCHDT_HDG_IDX XXHDM_HDG_IDX 
#define HCHDT_EXPECTED_FIELD_MASK HCHDM_EXPECTED_FIELD_MASK

#define FLDREC_HCHDT_HDG_IDX FLDREC_XXHDM_HDG_IDX 
#define HCHDT_SAVED_FIELD_MASK (1 << XXHDM_HDG_IDX)
#define HCHDT_REC_SAVED_FIELD_MASK (1 << FLDREC_XXHDM_HDG_IDX)

/////////////////////////////////////////////////////////////////////////////////////////
//RMB - The recommended minimum navigation sentence is sent whenever a route or a goto is active. 
//      On some systems it is sent all of the time with null data. The Arrival alarm flag is 
//      similar to the arrival alarm inside the unit and can be decoded to drive an external 
//      alarm. Note the use of leading zeros in this message to preserve the character spacing. 
//      This is done, I believe, because some autopilots may depend on exact character spacing.
//
//  $GPRMB,A,0.66,L,003,004,4917.24,N,12309.57,W,001.3,052.5,000.5,V*20
//
//   where:
//           RMB          Recommended minimum navigation information
//           A            Data status A = OK, V = Void (warning)
//           0.66,L       Cross-track error (nautical miles, 9.99 max),
//                                steer Left to correct (or R = right)
//           003          Origin waypoint ID
//           004          Destination waypoint ID
//           4917.24,N    Destination waypoint latitude 49 deg. 17.24 min. N
//           12309.57,W   Destination waypoint longitude 123 deg. 09.57 min. W
//           001.3        Range to destination, nautical miles (999.9 max)
//           052.5        True bearing to destination
//           000.5        Velocity towards destination, knots
//           V            Arrival alarm  A = arrived, V = not arrived
//           *20          checksum
/////////////////////////////////////////////////////////////////////////////////////////
enum xxrmb_sentence_field_indices {
    GPRMB_CMD_IDX,
    GPRMB_STATUS_IDX,
    GPRMB_XTE_IDX,
    GPRMB_XTE_DIR_IDX,
    GPRMB_ORIGIN_ID_IDX,
    GPRMB_DESTINATION_ID_IDX,
    GPRMB_DEST_WPT_LAT,
    GPRMB_DEST_WPT_LAT_DIR,
    GPRMB_DEST_WPT_LON,
    GPRMB_DEST_WPT_LON_DIR,    
    GPRMB_DTW_IDX,
    GPRMB_BTW_IDX,
    GPRMB_VTW_IDX
};
// Create the exptected field mask for the GPRMB msg
#define GPRMB_EXPECTED_FIELD_MASK ((((((1 << GPRMB_STATUS_IDX) | (1<<GPRMB_XTE_IDX)) | (1<<GPRMB_XTE_DIR_IDX)) | (1<<GPRMB_DTW_IDX))  | (1<<GPRMB_BTW_IDX)) | (1<<GPRMB_VTW_IDX))

// NMEA record instance defines
enum fldrec_xxrmb_indices {
 FLDREC_GPRMB_XTE_IDX = FLDREC_HCHDG_CNT,
 FLDREC_GPRMB_DTW_IDX,
 FLDREC_GPRMB_BTW_IDX,
 FLDREC_GPRMB_VTW_IDX,
 FLDREC_GPRMB_CNT
};         

#define GPRMB_SAVED_FIELD_MASK ((((1<<GPRMB_XTE_IDX) | (1<<GPRMB_DTW_IDX)) | (1<<GPRMB_BTW_IDX)) | (1<<GPRMB_VTW_IDX))
// Create the NMEA record mask
#define GPRMB_REC_SAVED_FIELD_MASK (((((1<<FLDREC_GPRMB_XTE_IDX)) | (1<<FLDREC_GPRMB_DTW_IDX)) | (1<<FLDREC_GPRMB_BTW_IDX)) | (1<<FLDREC_GPRMB_VTW_IDX))

/////////////////////////////////////////////////////////////////////////////////////////
//RMC - NMEA has its own version of essential gps pvt (position, velocity, time) data. It is called RMC, The Recommended Minimum, which will look similar to:
//
//$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A
//
//Where:
//     RMC          Recommended Minimum sentence C
//     123519       Fix taken at 12:35:19 UTC
//     A            Status A=active or V=Void.
//     4807.038,N   Latitude 48 deg 07.038' N
//     01131.000,E  Longitude 11 deg 31.000' E
//     022.4        Speed over the ground in knots
//     084.4        Track angle in degrees True
//     230394       Date - 23rd of March 1994
//     003.1,W      Magnetic Variation
//     *6A          The checksum data, always begins with *
//
//Note that, as of the 2.3 release of NMEA, there is a new field in the RMC sentence at the end just prior to the checksum. For more information on this field see here. 
/////////////////////////////////////////////////////////////////////////////////////////
enum xxrmc_sentence_field_indices {
    GPRMC_CMD_IDX,
    GPRMC_FIX_IDX,
    GPRMC_STATUS_IDX,
    GPRMC_CURLAT_IDX,
    GPRMC_CURLAT_DIR_IDX,
    GPRMC_CURLON_IDX,
    GPRMC_CURLON_DIR_IDX,
    GPRMC_CURVOG_IDX,
    GPRMC_CURHDG_IDX,
    GPRMC_DATE_IDX,
    GPRMC_MHDGVAR_IDX,
    GPRMC_MHDGVARDIR_IDX
};    

// Default mag variation -14.1, SF Bay
#define FLDREC_GPRMC_MHDGVAR_DEFAULT -((14*16)+1)

// Create the expected field mask for GPRMC msg
//#define GPRMC_EXPECTED_FIELD_MASK (((((1 << GPRMC_STATUS_IDX) + (1<<GPRMC_MHDGVAR_IDX)) + (1<<GPRMC_MHDGVARDIR_IDX)) + (1<<GPRMC_CURHDG_IDX)) + (1<<GPRMC_CURVOG_IDX))
#define GPRMC_EXPECTED_FIELD_MASK (((1 << GPRMC_STATUS_IDX) + (1<<GPRMC_CURHDG_IDX)) + (1<<GPRMC_CURVOG_IDX))

// NMEA record instance defines
// Internal NMEA record instance defines
enum fldrec_xxrmc_indices {
 FLDREC_GPRMC_CURVOG_IDX = FLDREC_GPRMB_CNT,
 FLDREC_GPRMC_CURHDG_IDX,
 FLDREC_GPRMC_MHDGVAR_IDX,
 FLDREC_GPRMC_CNT
};         
#define MAX_NMEA_RECS FLDREC_GPRMC_CNT

#define GPRMC_SAVED_FIELD_MASK ((1<<GPRMC_MHDGVAR_IDX) | (1<<GPRMC_CURHDG_IDX) | (1<<GPRMC_CURVOG_IDX))

#define GPRMC_REC_SAVED_FIELD_MASK ((((1<<FLDREC_GPRMC_MHDGVAR_IDX)) | (1<<FLDREC_GPRMC_CURHDG_IDX)) | (1<<FLDREC_GPRMC_CURVOG_IDX))

/////////////////////////////////////////////////////////////////////////////////////////
// Test Message Definitions definitions
/////////////////////////////////////////////////////////////////////////////////////////
// Testing commands
// $PMTST,(PMTST_FUNC_BTN_SET=0),[button index]*  //pushes the spec'd button
//	#define   	BTN_INPUTS_TGT_DN_BIT   	0
//	#define   	BTN_INPUTS_TGT_UP_BIT   	1
//	#define   	BTN_INPUTS_TGT_TGL_BIT   	2
//	#define   	BTN_INPUTS_NAV_RT_BIT   	3
//	#define   	BTN_INPUTS_NAV_LF_BIT   	4
//	#define   	BTN_INPUTS_NAV_DN_BIT   	5
//	#define   	BTN_INPUTS_NAV_UP_BIT 		6
//	#define 	BTN_INPUTS_NAV_SEL_BIT 		7
//	#define 	BTN_INPUTS_MUTE_BIT 		8
//	#define		BTN_INPUTS_NAV_EXIT_BIT		9
//	#define		BTN_INPUTS_NAV_SAVE_BIT		10
//	#define		BTN_INPUTS_STATUS_RPT_BIT	11
//	#define		BTN_INPUTS_RESET_CPU_BIT	13
//
// $PMTST,(PMTST_FUNC_READ_DGT_DSPLY=1)*   //returns the DIGIT display as three ascii numbers
// $PMTST,(PMTST_FUNC_READ_BAR_DSPLY=2)*   //returns the bar display TBD
// $PMTST,(PMTST_FUNC_READ_DGT_DSPLY_LVL=3)*   // TBD
// $PMTST,(PMTST_FUNC_READ_BAR_DSPLY_LVL=4)*   // TBD
// $PMTST,(PMTST_FUNC_SET_PARM=5),[parm index],[value]*   // Sets the spec'd cnfg parm to spec'd value
// $PMTST,(PMTST_FUNC_GET_PARM=6),[parm index]*   // Returns the spec'd cnfg parm to spec'd value
// $PMTST,(PMTST_FUNC_SAVE_CNFG=7)*  // Saves the configuration
// $PMTST,(PMTST_FUNC_SET_TRACE_MASK=8),mask*
// $PMTST,(PMTST_FUNC_GET_STATS=9),(reports=0-3),(msg=0-3)*
////////////////////////////////////////////////////////////////////
enum pmtst_cmd_indices {
 PMTST_CMD_HDR,
 PMTST_FUNC_IDX,
 PMTST_ARG0_IDX,
 PMTST_ARG1_IDX,
 PMTST_ARG2_IDX,
 PMTST_ARG3_IDX,
 PMTST_ARG4_IDX,
 PMTST_ARG5_IDX,
 PMTST_ARG6_IDX
};

#define PMTST_FUNC_BTN_SET 0
#define PMTST_FUNC_READ_DGT_DSPLY 1
#define PMTST_FUNC_READ_BAR_DSPLY 2
#define PMTST_FUNC_READ_DGT_DSPLY_LVL 3
#define PMTST_FUNC_READ_BAR_DSPLY_LVL 4
#define PMTST_FUNC_SET_PARM 5
#define PMTST_FUNC_GET_PARM 6
#define PMTST_FUNC_SAVE_CNFG 7
#define PMTST_FUNC_SET_TRACE_MASK 8
#define PMTST_FUNC_GET_STATS 9
#define PMTST_FUNC_SET_FMT_FREQ 10
#define PMTST_FUNC_SET_USART_SWITCHING 11
#define PMTST_FUNC_SET_CNFG_FLASH_2_PARM 12
#define PMTST_FUNC_GET_CNFG_FLASH_PARM 13
#define PMTST_FUNC_SET_TTS_MUTE_PARM 14
#define PMTST_FUNC_WRITE_I2C_REGISTER 15
#define PMTST_FUNC_I2C_ADDR_CHK 16

#define PMTST_STATS_RS232_A 0
#define PMTST_STATS_RS232_B 1
#define PMTST_STATS_RS232_USB 2
#define PMTST_STATS_ISD 3
#define PMTST_STATS_TTS 4
#define PMTST_STATS_CORE 5
#define PMTST_STATS_FMT 6

/////////////////////////////////////////////////////////////////////////////////////////
// Automatic reporting definitions
/////////////////////////////////////////////////////////////////////////////////////////
#define REPORT_RATE 5
#define NMEA_FLDREC_UPD_RATE_INTG_PER REPORT_RATE*4

// the heading changes 2.5 degrees before reporting at hi-res
#define HDG_DELTA_RPT_AMT 0x28
// report the heading at least every 30 seconds
#define HDG_RPT_MAX_PERIOD 30*REPORT_RATE
// report the heading at most every 8 seconds
#define HDG_RPT_MIN_PERIOD 8*REPORT_RATE
// report at least 3 digits
#define HDG_RPT_MIN_INTEGERS 3
// report at most 0 decimals
#define HDG_RPT_MAX_DECIMALS 0


// the VOG delta announces at 8/16ths of a nm/hr
#define VOG_DELTA_RPT_AMT 8
#define VOG_RPT_MAX_PERIOD 60*REPORT_RATE
#define VOG_RPT_MIN_PERIOD 15*REPORT_RATE
#define VOG_RPT_MIN_INTEGERS 1
#define VOG_RPT_MAX_DECIMALS 1

#define XTE_DELTA_RPT_AMT 8
#define XTE_RPT_MAX_PERIOD 60*REPORT_RATE
#define XTE_RPT_MIN_PERIOD 15*REPORT_RATE
#define XTE_RPT_MIN_INTEGERS 1
#define XTE_RPT_MAX_DECIMALS 1

#define DTW_DELTA_RPT_AMT 8
#define DTW_RPT_MAX_PERIOD 60*REPORT_RATE
#define DTW_RPT_MIN_PERIOD 15*REPORT_RATE
#define DTW_RPT_MIN_INTEGERS 1
#define DTW_RPT_MAX_DECIMALS 1

#define BTW_DELTA_RPT_AMT 8
#define BTW_RPT_MAX_PERIOD 60*REPORT_RATE
#define BTW_RPT_MIN_PERIOD 15*REPORT_RATE
#define BTW_RPT_MIN_INTEGERS 1
#define BTW_RPT_MAX_DECIMALS 1

#define VTW_DELTA_RPT_AMT 8
#define VTW_RPT_MAX_PERIOD 60*REPORT_RATE
#define VTW_RPT_MIN_PERIOD 15*REPORT_RATE
#define VTW_RPT_MIN_INTEGERS 1
#define VTW_RPT_MAX_DECIMALS 1

// External prototypes
void Adsc_ProcNmeaMsg( int8 UsartIdx, int8 msg_index );
////////////////////////////////////////////////////////////////////////////
// Client Msg Event 
////////////////////////////////////////////////////////////////////////////
#define NMEA_CLIENT_MSG_EVENT_ROUTINE Adsc_ProcNmeaMsg

#ifdef NMEA
void Adsc_NmeaClientInit();
void Nmea_UserInit()
{
 Adsc_NmeaClientInit();
}
#endif

#endif
