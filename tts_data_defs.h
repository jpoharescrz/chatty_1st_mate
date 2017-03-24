#ifndef TTS_DATA_DEFS_H
#define TTS_DATA_DEFS_H
/////////////////////////////////////////////////////////////////////////
////                          Tts_cmds_0.h                              ////
////                                                                 ////
// The RS232 & I2C configuration msgs for the TTS module are:
// !SAD,XXXX*, Set ISD Message Address, XXXX is the address
// !SPH,XXXX,YYYY,ZZZZ,...*, Set ISD Message Phrase, XXXX, YYYY, ZZZZ is the address
//		configured phrase may use up to 10 file addresses
// !REC*, STart Recording, use previously configured (SAD) address
// !STO*, StoP Recording/Play. 
// !PUP*, Power up the Isd4004
// !PDN*, Power down the Isd4004
// !RST*, Reset the Isd4004
// !VUP,[0-3],[1-32]*, Volume Up, 0-left spkr, 1-right spkr, 2-lo tone
//				3-hi tone, 2nd arg is the step count
// !VDN,[0-3],[1-32]*, Volume down, 0-left spkr, 1-right spkr, 2-lo tone
//				3-hi tone, 2nd arg is the step count
// !VOL,[0-3],[00-31]*, Sets the volume for the spec'd pot. 1st arg is the
//				pot #, 0-left spkr, 1-right spkr, 2-lo tone, 3-hi tone. 2nd
//				arg is volume setting.
// !TPD,[001-255]*, Sets the hi tone frequency period, lo tone frequency
//				period is 2x of hi tone. The period will be 
//				(arg1_value+1)*4*128*(1/(12 mHz)). So for an arg1_value=255,
//				the period would be 10.9 ms for a frequency of 91 hz. An Arg1_value
//				of 8 gives a period of 0.341 Ms for a frequency of 2929 Hz.
// !TEF,[0-4],[0-4],[0-255],[0-255],[0-255]*, Sets up the Tone Element (TE) function. 
//				(1st arg): TE 0-left spkr vol, 1-right spkr vol, 2-lo tone vol, 
//				3-hi tone vol, 4-tone frequency period
//				(2nd arg): TE Function, 0-None, 1-Sine, 2-Square, 3-Ramp_dn, 4 ramp_up
//				(3rd arg): function period. range 0-255 (* 10ms)
//				(4th arg): function amplitude. range 0-31 for types 0-3. Range 0-255 for type 4.
//				(5th arg): Duty Cycle for square function. Mute period for ramp_up and ramp_dn
//				range 0-255 (* 10ms).
// !VBL,[0-31],[0-31]*, Sets the speaker volume and balance.
//				(1st arg):	Volume
//				(2nd arg):	Balance
// 
// The testing msgs are:
// !PLA*, Start PLaying msg, use previously configured address, the code sends
//		a !PLC* msg (described below) when the played msg finishes
// !PHR*, Start Playing Phrase msg, use previously configured phrase, the code 
//      sends a !PLC* msg when played phrase finishes
//
// The response msgs are:
// !CMR*, Command Acknowledged, tells the PC that the last cmd was OK.
// !PLC*, msg play completed
// !PPC*, phrase play completed
// !RCC,XXXX*, msg record completed at address XXXX
// !CME,YYY*, Command Error, Specifies an error condition
// 		YYY: 000, no address specified
//
/////////////////////////////////////////////////////////////////////////
#define TTS_TE_LF_SPKR '0'
#define TTS_TE_RT_SPKR '1'
#define TTS_TE_LO_TONE '2'
#define TTS_TE_HI_TONE '3'
#define TTS_TE_TONE_GEN '4'
#define TTS_TE_VOICE_LVL '5'

#define TE_FUNC_NONE '0'
#define TE_FUNC_SINE '1'
#define TE_FUNC_SQUARE '2'
#define TE_FUNC_SLOPE_DN '3'
#define TE_FUNC_SLOPE_UP '4'
#define TE_FUNC_SHIFT_DN '5'
#define TE_FUNC_SHIFT_UP '6'
#define TE_FUNC_MAX	6

#define TTS_TONE_PERIOD	70

#define TTS_CMD_SAD	0
#define TTS_CMD_SPH 1
#define TTS_CMD_REC 2
#define TTS_CMD_STO 3
#define TTS_CMD_PLA 4
#define TTS_CMD_PHR 5
#define TTS_CMD_PUP 6
#define TTS_CMD_PDN 7
#define TTS_CMD_RST 8
#define TTS_CMD_VUP 9
#define TTS_CMD_VDN 10
#define TTS_CMD_VOL 11
#define TTS_CMD_TPD 12
#define TTS_CMD_TEF 13
#define TTS_CMD_VBL 14

#define MAX_TTS_CMDS 15
#define MAX_TTS_CMD_LEN 4
#ifdef TTS
const char TtsCmds[MAX_TTS_CMDS][MAX_TTS_CMD_LEN] ={
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
"VBL"};
#else
extern char TtsCmds[MAX_TTS_CMDS][MAX_TTS_CMD_LEN];

#endif

#define TTS_EVT_CMR_TKN 0
#define TTS_EVT_PLC_TKN 1
#define TTS_EVT_RCC_TKN 2
#define TTS_EVT_PPC_TKN 3

//Const Char IsdEvts[4][4] = {"CMR","PLC","RCC","PPC"};

//#include "Tts_CmdMap.h"

#ifdef TTS
const int8 TtsAddrIndex0_9[10] = {	
	TTS_ADDR_ZERO_INDEX,	// index to 'zero' msg
	TTS_ADDR_ONE_INDEX, 	// index to 'one' msg
	TTS_ADDR_TWO_INDEX, 	// index to 'two' msg
	TTS_ADDR_THREE_INDEX, 	// index to 'three' msg
	TTS_ADDR_FOUR_INDEX, 	// index to 'four' msg
	TTS_ADDR_FIVE_INDEX, 	// index to 'five' msg
	TTS_ADDR_SIX_INDEX, 	// index to 'six' msg
	TTS_ADDR_SEVEN_INDEX, 	// index to 'seven' msg
	TTS_ADDR_EIGHT_INDEX, 	// index to 'eight' msg
	TTS_ADDR_NINE_INDEX};   // index to 'nine' msg

const int8 TtsAddrIndex10_19[10] = {	
	TTS_ADDR_TEN_INDEX,   		// index to 'ten' msg
	TTS_ADDR_ELEVEN_INDEX,		// index to 'eleven' msg
	TTS_ADDR_TWELVE_INDEX,		// index to 'twelve' msg
	TTS_ADDR_THIRTEEN_INDEX,	// index to 'thirteen' msg
	TTS_ADDR_FOURTEEN_INDEX,	// index to 'fourteen' msg
	TTS_ADDR_FIFTEEN_INDEX,		// index to 'fifteen' msg
	TTS_ADDR_SIXTEEN_INDEX,		// index to 'sixteen' msg
	TTS_ADDR_SEVENTEEN_INDEX,	// index to 'seventeen' msg
	TTS_ADDR_EIGHTEEN_INDEX,	// index to 'eighteen' msg
	TTS_ADDR_NINETEEN_INDEX};	// index to 'nineteen' msg

const int8 TtsAddrIndex00_90[10] = {	
	TTS_ADDR_ZERO_INDEX,		// index to 'zero' msg
	TTS_ADDR_TEN_INDEX,   		// index to 'ten' msg
	TTS_ADDR_TWENTY_INDEX,		// index to 'twenty' msg
	TTS_ADDR_THIRTY_INDEX, 		// index to 'thirty' msg
	TTS_ADDR_FORTY_INDEX, 		// index to 'forty' msg
	TTS_ADDR_FIFTY_INDEX, 		// index to 'fifty' msg
	TTS_ADDR_SIXTY_INDEX, 		// index to 'sixty' msg
	TTS_ADDR_SEVENTY_INDEX, 	// index to 'seventy' msg
	TTS_ADDR_EIGHTY_INDEX, 		// index to 'eighty' msg
	TTS_ADDR_NINETY_INDEX}; 	// index to 'ninety' msg

const int8 TtsAddrIndex00_900[10] = {	
	TTS_ADDR_ZERO_INDEX,		// index to 'zero' msg
	TTS_ADDR_1HUNDRED_INDEX,   	// index to '1 hundred' msg
	TTS_ADDR_2HUNDRED_INDEX,	// index to '2 hundred' msg
	TTS_ADDR_3HUNDRED_INDEX, 	// index to '3 hundred' msg
	TTS_ADDR_4HUNDRED_INDEX, 	// index to '4 hundred' msg
	TTS_ADDR_5HUNDRED_INDEX, 	// index to '5 hundred' msg
	TTS_ADDR_6HUNDRED_INDEX, 	// index to '6 hundred' msg
	TTS_ADDR_7HUNDRED_INDEX, 	// index to '7 hundred' msg
	TTS_ADDR_8HUNDRED_INDEX, 	// index to '8 hundred' msg
	TTS_ADDR_9HUNDRED_INDEX}; 	// index to '9 hundred' msg

const int8 TtsAddrIndexA_Z[26] = {	
	TTS_ADDR_A_INDEX,	// index to 'a' msg
	TTS_ADDR_B_INDEX, 	// index to 'b' msg
	TTS_ADDR_C_INDEX, 	// index to 'c' msg
	TTS_ADDR_D_INDEX, 	// index to 'd' msg
	TTS_ADDR_E_INDEX, 	// index to 'e' msg
	TTS_ADDR_F_INDEX, 	// index to 'f' msg
	TTS_ADDR_G_INDEX, 	// index to 'g' msg
	TTS_ADDR_H_INDEX, 	// index to 'h' msg
	TTS_ADDR_I_INDEX, 	// index to 'i' msg
	TTS_ADDR_J_INDEX,	// index to 'j' msg
	TTS_ADDR_K_INDEX,	// index to 'k' msg
	TTS_ADDR_L_INDEX, 	// index to 'l' msg
	TTS_ADDR_M_INDEX, 	// index to 'm' msg
	TTS_ADDR_N_INDEX, 	// index to 'n' msg
	TTS_ADDR_O_INDEX, 	// index to 'o' msg
	TTS_ADDR_P_INDEX, 	// index to 'p' msg
	TTS_ADDR_Q_INDEX, 	// index to 'q' msg
	TTS_ADDR_R_INDEX, 	// index to 'r' msg
	TTS_ADDR_S_INDEX, 	// index to 's' msg
	TTS_ADDR_T_INDEX,	// index to 't' msg
	TTS_ADDR_U_INDEX,	// index to 'u' msg
	TTS_ADDR_V_INDEX, 	// index to 'v' msg
	TTS_ADDR_W_INDEX, 	// index to 'w' msg
	TTS_ADDR_X_INDEX, 	// index to 'x' msg
	TTS_ADDR_Y_INDEX, 	// index to 'y' msg
	TTS_ADDR_Z_INDEX 	// index to 'z' msg
};   // index to 'nine' msg
#else

extern int8 TtsAddrIndex0_9[10];
extern int8 TtsAddrIndex10_19[10];
extern int8 TtsAddrIndex00_90[10];
extern int8 TtsAddrIndex00_900[10];
extern int8 TtsAddrIndexA_Z[26];

#endif

#define TTS_TASK_WAIT 0
#define TTS_TASK_PROC_QUE 1
#define TTS_TASK_BLOCKED 2
#define TTS_TASK_WAIT_FOR_TRUNCATED_TIMER 3

#define TTS_CMD_BUFSIZE 126

#ifndef TTS_FORCE_HI_PRIORITY

#define LOW_PRIORITY_BUFFER 0
#define HIGH_PRIORITY_BUFFER 1
#define MAX_CMD_BUFFERS 2

#else

#define HIGH_PRIORITY_BUFFER 0
#define MAX_CMD_BUFFERS 1

#endif

#define TTS_EVT_CMR 0
#define TTS_EVT_PLC 1
#define TTS_EVT_PPC 2
#define TTS_EVT_RCC 3
#define TTS_EVT_CME 4
#define TTS_EVT_STP 5
#define TTS_EVT_PUP 6
#define TTS_EVT_UNKNOWN MAX_TTS_EVTS

#define TTS_EVT_ANY 0xFF
#define TTS_EVT_MATCHED TTS_EVT_ANY

#define NO_DELIMITER 0
#define APPEND_DELIMITER 1
#define PREPEND_DELIMITER 2

#define RANGE_0_9 0
#define RANGE_10_19 1
#define RANGE_20_90 2
#define RANGE_100_900 3

#define SPCH_VOL_OFF "0"
#define SPCH_VOL_LOW "4"
#define SPCH_VOL_MED "13"
#define SPCH_VOL_HIGH "36"

#define TTS_CMD_PRIORITY_HI	1
#define TTS_CMD_PRIORITY_LO 0

#define TTS_CMD_ADMIN_BIT 7
#define TTS_CMD_CLIP_BIT 6

#define TTS_CMD_ADMIN_MASK (1<<TTS_CMD_ADMIN_BIT)
#define TTS_CMD_CLIP_MASK (1<<TTS_CMD_CLIP_BIT)

#define TTS_CMD_MASK 0x3F

#define TTS_CNFG_SPEECH_VOL_OFF 0
#define TTS_CNFG_SPEECH_VOL_LO 1
#define TTS_CNFG_SPEECH_VOL_MED 2
#define TTS_CNFG_SPEECH_VOL_HI 3

#define TTS_CNFG_SPKR_VOL_OFF 0
#define TTS_CNFG_SPKR_VOL_LO 1
#define TTS_CNFG_SPKR_VOL_MED 2
#define TTS_CNFG_SPKR_VOL_HI 3

#define TTS_CNFG_SPKR_BAL_NOMINAL 8
#define TTS_CNFG_SPKR_BAL_LEFT 16
#define TTS_CNFG_SPKR_BAL_RIGHT 0

#define TTS_CNFG_TONE_VOL_OFF 0
#define TTS_CNFG_TONE_VOL_LO 1
#define TTS_CNFG_TONE_VOL_MED 2
#define TTS_CNFG_TONE_VOL_HI 3

#define TTS_CMD_PROTOCOL_START 0xFE
#define TTS_CMD_PROTOCOL_STOP 0xFF
#define TTS_CMD_PROTOCOL_DELIM ','

#define TTS_OPT_BLOCK_MASK 0x3F
#define TTS_OPT_FORCE_MASK 0x40
#define TTS_OPT_HIGH_PRIORITY_QUEUE_MASK 0x80
#define TTS_OPT_EXPL_DIGIT_MASK 0x100
#define TTS_OPT_CLIP_MASK 0x200
#define TTS_OPT_JAM_MASK 0x40

#define TTS_OPT_BLOCK_PERIOD_MASK 0x3F

#define TTS_POSTDELAY_10MS 0x11
#define TTS_POSTDELAY_20MS 0x12
#define TTS_POSTDELAY_30MS 0x13
#define TTS_POSTDELAY_40MS 0x14
#define TTS_POSTDELAY_50MS 0x15
#define TTS_POSTDELAY_60MS 0x16
#define TTS_POSTDELAY_70MS 0x17
#define TTS_POSTDELAY_80MS 0x18
#define TTS_POSTDELAY_90MS 0x19

#define TTS_POSTDELAY_100MS 0x21
#define TTS_POSTDELAY_200MS 0x22
#define TTS_POSTDELAY_300MS 0x23
#define TTS_POSTDELAY_400MS 0x24
#define TTS_POSTDELAY_500MS 0x25
#define TTS_POSTDELAY_600MS 0x26
#define TTS_POSTDELAY_700MS 0x27
#define TTS_POSTDELAY_800MS 0x28
#define TTS_POSTDELAY_900MS 0x29

#define TTS_POSTDELAY_1SEC 0x31
#define TTS_POSTDELAY_2SEC 0x32
#define TTS_POSTDELAY_3SEC 0x33
#define TTS_POSTDELAY_4SEC 0x34
#define TTS_POSTDELAY_5SEC 0x35
#define TTS_POSTDELAY_6SEC 0x36
#define TTS_POSTDELAY_7SEC 0x37
#define TTS_POSTDELAY_8SEC 0x38
#define TTS_POSTDELAY_9SEC 0x39

#define TTS_CMD_SET_SPEECH_VOLUME 0x80
#define TTS_CMD_BLOCK 0x81
#define TTS_CMD_SET_TONE_FUNCTION 0x82
#define TTS_CMD_SET_TONE_PERIOD 0x83
#define TTS_CMD_SET_TONE_VOLUME 0x84
#define TTS_CMD_SET_SPKR_VOLUME_BALANCE 0x85
#define TTS_CMD_BUILD_A_COMMAND 0x86
#define TTS_CMD_MUTE 0x87
#define TTS_SPCH_CMD_BUILD_A_PHRASE 1

#define TTS_SPCH_CMD_MAX 1
#define TTS_CONTROL_CMD_MAX 0x87

// 10 second timeout on all speech commands
#define TTS_EVT_TIMEOUT 10

// timeouts for block cmds
#define TTS_BLK_1MS_TIMOUT 1
#define TTS_BLK_10MS_TIMOUT 1
#define TTS_BLK_100MS_TIMOUT 2
#define TTS_BLK_1SEC_TIMOUT 20
#define TTS_BLK_10SEC_TIMOUT 200

#define TTS_BLOCK_1MS 0
#define TTS_BLOCK_10MS 1
#define TTS_BLOCK_100MS 2
#define TTS_BLOCK_1SEC 3
#define TTS_BLOCK_10SEC 4

#define TTS_HI_TONE_MASK	0x80
#define TTS_LO_TONE_MASK	0x00

#define TTS_RT_SPKR_VOL_MASK 0x80
#define TTS_LF_SPKR_VOL_MASK 0x00


#ifdef TTS
typedef struct {
 int8	AliasErrs;	
 int8	null1;							
 unsigned int8 CmdCnt;				//0x02
 unsigned int8 CmdByteCnt;			//0x03
 int CmdRdIdx;						//0x04
 int CmdWtIdx;						//0x05
 int8 CmdDiscardCnt;				//0x06
 int8 CmdBufferState;				//0x07
 int8 CmdInProc;					//0x08
 int8 CmdBufferPriority;			//0x09
 int8 CmdWaitingForEvent;			//0x0A
 int8 CmdEventTimeout;				//0x0B
 int8 CmdIsClippable;				//0x0C
 int8 CmdErrsUnAcked;				//0x0D
 int8 CmdBuf[ TTS_CMD_BUFSIZE ]; 	//0x0E
} CmdBuffer;

struct tts_control_str {
 CmdBuffer CmdBuffers[ MAX_CMD_BUFFERS ];
 uint8 TtsBufferPlayingMsg;
 int8 TtsSpeechVolume; 
 int8 TtsHiToneVolume;
 int8 TtsLoToneVolume;
 int8 TtsErrors_WD;
 int8 TtsErrors_CME;
 int8 TtsErrors_BF;
 int8 TtsErrors_BadEvents;
 int8 TtsErrors_BadCmds;
 int8 TtsErrors_CodeErrs;
 int8 TtsErrors_isd_BF;
 int8 muted;
 int16 mute_timeout;
} tts_control;
#endif

////////////////////////////////////////////////////////////////////////////
// TTS module events
//
// The response msgs are:
// !CMR*, Command Acknowledged (not used), tells the PC that the last cmd was OK.
// !PLC*, msg play completed
// !PPC*, phrase play completed
// !RCC,XXXX*, msg record completed at address XXXX
// !CME,YYY*, Command Error, Specifies an error condition
// 		YYY: 000, no address specified
/////////////////////////////////////////////////////////////////////////////
#define MAX_TTS_EVTS 7
#define TTS_TIMEOUT 10

#ifdef TTS

const char TTS_IO_TtsEvts[ MAX_TTS_EVTS ][5] = {
"CMR",
"PLC",
"PPC",
"RCC",
"CME",
"STP",
"PUP"
};

// Added to speech addresses before being sent to the TTS
int16 Tts_SpchSpeed;
#define TTS_SPCH_SPEED_LOW 0
#define TTS_SPCH_SPEED_HIGH 1

#else

extern char TTS_IO_TtsEvts[ MAX_TTS_EVTS ][5];

// Added to speech addresses before being sent to the TTS
extern int16 Tts_SpchSpeed;
#define TTS_SPCH_SPEED_LOW 0
#define TTS_SPCH_SPEED_HIGH 1

#endif

#define TTS_IO_RX_CMD_WAITING 0
#define TTS_IO_RX_CMD_RECEIVING TTS_IO_RX_CMD_WAITING+1

#define TTS_IO_TX_CMD_WAITING 0
#define TTS_IO_TX_CMD_RECEIVING TTS_IO_TX_CMD_WAITING+1

#define TTS_IO_CMD_WAITING 0
#define TTS_IO_CMD_RECEIVING TTS_IO_CMD_WAITING+1

typedef struct {
	uint8	TokenIdx;
	uint8	TokenCnt;
	int		MsgByteCnt;
} MsgTknRec;

typedef struct {
	uint8		TokenCnt;
	uint8		TokenCnt_BU;
	uint8		CurTokenCnt;
	uint8		TokenRdIdx;
	uint8		TokenWtIdx;
	uint8		*Tokens[ TTS_IO_MAX_TOKENS ];
	
	uint8 		MsgCnt;
	uint8		MsgTokenWtIdx;
	uint8		MsgTokenRdIdx;
	MsgTknRec	MsgTokens[ TTS_IO_MAX_CMDS ];
	
	uint8		BufByteCnt;
	uint8		BufByteCnt_BU;
	uint8		BufWtIdx;
	uint8		BufWtIdx_BU;
	uint8		BufRdIdx;
	uint8		Buf[ TTS_IO_BUF_SIZE ];
	
	uint8		InState;
	uint8		InByte;
	uint8		BadEvents;
	uint8		DroppedPkts;
	
	uint8		TgtTask;
	uint8		TgtTaskSignal;
	
} TTS_IO_CtrlBlkRec;

#ifdef TTS
TTS_IO_CtrlBlkRec TTS_IO_InCtrlBlk, TTS_IO_OutCtrlBlk;
#else
extern TTS_IO_CtrlBlkRec TTS_IO_InCtrlBlk, TTS_IO_OutCtrlBlk;
#endif

#endif
