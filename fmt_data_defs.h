#ifndef FMT_DATA_DEFS_H
#define FMT_DATA_DEFS_H 1
/////////////////////////////////////////////////////////////////////////
////                          fmt_data_defs.h                              ////
////   
/////////////////////////////////////////////////////////////////

#define FM_XMTR_ADDR 0xCC

#define FM_XMTR_REG0_INIT 0x20
#define FM_XMTR_REG1_INIT 0xBC
#define FM_XMTR_REG2_INIT 0x05
#define FM_XMTR_REG3_INIT 0x8A
#define FM_XMTR_REG4_INIT 0x2E
#define FM_XMTR_REG5_INIT 0x00
#define FM_XMTR_REG6_INIT 0x1E
#define FM_XMTR_REG7_INIT 0x00
#define FM_XMTR_REG8_INIT 0x1B
#define FM_XMTR_REG9_INIT 0x00
#define FM_XMTR_REG10_INIT 0x00
#define FM_XMTR_REGE_INIT 0x05

#define FMT_FREQ_LO_ADDR 3
#define FMT_FREQ_HI_ADDR 4

#define FMT_MIN_FREQ	10718
#define	FMT_MAX_FREQ	13220
#define FMT_1MHZ_STEP	(124 >> 2)
#define FMT_100KHZ_STEP	(13 >> 2)

#define FMT_STT_POWERUP				0
#define	FMT_STT_CEX_INIT			1
#define FMT_STT_CEX_1MHZ_STEP		2
#define FMT_STT_CEX_100KHZ_PreSTEP	3
#define FMT_STT_CEX_100KHZ_STEP		4
#define FMT_STT_CEX_100KHZ_PostSTEP	5
#define FMT_STT_RDY					6
#define FMT_STT_CMD					7
#define FMT_STT_ERR					8

#define FMT_TEB 	PORTCbits.RC5
#define FMT_TEB_POS		6
#define FMT_TEB_NEG		-12

#define FMT_STATUS_INITIALIZING 0
#define FMT_STATUS_ON_AIR 1
#define FMT_STATUS_OFF_AIR 2
#define FMT_STATUS_ERROR 3
#define FMT_STATUS_NOT_PRESENT 4 

#define FMT_CMD_MUTE	0
#define FMT_CMD_UNMUTE	1
#define	FMT_MAX_CMD		1

#ifdef FMT

typedef struct fmt_ctl_struct {
	int8	ProcState;
	uint16	Status;
	uint16	Attached;
	int8	Muted;
	int8	OnAir;
	uint16	PwrLvl;
	int8	Pup_cnt;
	int8	CnfgFlash;
	uint16	CEX;
	int8	Cmd;
	uint16	TEB_cnt;
	uint16	freq;
	uint16	CEX_tbl[4];
	
} FMT_Ctl;

FMT_Ctl FMT_CtlBlk;
	
#endif


#endif
