/////////////////////////////////////////////////////////////////////////
////                          cmps03_data_defs.h                              ////
////   
/////////////////////////////////////////////////////////////////

#define CMPS03_I2C_ADDR 0xC0

#define CMPS03_REG_FW_REV 0
#define CMPS03_REG_HDG_HI 2
#define CMPS03_REG_CMD 22

#define CMPS03_STT_INITIALIZING 0
#define CMPS03_STT_READ_REGS 	1
#define CMPS03_STT_RUNNING 		2
#define CMPS03_STT_RESET_0 		3
#define CMPS03_STT_RESET_1 		4
#define CMPS03_STT_RESET_2 		5

#ifdef CMPS03

//$HCHDM,238,M<CR><LF>

typedef struct cmps03_ctl_struct {
	
	uint8 TaskState;
	uint8 reg_idx;
	uint8 reg_values[16];
	char msg_str[20];
	
} CMPS03_Ctl;

CMPS03_Ctl CMPS03_CtlBlk;
	
	
#endif

