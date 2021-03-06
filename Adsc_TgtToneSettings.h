// TGT_TONE_SCHEME_0 uses a continuous tone change sloping up
// or down
#if TGT_TONE_SCHEME == 0

#define ADSC_TGT_TONE_TGT_MIN	1
#define ADSC_TGT_TONE_GROSS_OFFCOURSE_MIN 5
#define ADSC_TGT_TONE_BALANCE_SHIFT_OFFCOURSE_MIN 2
#define ADSC_TGT_TONE_MAX_TONE_OFF_PERIOD 95
#define ADSC_TGT_TONE_MAX_OFFSET 20
#define ADSC_TGT_TONE_TONE_AMPLITUDE 35
#define ADSC_TGT_TONE_TONE_PERIOD 25
#define ADSC_TGT_TONE_TGT_MIN_PERIOD 15
#define ADSC_TGT_TONE_ONCOURSE_PERIOD 3
#define ADSC_TGT_TONE_TGT_MIN_AMPLITUDE 20
#define ADSC_TGT_TONE_ONCOURSE_AMPLITUDE 0

#define ADSC_TGT_TONE_TONE_FREQ_PERIOD	60
#define ADSC_TGT_TONE_TONE_FREQ_PERIOD_SHIFT	9
#define ADSC_TGT_TONE_GROSS_OFFCOURSE_FREQ_PERIOD_SHIFT 7
#define ADSC_TGT_TONE_GROSS_OFFCOURSE_SPKR_BALNCE_SHIFT 3
#define ADSC_TGT_TONE_TONE_FREQ_PERIOD_SHIFT_2	15
#define ADSC_TGT_TONE_PORT_TONE_FREQ_PERIOD ADSC_TGT_TONE_TONE_FREQ_PERIOD-ADSC_TGT_TONE_TONE_FREQ_PERIOD_SHIFT
#define ADSC_TGT_TONE_STBD_TONE_FREQ_PERIOD ADSC_TGT_TONE_TONE_FREQ_PERIOD

#define TGT_TONE_UPDATE_CNT_INIT 5

#endif

// TGT_TONE_SCHEME_1 uses a stepped two tone change stepping up
// or down
#if TGT_TONE_SCHEME == 1

#define ADSC_TGT_TONE_TGT_MIN	1
#define ADSC_TGT_TONE_GROSS_OFFCOURSE_MIN 5
#define ADSC_TGT_TONE_BALANCE_SHIFT_OFFCOURSE_MIN 0
#define ADSC_TGT_TONE_MAX_TONE_OFF_PERIOD 100
#define ADSC_TGT_TONE_MAX_OFFSET 17
#define ADSC_TGT_TONE_TONE_PERIOD 25
#define ADSC_TGT_TONE_ONCOURSE_PERIOD 2
#define ADSC_TGT_TONE_ONCOURSE_AMPLITUDE 0
#define ADSC_TGT_TONE_TGT_MIN_PERIOD 4
#define ADSC_TGT_TONE_TGT_MIN_AMPLITUDE 6
#define PERIOD_OFFSET_FACTOR 0
#define AMPLITUDE_OFFSET_FACTOR 1

#define ADSC_TGT_TONE_TONE_FREQ_PERIOD	52
#define ADSC_TGT_TONE_TONE_FREQ_PERIOD_SHIFT	4
#define ADSC_TGT_TONE_GROSS_OFFCOURSE_FREQ_PERIOD_SHIFT 2
#define ADSC_TGT_TONE_GROSS_OFFCOURSE_SPKR_BALNCE_SHIFT 2
#define ADSC_TGT_TONE_TONE_FREQ_PERIOD_SHIFT_2	0
#define ADSC_TGT_TONE_PORT_TONE_FREQ_PERIOD ADSC_TGT_TONE_TONE_FREQ_PERIOD-ADSC_TGT_TONE_TONE_FREQ_PERIOD_SHIFT
#define ADSC_TGT_TONE_STBD_TONE_FREQ_PERIOD ADSC_TGT_TONE_TONE_FREQ_PERIOD

#define TGT_TONE_UPDATE_CNT_INIT 5

#endif

// TGT_TONE_SCHEME_2 uses a stepped two tone change stepping up
// or down
#if TGT_TONE_SCHEME == 2

#define ADSC_TGT_TONE_TGT_MIN	0
#define ADSC_TGT_TONE_GROSS_OFFCOURSE_MIN 5
#define ADSC_TGT_TONE_BALANCE_SHIFT_OFFCOURSE_MIN 0
#define ADSC_TGT_TONE_MAX_TONE_OFF_PERIOD 90
#define ADSC_TGT_TONE_MAX_OFFSET 12
#define ADSC_TGT_TONE_TONE_PERIOD 25
#define ADSC_TGT_TONE_ONCOURSE_PERIOD 10
#define ADSC_TGT_TONE_ONCOURSE_AMPLITUDE 0
#define ADSC_TGT_TONE_TGT_MIN_PERIOD 10
#define ADSC_TGT_TONE_TGT_MIN_AMPLITUDE 0
#define PERIOD_OFFSET_FACTOR 1
#define AMPLITUDE_OFFSET_FACTOR 0

#define ADSC_TGT_TONE_TONE_FREQ_PERIOD	52
#define ADSC_TGT_TONE_TONE_FREQ_PERIOD_SHIFT	4
#define ADSC_TGT_TONE_GROSS_OFFCOURSE_FREQ_PERIOD_SHIFT 2
#define ADSC_TGT_TONE_GROSS_OFFCOURSE_SPKR_BALNCE_SHIFT 2
#define ADSC_TGT_TONE_TONE_FREQ_PERIOD_SHIFT_2	0
#define ADSC_TGT_TONE_PORT_TONE_FREQ_PERIOD ADSC_TGT_TONE_TONE_FREQ_PERIOD-ADSC_TGT_TONE_TONE_FREQ_PERIOD_SHIFT
#define ADSC_TGT_TONE_STBD_TONE_FREQ_PERIOD ADSC_TGT_TONE_TONE_FREQ_PERIOD

#define TGT_TONE_UPDATE_CNT_INIT 4

#endif

// TGT_TONE_SCHEME_3 uses a stepped two tone change stepping up
// or down
#if TGT_TONE_SCHEME == 3

#define ADSC_TGT_TONE_TGT_MIN	0
#define ADSC_TGT_TONE_GROSS_OFFCOURSE_MIN 5
#define ADSC_TGT_TONE_BALANCE_SHIFT_OFFCOURSE_MIN 0
#define ADSC_TGT_TONE_MAX_TONE_OFF_PERIOD 90
#define ADSC_TGT_TONE_MAX_OFFSET 12
#define ADSC_TGT_TONE_TONE_PERIOD 25
#define ADSC_TGT_TONE_ONCOURSE_PERIOD 10
#define ADSC_TGT_TONE_ONCOURSE_AMPLITUDE 0
#define ADSC_TGT_TONE_TGT_MIN_PERIOD 10
#define ADSC_TGT_TONE_TGT_MIN_AMPLITUDE 0
#define PERIOD_OFFSET_FACTOR 1
#define AMPLITUDE_OFFSET_FACTOR 0

#define ADSC_TGT_TONE_TONE_FREQ_PERIOD	52
#define ADSC_TGT_TONE_TONE_FREQ_PERIOD_SHIFT	4
#define ADSC_TGT_TONE_GROSS_OFFCOURSE_FREQ_PERIOD_SHIFT 2
#define ADSC_TGT_TONE_GROSS_OFFCOURSE_SPKR_BALNCE_SHIFT 2
#define ADSC_TGT_TONE_TONE_FREQ_PERIOD_SHIFT_2	0
#define ADSC_TGT_TONE_PORT_TONE_FREQ_PERIOD ADSC_TGT_TONE_TONE_FREQ_PERIOD-ADSC_TGT_TONE_TONE_FREQ_PERIOD_SHIFT
#define ADSC_TGT_TONE_STBD_TONE_FREQ_PERIOD ADSC_TGT_TONE_TONE_FREQ_PERIOD

#define TGT_TONE_UPDATE_CNT_INIT 4

#endif

// TGT_TONE_SCHEME_4 uses a stepped two tone change stepping up
// or down
#if TGT_TONE_SCHEME == 4

#define ADSC_TGT_TONE_TGT_MIN	1
#define ADSC_TGT_TONE_GROSS_OFFCOURSE_MIN 5
#define ADSC_TGT_TONE_BALANCE_SHIFT_OFFCOURSE_MIN 0
#define ADSC_TGT_TONE_MAX_TONE_OFF_PERIOD 100
#define ADSC_TGT_TONE_MAX_OFFSET 17
#define ADSC_TGT_TONE_TONE_PERIOD 25
#define ADSC_TGT_TONE_ONCOURSE_PERIOD 2
#define ADSC_TGT_TONE_ONCOURSE_AMPLITUDE 0
#define ADSC_TGT_TONE_TGT_MIN_PERIOD 4
#define ADSC_TGT_TONE_TGT_MIN_AMPLITUDE 6
#define PERIOD_OFFSET_FACTOR 0
#define AMPLITUDE_OFFSET_FACTOR 1

#define ADSC_TGT_TONE_TONE_FREQ_PERIOD	52
#define ADSC_TGT_TONE_TONE_FREQ_PERIOD_SHIFT	4
#define ADSC_TGT_TONE_GROSS_OFFCOURSE_FREQ_PERIOD_SHIFT 2
#define ADSC_TGT_TONE_GROSS_OFFCOURSE_SPKR_BALNCE_SHIFT 2
#define ADSC_TGT_TONE_TONE_FREQ_PERIOD_SHIFT_2	0
#define ADSC_TGT_TONE_PORT_TONE_FREQ_PERIOD ADSC_TGT_TONE_TONE_FREQ_PERIOD-ADSC_TGT_TONE_TONE_FREQ_PERIOD_SHIFT
#define ADSC_TGT_TONE_STBD_TONE_FREQ_PERIOD ADSC_TGT_TONE_TONE_FREQ_PERIOD

#define TGT_TONE_UPDATE_CNT_INIT 5

#endif
