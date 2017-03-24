#ifndef ISD_HW_DEFS_H
#define ISD_HW_DEFS_H
#include "error_defs.h"
/////////////////////////////////////////////////////////////////////////
////                          isd_defs.h                             ////
////                                                                 ////
//
/////////////////////////////////////////////////////////////////////////

#define 	SPI_OUT 	_RC7
#define 	SPI_IN 		_RC8
#define 	SPI_CLK 	_RC9
#define		SPI_SS 		_RA8
#define		SPI_RAC 	_RA10

// define some member aliases
#define function_quiet_period function_duty_cycle
#define function_quiet_period_cnt function_duty_cycle_cnt
#define position_shift_img function_amplitude
#define position_shift_sync scratch[0]
#define position_sync scratch[1]
#define function_period_sync scratch[2]

typedef struct {
	uint8 MinState;	// minimum state for new incoming command
	uint8 MinStateOptions;	// either EQ, GT, GTE, etc.
} MinStateRec;

#define ISD_CMD_RS232_RCV 0
#define ISD_CMD_I2C_RCV 1
#define ISD_CMD_ISD_RCV 2

#define ISD_EVT_POWERUP_DONE	16
#define ISD_EVT_PLAYMSG_DONE	17
#define ISD_EVT_PLAYPHRASE_DONE	18
#define ISD_EVT_OVERFLOW		19
#define ISD_EVT_BAD_EOM			20

#define SER_EVT_CMR_TKN 0
#define SER_EVT_PLC_TKN 1
#define SER_EVT_RCC_TKN 2
#define SER_EVT_PPC_TKN 3
#define SER_EVT_STP_TKN 4
#define SER_EVT_PUP_TKN 5
#define SER_EVT_DBG_TKN 6

////////////////////////////////////////////////////////////////////////////////
// ISD operation states
//*****************************************************************************
#define ISD_STT_RESET 0
// When not recording, playing a msg, or playing a phrase, we
// return to this state, ISD_STT_READY
#define ISD_STT_READY 1
#define ISD_STT_REC 2
#define ISD_STT_PLAYMSG 3
#define ISD_STT_PLAYPHRASE 4
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// IsdCmdMinState[ ] option definitions
//******************************************************************************
#define MIN_STT_OPT_EQ	0
#define MIN_STT_OPT_GT	1
#define MIN_STT_OPT_GTE	2
#define MIN_STT_OPT_LT	3
#define MIN_STT_OPT_LTE	4
#define MIN_STT_OPT_NE	5

#define ISD_MAX_ROW_ADDR 2400-1

#define MAX_ISD_CMD_MIN_STATE 21
#ifdef ISD
////////////////////////////////////////////////////////////////////////////////
const MinStateRec IsdCmdMinState[ MAX_ISD_CMD_MIN_STATE ] = {
    {ISD_STT_READY, MIN_STT_OPT_EQ}, // SAD Cmd requires ISD state equal to ISD_STT_READY //
	{ISD_STT_READY, MIN_STT_OPT_EQ}, // Min State for SPH Cmd //
    {ISD_STT_READY, MIN_STT_OPT_EQ}, // Min State for REC Cmd //
    {ISD_STT_READY, MIN_STT_OPT_GTE}, // Min State for STO Cmd //
    {ISD_STT_READY, MIN_STT_OPT_EQ}, // Min State for PLA Cmd //
    {ISD_STT_READY, MIN_STT_OPT_EQ}, // Min State for PHR Cmd //
    {ISD_STT_RESET, MIN_STT_OPT_EQ}, // Min State for PUP Cmd //
    {ISD_STT_READY, MIN_STT_OPT_GTE}, // Min State for PDN Cmd //
    {ISD_STT_RESET, MIN_STT_OPT_GTE}, // Min State for RST Cmd //
    {ISD_STT_READY, MIN_STT_OPT_EQ}, // Min State for VUP Cmd //
    {ISD_STT_READY, MIN_STT_OPT_EQ}, // Min State for VDN Cmd //
    {ISD_STT_RESET, MIN_STT_OPT_GTE}, // Min State for VOL Cmd //
    {ISD_STT_RESET, MIN_STT_OPT_EQ}, // Min State for Undefined Cmd //
    {ISD_STT_RESET, MIN_STT_OPT_EQ}, // Min State for Undefined Cmd //
    {ISD_STT_RESET, MIN_STT_OPT_EQ}, // Min State for Undefined Cmd //
    {ISD_STT_RESET, MIN_STT_OPT_EQ}, // Min State for Undefined Cmd //
    {ISD_STT_RESET, MIN_STT_OPT_EQ}, // Min State for Powerup Done Event //
    {ISD_STT_PLAYMSG, MIN_STT_OPT_EQ}, // Min State for PLay Msg Done Event //
    {ISD_STT_PLAYPHRASE, MIN_STT_OPT_EQ}, // Min State for Play Phrase Done Event //
    {ISD_STT_RESET, MIN_STT_OPT_GTE}, // Min State for Overflow Event //
	{ISD_STT_RESET, MIN_STT_OPT_GTE}  // Min State for Bad EOM Event //
    };
   
#else
extern MinStateRec IsdCmdMinState[ MAX_ISD_CMD_MIN_STATE ];
#endif

///////////////////////////////////////////////////////////////////////////////////
// ISD related error message defines
#define ISD_ERR_OVERFLOW 16
#define ISD_ERR_RACWD 17
#define ISD_ERR_BADRAC 18
#define ISD_ERR_BADSRC 19
#define ISD_ERR_BADEOM 20
#define ISD_ERR_NORSRC 21
#define ISD_ERR_BADSTT 22
#define ISD_ERR_BADCMD 23
#define ISD_ERR_BADADR 24
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// ISD SPI command definitions
#define SPI_POWERUP		0x20
#define SPI_POWERDN		0x10
#define SPI_SETPLAY 	0xE0
#define SPI_SETREC 		0xA0
#define SPI_STOPPLAY 	0x70
#define SPI_STOPREC		0x30
#define SPI_RINT		0x30
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// IsdControl structure definitions
#define SER_MAX_PHRASE_MSGS MAX_RX_TOKENS-4
#define SER_RACWD_INIT 21
#define MAX_PHRASES 5
#define MAX_PHRASE_INDEX MAX_PHRASES-1

typedef struct {
	int16 IsdPhrase[ SER_MAX_PHRASE_MSGS ];
	int8 IsdPhraseIdx;
	int8 IsdPhraseCnt;
} PhraseCtlType;

typedef struct {
	int16 IsdAddress;
	int8 IsdState;
	int8 IsdSerCmd;
    int8 IsdTmrEvt;
	int8 IsdIntEvt;
	int8 IsdCmdSrc; 
	int8 IsdSerChannel;
	uint8 IsdSerPort;
	int8 IsdSerRxTokenCnt;
	int8 IsdRacWd;
	int8 IsdLastRac;
	int8 IsdMsgTimeout;
	int8 IsdRacTimeout;
	int8 IsdBadAddress;
	int8 IsdErrMsgTimeout;
    int8 IsdEvtCollision;
    int8 IsdSerCmdBadStt;
    int8 IsdBadSignals;
	int8 IsdBadRacs;
    int8 IsdBadMsgs;
	unsigned int16 IsdSpiAddress;
	unsigned int8 IsdSpiStatus;
    int8 CurrentPhraseIndex;
	PhraseCtlType PhraseCtl[MAX_PHRASES];
} IsdControlType;

#ifdef ISD
IsdControlType IsdControl;
#else
extern IsdControlType IsdControl;
#endif

#define SPI_STATUS_OVF ((IsdControl.IsdSpiStatus) & 0x02)
#define SPI_STATUS_EOM ((IsdControl.IsdSpiStatus) & 0x01)

typedef struct {
	int8 TraceBits;
	int8 TracePort;	// USARTA or USARTB
} IsdTraceCtl;

#ifdef ISD
IsdTraceCtl IsdTrace;

#else
extern IsdTraceCtl IsdTrace;
extern int8	Fm_XmtrAttached;
#endif

/////////////////////////////////////////////////////////////////////////
////                          isd_cmds.h                              ////
////                                                                 ////
// The RS232 & I2C configuration msgs for the TTS module are:
// !arg0,arg1,arg2...*
// Every message starts with a '!' character and ends with a '*'
// character.
// arg0 is always a single character digit specifying the channel 
// number.
// arg1 is always a three chracter command.
// arg2,arg3,etc. are command specific options]
// 
// ![0-9],SAD,XXXX*, Set ISD Message Address, XXXX is the address
// ![0-9],SPH,XXXX,YYYY,ZZZZ,...*, Set ISD Message Phrase, XXXX, YYYY, ZZZZ is the address
//		configured phrase may use up to 10 file addresses
// ![0-9],REC* or ![0-9],REC,XXXX*, STart Recording, use previously configured (SAD) address if no arg2 or use
//		address spec'd in arg2.
// ![0-9],PLA* or ![0-9],PLA,XXXX*, Start PLaying msg, use previously configued address if no arg2 or use
//		address spec'd in arg2. The code sends
//		a ![0-9],PLC* msg (described below) when the played msg finishes
// ![0-9],STO*, StoP Recording/Play. 
// ![0-9],PUP*, Power up the Isd4004
// ![0-9],PDN*, Power down the Isd4004
// ![0-9],RST*, Reset the Isd4004
// ![0-9],VUP,[0-3],[1-32]*, Volume Up, 0-left spkr, 1-right spkr, 2-lo tone
//				3-hi tone, 2nd arg is the step count
// ![0-9],VDN,[0-3],[1-32]*, Volume down, 0-left spkr, 1-right spkr, 2-lo tone
//				3-hi tone, 2nd arg is the step count
// ![0-9],VOL,[0-3],[00-31]*, Sets the volume for the spec'd pot. 1st arg is the
//				pot #, 0-left spkr, 1-right spkr, 2-lo tone, 3-hi tone. 2nd
//				arg is volume setting.
// ![0-9],TPD,[001-255]*, Sets the hi tone frequency period, lo tone frequency
//				period is 2x of hi tone. The period will be 
//				(arg1_value+1)*4*128*(1/(12 mHz)). So for an arg1_value=255,
//				the period would be 10.9 ms for a frequency of 91 hz. An Arg1_value
//				of 8 gives a period of 0.341 Ms for a frequency of 2929 Hz.
// ![0-9],TEF,[0-4],[0-4],[0-255],[0-255],[0-255]*, Sets up the Tone Element (TE) function. 
//				(1st arg): TE 0-left spkr vol, 1-right spkr vol, 2-lo tone vol, 
//				3-hi tone vol, 4-tone frequency period
//				(2nd arg): TE Function, 0-None, 1-Sine, 2-Square, 3-Ramp_dn, 4 ramp_up
//				(3rd arg): function period. range 0-255 (* 10ms)
//				(4th arg): function amplitude. range 0-31 for types 0-3. Range 0-255 for type 4.
//				(5th arg): Duty Cycle for square function. Mute period for ramp_up and ramp_dn
//				range 0-255 (* 10ms).
// ![0-9],VBL,[0-31],[0-31]*, Sets the speaker volume and balance.
//				(1st arg):	Volume
//				(2nd arg):	Balance
// ![0-9],DBG,[0-31],[0-31]*, Debug functions.
//				(1st arg):	0 - I2C Monitor function. 2nd arg is non-zero to enable, zero to disable.
//				(2nd arg):	Value, 
// 
// The testing msgs are:
// ![0-9],PLA*, Start PLaying msg, use previously configued address, the code sends
//		a !PLC* msg (described below) when the played msg finishes
// ![0-9],PHR*, Start Playing Phrase msg, use previously configured phrase, the code 
//      sends a !PLC* msg when played phrase finishes
//
// The response msgs are:
// ![0-9],CMR*, Command Acknowledged, tells the PC that the last cmd was OK.
// ![0-9],PLC*, msg play completed
// ![0-9],PPC*, phrase play completed
// ![0-9],RCC,XXXX*, msg record completed at address XXXX
// ![0-9],CME,YYY*, Command Error, Specifies an error condition
// 		YYY: 000, no address specified
//
/////////////////////////////////////////////////////////////////////////

#ifndef ISD_CMDS_H
#define ISD_CMDS_H

#define ISD_CMD_SAD	0
#define ISD_CMD_SPH 1
#define ISD_CMD_REC 2
#define ISD_CMD_STO 3
#define ISD_CMD_PLA 4
#define ISD_CMD_PHR 5
#define ISD_CMD_PUP 6
#define ISD_CMD_PDN 7
#define ISD_CMD_RST 8
#define ISD_CMD_VUP 9
#define ISD_CMD_VDN 10
#define ISD_CMD_VOL 11
#define ISD_CMD_TPD 12
#define ISD_CMD_TEF 13
#define ISD_CMD_VBL 14
#define ISD_CMD_DBG 15
#define ISD_CMD_FMT 16

#define MAX_ISD_CMDS 17

#define ISD_CMD_LENGTH 4
#ifdef ISD
const char IsdCmds[MAX_ISD_CMDS][ISD_CMD_LENGTH] ={
"SAD",
"SPH",
"REC",
"STO",
"PLA",
"PHR",
"PUP",
"PDN",
"RST",
"VUP",
"VDN",
"VOL",
"TPD",
"TEF",
"VBL",
"DBG",
"FMT"};
#else
extern char IsdCmds[MAX_ISD_CMDS][ISD_CMD_LENGTH];
#endif

#define ISD_EVT_CMR_TKN 0
#define ISD_EVT_PLC_TKN 1
#define ISD_EVT_RCC_TKN 2
#define ISD_EVT_PPC_TKN 3
#define ISD_EVT_STP_TKN 4
#define ISD_EVT_PUP_TKN 5
#define ISD_EVT_DBG_TKN 6
#define ISD_EVT_CME_TKN 7

#define ISD_SER_EVT_LENGHTH 4
#ifdef ISD
const char SerEvts[8][ISD_SER_EVT_LENGHTH] = {"CMR","PLC","RCC","PPC","STP","PUP","DBG","CME"};

const char UsartHdr[7] = {"PMTTS,"};
#else
extern char SerEvts[8][ISD_SER_EVT_LENGHTH];
extern char UsartHdr[7];
#endif

#define ISD_CMD_SRC_RS232_A SGNL_USARTA_MASK
#define ISD_CMD_SRC_RS232_B SGNL_USARTB_MASK
#define ISD_CMD_SRC_RS232_USB SGNL_USART_USB_MASK
#define ISD_CMD_SRC_TTS SGNL_TTS_MASK

#endif
#endif


