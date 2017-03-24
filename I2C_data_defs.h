/////////////////////////////////////////////////////////////////////////
////                          I2C_data_def.h                              ////
////   This file is included in the I2C_Master.c file and
////   defines data structures and constants for I2C.
/////////////////////////////////////////////////////////////////

#define I2C_MASTER 
//#define I2C_SLAVE 0
#define I2C_CNFG_ENB 	0x8200
#define I2C_CNFG_SIDL 	0x2000
#define I2C_CNFG_SCLREL 0x1000
#define I2C_CNFG_IPMIEN 0x0800
#define I2C_CNFG_A10M 	0x0400
#define I2C_CNFG_DISSLW 0x0200
#define I2C_CNFG_SMEN 	0x0100
#define I2C_CNFG_GCEN 	0x0080
#define I2C_CNFG_STREN 	0x0040
#define I2C_CNFG_ACKDT 	0x0010
#define I2C_CNFG_RCEN 	0x0008
#define I2C_CNFG_PEN 	0x0004
#define I2C_CNFG_RSEN 	0x0002
#define I2C_CNFG_SEN 	0x0001

#define I2C1_IDLE_OK() ((I2C1CONL & 0x001F) == 0)

#define I2C_CNFG_ENB 0x8200

#define I2C_CONFIG_INIT (I2C_CNFG_ENB )

// Configure I2C to run at 100 KHZ
//#define I2C_CNFG_BRG 197
#define I2C_CNFG_BRG 0x004E

#define	I2C1_RESET	PORTCbits.RC6

#define i2c1_enable() I2C1CONLbits.I2CEN = 1
#define i2c1_disable() I2C1CONLbits.I2CEN = 0
#define i2c1_start() I2C1CONLbits.SEN = 1
#define i2c1_restart() I2C1CONLbits.RSEN = 1
#define i2c1_stop() I2C1CONLbits.PEN = 1
#define i2c1_write(reg_data) {I2C1TRN = reg_data;}
#define i2c1_read() I2C1RCV
#define i2c1_clr_sda_scl() {PORTB &= 0xFCFF;}
//#define i2c_ack() AckI2C1()

#define I2C1_ACKDATA I2C1CONLbits.ACKDT
#define I2C1_ACKDATA_EN I2C1CONLbits.ACKEN
#define I2C1_TBF I2C1STATbits.TBF
// Tranmit Status
#define I2C1_TRSTAT I2C1STATbits.TRSTAT
#define I2C1_RBF I2C1STATbits.RBF
#define I2C1_ACKSTAT (I2C1STATbits.ACKSTAT == 0)
#define I2C1_BCL I2C1STATbits.BCL
// Receive Overflow
#define I2C1_I2COV I2C1STATbits.I2COV
#define I2C1_IWCOL I2C1STATbits.IWCOL
// Stop bit
#define I2C1_P		I2C1STATbits.P
// Start bit
#define I2C1_S		I2C1STATbits.S

#define I2C_RCEN_MASK 0x0008
#define I2C_I2CEN_MASK 0x8000
#define _I2C_I2CEN_MASK 0x7FFF

#define I2C_I2COV_MASK 0x0020 
#define _I2C_I2COV_MASK 0xFFDF 

#define I2C1_INT(state) IEC1bits.MI2C1IE=state
#define I2C1_INT_FLAG() IFS1bits.MI2C1IF

#define I2C_READ_BIT	0x01

#define I2C_CMD_ERR_NO_ADDRESS 0
#define I2C_CMD_ERR_NO_PHRASE 1
#define I2C_CMD_ERR_UNKNOWN_CMD 2
#define I2C_CMD_ERR_BAD_TERM 3
#define I2C_CMD_ERR_BAD_ARGS 4
// Add new Error codes after this comment

#define I2C_STT_POWERUP 		0
#define I2C_STT_WAITING 		1
#define I2C_STT_SEND_ADDR 		2
#define I2C_STT_SEND_ADDR_ACK	3
#define I2C_STT_SEND_REG_ACK	4
#define I2C_STT_WRITE_BYTES 	5
#define I2C_STT_STOP_WRITING 	6
#define I2C_STT_SEND_READ_ADDR 		7
#define I2C_STT_SEND_READ_ADDR_ACK	8
#define I2C_STT_INITIATE_READ 	9
#define I2C_STT_COMPLETE_READ 	10
#define I2C_STT_STOP			11

#define I2C_POWERUP_CNT 2
#define I2C_WAIT_TIMEOUT 100

#define I2C_CMD_WRT_BYTES 0
#define I2C_CMD_READ_REG8_BYTES 1

typedef struct I2C_TxCmdRecStruct {
  int8 SlaveAddr;
  int8 Reg;
  int8 ByteCnt;
  int8 CmdType;
  int8 TaskId;
  uint8 TaskSgnl;
  uint8 *read_ptr;
} I2C_TxCmdRec;

typedef struct I2C_CtrlBlkRec_struct {
  int8 I2cTaskState;		//0x01
  int8 I2cPupTimer;
  int8 TxCmdRdIdx;
  int8 TxCmdWtIdx;
  int8 TxCmdCnt;
  uint8 read_wait_cntr;
  I2C_TxCmdRec	*CurCmdPtr;
  I2C_TxCmdRec  TxCmdRecords[ I2C_MAX_TX_CMDS ];
  uint16 i2c_abort_cntr;
  uint16 HwLockUpCnt;
  uint8 HwLockUp;

  int8 TxByteCnt;
  int8 TxBufWtIdx;
  int8 TxBufRdIdx;
  char TxBuf[ I2C_TX_BUF_SIZE ];

} I2C_CtrlBlkRec;

I2C_CtrlBlkRec I2C_CtrlBlk;


