
//int16 SourceErrTmr;
//int16 TgtSource;

//int16 HdgSource;

//int16 heading;                       // heading in XXXY format
//int16 adjusted_heading;               // adjusted heading. adj_abs_hdg = heading + heading_adjust;
//int16 next_adjusted_heading;
//int16 heading_adjust;                // Set up hdg_adjust so that adj_tgt_hdg is (MAX_ABS_HDG_MODULO/2)
//int16 prev_adjusted_heading;          // absolute unadjusted heading, upper 12 bits:integer, lower 4 bits: 16ths

//signed int16 heading_change;

//int16 target_heading;                // unadjusted target heading/angle
//int16 adjusted_target_heading;        // = target_heading + heading_adjust

//int16 Adsc_SetHeadingInit;

//int16 Adsc_TaskState = 0;

typedef struct adsc_cblk_struct {
	
	int8	status;
	int8 	TaskState;
	
	int16	power_up_cnt;
	int16 	SourceErrTmr;
	int16 	TgtSource;
	int16 	TgtLedPulser;

	int16 	HdgSource;
	int16	HdgSrcMsgIndex;
	int16	HdgSrcBounceCnt;
	int16	HdgSourcePort;
	int16 	HdgSourceNmeaRecIdx;
    char    MsgHdr[ RS232_HDR_SIZE+1 ];

	int16 	heading;                       	// heading in XXXY format
	int16 	prev_heading;          			// absolute unadjusted heading, upper 12 bits:integer, lower 4 bits: 16ths

	uint32	hdg_src_timestamp;				// last msg timestamp in 10 ms units
	uint16	hdg_src_rate;					// heading src update rate (per 10 secs)
	int16	heading_rate_of_change;			// rate of heading change per second, upper 12 bits:integer, lower 4 bits: 16ths

	signed 	int16 heading_change;

	int16 	target_heading;                	// unadjusted target heading/angle

	int16 	HeadingInit;					// True indicates next heading message is first msg after NO_SOURCE state

}adsc_ctl_blk;

adsc_ctl_blk adsc_cblk;

#define ADSC_PERIOD_TO_RATE_TBL_MAX_INDEX 149
uint16 adsc_period_to_rate_lu_tbl[] = {
1600,1600,800,533,400,320,267,229,200,178,
160	,145,133,123,114,107,100,94,89,84,
80	,76,73,70,67,64,62,59,57,55,
53	,52	,50	,48	,47	,46	,44	,43	,42	,41	,
40	,39	,38	,37	,36	,36	,35	,34	,33	,33	,
32	,31	,31	,30	,30	,29	,29	,28	,28	,27	,
27	,26	,26	,25	,25	,25	,24	,24	,24	,23	,
23	,23	,22	,22	,22	,21	,21	,21	,21	,20	,
20	,20	,20	,19	,19	,19	,19	,18	,18	,18	,
18	,18	,17	,17	,17	,17	,17	,16	,16	,16	,
16	,16	,16	,16	,15	,15	,15	,15	,15	,15	,
15	,14	,14	,14	,14	,14	,14	,14	,14	,13	,
13	,13	,13	,13	,13	,13	,13	,13	,13	,12	,
12	,12	,12	,12	,12	,12	,12	,12	,12	,12	,
11	,11	,11	,11	,11	,11	,11	,11	,11	,11	};

