#ifndef GPS_DATA_DEFS_H
#define GPS_DATA_DEFS_H 1
/////////////////////////////////////////////////////////////////////////
////                          gps_data_defs.h                              ////
////   
/////////////////////////////////////////////////////////////////

#ifdef GPS

#define GPS_CN	CNEN2bits.CN25IE

// Internal States
#define GPS_STT_MODULE_CHK_0 0
#define GPS_STT_MODULE_RUNNING 1
#define GPS_STT_MODULE_ERROR 2

// Try 20 times to raise the module
#define GPS_MODCHK_CNT_INIT 20

typedef struct gps_ctl_struct {
	int8	ProcState;
	int8	On;
	int8	Status;
	int8	Attached;
	int8	AttachedCnt;
	int8	ModChkCnt;
	int8	ModMsgIndex;
	int8	nav_to_wpt;
	int16		BTW;
	int16		DTW;
	long double	lat;
	long double lon;
	int8		rmb_upd_state;
	
} GPS_Ctl;

GPS_Ctl GPS_CtlBlk;

typedef struct gps_mod_cmd_struct {
	char	*CmdNumberStr;
	char	*CmdArgStr;
} GPS_ModCmd;

#define MAX_GPS_MOD_CMDS 2

const char Cmd301NumStr[] = "301";
const char Cmd301ArgsStr[] = ",1";
const char Cmd314NumStr[] = "314";
const char Cmd314ArgsStr[] = ",0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0";

GPS_ModCmd GPS_ModCmds[] = {
	{(char *)Cmd301NumStr, (char *)Cmd301ArgsStr},
	{(char *)Cmd314NumStr, (char *)Cmd314ArgsStr}};
	
#endif

#define GPS_WPT_NAME_SIZE 7
#define GPS_WPT_BUF_SIZE 7
#define GPS_WPT_MAX	1

#ifdef GPS

typedef struct gps_wpt_struct {
	long double	lat;
	long double	lon;
	char	name[GPS_WPT_NAME_SIZE+1];
} gps_wpt_type;

gps_wpt_type gps_wpt[ GPS_WPT_BUF_SIZE ];

union {
   gps_wpt_type gps_wpt[ GPS_WPT_BUF_SIZE ];
   uint8 gps_wpt_array[ sizeof( gps_wpt_type ) * GPS_WPT_BUF_SIZE ];
} gps_unpacked_union;
	
#endif

#define PI 3.14159265
#define KmPerNm 1.85645
#define Radius 3438

// External defines
#define GPS_MODULE_NOT_PRESENT 0
#define GPS_MODULE_PRESENT 1
#define GPS_MODULE_RUNNING 2
#define GPS_MODULE_ERROR 3

// Prototypes
int8 GPS_Status();
int8 GPS_Attached();
char *GPS_GetStats( int8 Clear, char *buf );
int GPS_UsartAClientReady( );


#endif
