#ifndef DISPLAY_DEFS_H
#define DISPLAY_DEFS_H

#define DISP_I2C_ADDR 0xC2

#define DISP_REG_FW_REV 0

#define DISP_STT_INITIALIZING 0
#define DISP_STT_READ_FW_REV 	1
#define DISP_STT_RUNNING 		2
#define DISP_STT_NOT_PRESENT 	3

typedef struct disp_ctl_struct {
	
	uint8 ProcState;
	uint8 attached;
	uint8 powerup_dly;
	uint8 reg_idx;
	uint16 fw_rev;
	uint16 heading;
	uint8 sweep:1;
	uint8 sweep_rt:1;
	uint8 sweep_rate:6;
	uint8 brightness:2;
	uint8 text_color:2;
	uint8 signal_active:1;
	uint8 targeting:1;
	uint8 empty_0:2;
	int16 target_offset;

} Disp_Ctl;

union {
	Disp_Ctl _disp_ctl_blk;
	uint8 registers[ sizeof( Disp_Ctl ) ];
} Disp_Ctl_union;

#define DISP_FW_REV_REG_ADDR 4
#define DISP_REG_ADDR_0 4
#define DISP_REG_MAX_ADDR 17

#define Disp_CtlBlk Disp_Ctl_union._disp_ctl_blk;
#define Disp_CtlRegs Disp_Ctl_union.registers;

#define SGNL_UPD_DISP_BIT	SGNL_GENERIC_BIT
#define _SGNL_UPD_DISP_MASK	_SGNL_GENERIC_MASK
#define SGNL_UPD_DISP_MASK	SGNL_GENERIC_MASK


// We set MAX_RES in order to set the maximum resolution of the build
// MAX_RES == 0: Max Resolution of single degrees. e.g. 112 degrees
// MAX_RES == 1: Max Resolution of 1/2 of degree. e.g. 112 1/2 degrees
// MAX_RES == 2: Max Resolution of 1/4 of degree. e.g. 112 3/4 degrees
// MAX_RES == 3: Max Resolution of 1/8 of degree. e.g. 112 5/8 degrees
// MAX_RES == 4: Max Resolution of 1/16 of degree. e.g. 112 9/16 degrees

#define MAX_RES 1

#define MAX_HDG_VALUE_RES_SHIFT 4
#define MAX_HDG_VALUE_RES_FACTOR 16
#define MAX_ABS_HEADING_VALUE   ((360*MAX_HDG_VALUE_RES_FACTOR)-1)
#define MAX_ABS_HDG_MODULO MAX_ABS_HEADING_VALUE+1
#define HDG_ADJ_LOW_LIMIT (20*MAX_HDG_VALUE_RES_FACTOR)
#define HDG_ADJ_HIGH_LIMIT (340*MAX_HDG_VALUE_RES_FACTOR)
#define HDG_VALUE_INCR 16

#if MAX_RES==0
// if max resolution is to single degrees
#define MAX_HEADING_VALUE   359
#define HDG_VALUE_RES_SHIFT (MAX_HDG_VALUE_RES_SHIFT-0)
#endif

#if MAX_RES==1
// if max resolution is to 1/2 degrees
#define MAX_HEADING_VALUE   719
#define HDG_VALUE_RES_SHIFT (MAX_HDG_VALUE_RES_SHIFT-1)
#endif

#if MAX_RES==2
// if max resolution is to 1/4 degrees
#define MAX_HEADING_VALUE   1439
#define HDG_VALUE_RES_SHIFT (MAX_HDG_VALUE_RES_SHIFT-2)
#endif

#if MAX_RES==3
// if max resolution is to 1/8 degrees
#define MAX_HEADING_VALUE   2879
#define HDG_VALUE_RES_SHIFT (MAX_HDG_VALUE_RES_SHIFT-3)
#endif

#if MAX_RES==4
// if max resolution is to 1/16 degrees
#define MAX_HEADING_VALUE   5759
#define HDG_VALUE_RES_SHIFT (MAX_HDG_VALUE_RES_SHIFT-4)
#endif


#define DISP_RIGHT_TO_LEFT   0
#define DISP_LEFT_TO_RIGHT   1
#define   DISP_MODE 1

#define BAR_DISP_WIDTH 48
#define BAR_DISP_MIDPOINT (BAR_DISP_WIDTH/2)
#define LED_BAR_CNT   96
#define LED_BAR_BYTE_CNT (LED_BAR_CNT/8)

#if DISP_LEFT_TO_RIGHT==1
#define BAR_DISP_LEFT_END 0
#define BAR_DISP_RIGHT_END (BAR_DISP_WIDTH-1)
#else
#define BAR_DISP_LEFT_END (BAR_DISP_WIDTH-1)
#define BAR_DISP_RIGHT_END 0
#endif

#define INNER_LED_BIT 64
#define OUTER_LED_BIT 128

#define DGT_DISP_BITS 24
#define LED_DIGIT_CNT   DGT_DISP_BITS

#define  BAR_SCLK_PORT     PORTA
#define  BAR_SCLK_BIT      4
#define  DGT_SCLK_PORT     PORTA
#define  DGT_SCLK_BIT      9
#define  DD_SDATA_OUT_PORT    PORTB
#define  DD_SDATA_OUT_BIT     3
#define  BD_SDATA_OUT_PORT    PORTC
#define  BD_SDATA_OUT_BIT     5
#define  PCLK_PORT         PORTB
#define  PCLK_BIT          14

#define TMR2_INIT          0x200

#define DISP_DGT_LVL_OFF   0
#define DISP_DGT_LVL_LOW   1
#define DISP_DGT_LVL_MED   2
#define DISP_DGT_LVL_HIGH  3
#define DISP_DGT_LVL_MAX DISP_DGT_LVL_HIGH

#define DISP_BAR_LVL_OFF   0x200
#define DISP_BAR_LVL_LOW   0x1FF
#define DISP_BAR_LVL_MED   0x160
#define DISP_BAR_LVL_HIGH   0

#endif


uint8 const hex2_7seg[16] = { 
  0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,
  0x7F,0x6F,0x5C,0x7C,0x58,0x21,0x06,0x71};

uint8 const BitMask[8] = {1,2,4,8,0x10,0x20,0x40,0x80};

uint8 const BarAddrLU[16] = {0,1,8,9,2,3,0xA,0xB,4,5,0xC,0xD,6,7,0xE,0xF};

int8 Disp_ProcTaskState;
int8 digDsplyAscii[4];
int8 bar_dsply[16];
int8 bar_dsply_byte_cnt;
int8 DgtDsplyLevel;
int8 BarDsplyLevel;
int8 BarDsplyLed;
