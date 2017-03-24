
enum adsc_task_state_indices {
    SOURCE_OK = 0,
    MSGS_IN_ERROR = 2,
    ADSC_PUP = 4
};


// How often we say "No Signal" when there are no messages
// in 100 ms units, so every 60 seconds
#define NO_SGNL_MSG_PERIOD 600
// When the source changes we set the SourceErrTmr to NO_SGNL_MSG_PERIOD_SRC_CHG
// to give the new source to come out of alarm if it is in alarm, 2 seconds here
#define NO_SGNL_MSG_PERIOD_SRC_CHG 30

// defines for TgtSource
#define TGT_SRC_NONE 0
#define TGT_SRC_HCHDM_HDG 1
#define TGT_SRC_GPRMC_HDG 2
#define TGT_SRC_GPRMB_BTW 3

#define TGT_LED_PULSER_PERIOD 6

#define ADSC_STATUS_POWERUP 0
#define ADSC_STATUS_NO_SIGNAL 1
#define ADSC_STATUS_RCVING_SIGNAL 2

#define ADSC_POWER_UP_CNT_INIT 150

#define ADSC_PILOT_CNT_INIT 50
