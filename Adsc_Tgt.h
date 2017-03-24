
// Set minimum period on-course announced, times 100 ms
#define ADSC_TGT_ON_COURSE_MIN_PERIOD 300
#define ADSC_TGT_WAY_OFF_COURSE_MIN_PERIOD 200

#define MAX_OFFCOURSE_LEVEL 5

// LU table to figure off-course level given the absolute offset
const int8 OffCourseLevel[] = {0,0,1,1,1,2,2,2,3,3,3,4,4,4,4,5,5,5,5,5,5,5,5,5,5};

#define ADSC_TGT_NOTE_LVL0_UPDT_INIT 80
#define ADSC_TGT_NOTE_LVL1_UPDT_INIT 60
#define ADSC_TGT_NOTE_LVL2_UPDT_INIT 45
#define ADSC_TGT_NOTE_LVL3_UPDT_INIT 20
#define ADSC_TGT_NOTE_LVL4_UPDT_INIT 15
#define ADSC_TGT_NOTE_LVL5_UPDT_INIT 10

const int8 UpdatePeriod[] = {
	ADSC_TGT_NOTE_LVL0_UPDT_INIT,
	ADSC_TGT_NOTE_LVL1_UPDT_INIT,
	ADSC_TGT_NOTE_LVL2_UPDT_INIT,
	ADSC_TGT_NOTE_LVL3_UPDT_INIT,
	ADSC_TGT_NOTE_LVL4_UPDT_INIT,
	ADSC_TGT_NOTE_LVL5_UPDT_INIT};

#define TGT_TONE_SCHEME_4

const unsigned int8 OffCourseNote[] = {
			
#ifdef TGT_TONE_SCHEME_1
#define TGT_TONE_SCHEME 1
// Two tones. Lower-higher when target to pot
// higher-lower when target to starboard
// lower tone-pairs when close to on course
			TTS_ADDR_NOTE_C4_INDEX,	TTS_ADDR_NOTE_C4_INDEX,// level 0, target to stbd
			TTS_ADDR_NOTE_C4_INDEX,	TTS_ADDR_NOTE_E4_INDEX,// level 1, target to stbd
			TTS_ADDR_NOTE_D4_INDEX,	TTS_ADDR_NOTE_F4_INDEX,// level 2, target to stbd
			TTS_ADDR_NOTE_E4_INDEX,	TTS_ADDR_NOTE_G4_INDEX,// level 3, target to stbd
			TTS_ADDR_NOTE_F4_INDEX,	TTS_ADDR_NOTE_A4_INDEX,// level 4, target to stbd
			TTS_ADDR_NOTE_A4_INDEX,	TTS_ADDR_NOTE_B4_INDEX,// level 5, target to stbd

			TTS_ADDR_NOTE_C4_INDEX,	TTS_ADDR_NOTE_C4_INDEX,// level 0, target to port
			TTS_ADDR_NOTE_E4_INDEX,	TTS_ADDR_NOTE_C4_INDEX,// level 1, target to port
			TTS_ADDR_NOTE_F4_INDEX,	TTS_ADDR_NOTE_D4_INDEX,// level 2, target to port
			TTS_ADDR_NOTE_G4_INDEX,	TTS_ADDR_NOTE_E4_INDEX,// level 3, target to port
			TTS_ADDR_NOTE_A4_INDEX,	TTS_ADDR_NOTE_F4_INDEX,// level 4, target to port
			TTS_ADDR_NOTE_B4_INDEX,	TTS_ADDR_NOTE_A4_INDEX,// level 5, target to port
#endif

#ifdef TGT_TONE_SCHEME_2
#define TGT_TONE_SCHEME 2
// Two tones. Lower-higher when target to port
// higher-lower when target to starboard
// Higher tone-pairs when close to on course
			TTS_ADDR_NOTE_C5_INDEX,	TTS_ADDR_NOTE_C5_INDEX,// level 0, target to stbd
			TTS_ADDR_NOTE_A4_INDEX,	TTS_ADDR_NOTE_B4_INDEX,// level 1, target to stbd
			TTS_ADDR_NOTE_F4_INDEX,	TTS_ADDR_NOTE_A4_INDEX,// level 2, target to stbd
			TTS_ADDR_NOTE_E4_INDEX,	TTS_ADDR_NOTE_G4_INDEX,// level 3, target to stbd
			TTS_ADDR_NOTE_D4_INDEX,	TTS_ADDR_NOTE_F4_INDEX,// level 4, target to stbd
			TTS_ADDR_NOTE_C4_INDEX,	TTS_ADDR_NOTE_E4_INDEX,// level 5, target to stbd

			TTS_ADDR_NOTE_C5_INDEX,	TTS_ADDR_NOTE_C5_INDEX,// level 0, target to port
			TTS_ADDR_NOTE_B4_INDEX,	TTS_ADDR_NOTE_A4_INDEX,// level 1, target to port
			TTS_ADDR_NOTE_A4_INDEX,	TTS_ADDR_NOTE_F4_INDEX,// level 2, target to port
			TTS_ADDR_NOTE_G4_INDEX,	TTS_ADDR_NOTE_E4_INDEX,// level 3, target to port
			TTS_ADDR_NOTE_F4_INDEX,	TTS_ADDR_NOTE_D4_INDEX,// level 4, target to port
			TTS_ADDR_NOTE_E4_INDEX,	TTS_ADDR_NOTE_C4_INDEX,// level 5, target to port

#endif	

#ifdef TGT_TONE_SCHEME_3
#define TGT_TONE_SCHEME 3
// Two tones in one file. Lower-higher when target to pot
// higher-lower when target to starboard
// Higher tone-pairs when close to on course
			TTS_ADDR_HEADING_STEADY_INDEX,	0,// level 0, target to stbd
			TTS_ADDR_NOTE_A4_B4_INDEX,	0,// level 1, target to stbd
			TTS_ADDR_NOTE_F4_A4_INDEX,	0,// level 2, target to stbd
			TTS_ADDR_NOTE_E4_G4_INDEX,	0,// level 3, target to stbd
			TTS_ADDR_NOTE_D4_F4_INDEX,	0,// level 4, target to stbd
			TTS_ADDR_NOTE_C4_E4_INDEX,	0,// level 5, target to stbd

			TTS_ADDR_HEADING_STEADY_INDEX, 0,// level 0, target to port
			TTS_ADDR_NOTE_B4_A4_INDEX,	0,// level 1, target to port
			TTS_ADDR_NOTE_A4_F4_INDEX,	0,// level 2, target to port
			TTS_ADDR_NOTE_G4_E4_INDEX,	0,// level 3, target to port
			TTS_ADDR_NOTE_F4_D4_INDEX,	0,// level 4, target to port
			TTS_ADDR_NOTE_E4_C4_INDEX,	0,// level 5, target to port

#endif	

#ifdef TGT_TONE_SCHEME_4
#define TGT_TONE_SCHEME 4
// Two tones in one file. Lower-higher when target to pot
// higher-lower when target to starboard
// Higher tone-pairs when close to on course
			TTS_ADDR_HEADING_STEADY_INDEX,	0,// level 0, target to stbd
			TTS_ADDR_DELTA_STBD_INDEX,	0,// level 1, target to stbd
			TTS_ADDR_DELTA_STBD_INDEX,	0,// level 2, target to stbd
			TTS_ADDR_DELTA_STBD_INDEX,	0,// level 3, target to stbd
			TTS_ADDR_DELTA_STBD_INDEX,	0,// level 4, target to stbd
			TTS_ADDR_DELTA_STBD_INDEX,	0,// level 5, target to stbd

			TTS_ADDR_HEADING_STEADY_INDEX, 0,// level 0, target to port
			TTS_ADDR_DELTA_PORT_INDEX,	0,// level 1, target to port
			TTS_ADDR_DELTA_PORT_INDEX,	0,// level 2, target to port
			TTS_ADDR_DELTA_PORT_INDEX,	0,// level 3, target to port
			TTS_ADDR_DELTA_PORT_INDEX,	0,// level 4, target to port
			TTS_ADDR_DELTA_PORT_INDEX,	0,// level 5, target to port

#endif	
};

// intertone delay applied to a two tone scheme based on off-course level
int16 tgt_internote_dly[] = {
		TTS_POSTDELAY_500MS,		// level 0, on-course
		TTS_POSTDELAY_300MS,		// level 1
		TTS_POSTDELAY_200MS,		// level 2
		TTS_POSTDELAY_100MS,		// level 3
		0,							// level 4
		0							// level 5
};
		
	

//int16 target_offset;				 // difference between heading - target_heading
//int16 last_target_offset;
//int16 last_rptd_abs_offset;			// last reported using tones/notes
//int16 _DeltaUpdate;
//int8 last_tgt_offset = 0;
//int8 offcourselevel = 0;
//int8 target_tone_update_cnt = 1;

typedef struct adsc_tgt_cblk_struct {
	
	int16 target_offset;				 // difference between heading - target_heading
	int16 last_target_offset;
	int16 last_rptd_abs_offset;			// last reported using tones/notes
	int16 last_rptd_trend;				// last reported trend tones/notes
	int8 last_tgt_offset;
	int8 offcourselevel;
	int8 target_tone_update_cnt;
	int16 oncourse_hdg_announce_cnt;
	int16 offcourse_hdg_announce_cnt;
	
} adsc_tgt_ctl_blk;

adsc_tgt_ctl_blk adsc_tgt_cblk;


void Init_AdscTgt();

#include "Adsc_TgtToneSettings.h"
//////////////////////////////////////////////////////////////////
// void Adsc_Targeting_Tone( int8 suspend_flag, int16 tgt_offset )
//
// Updates the targeting tones depending on the args:
// tgt_offset (XXXY format) holds off course amount
// suspend_flag: TRUE shuts down the tones
/////////////////////////////////////////////////////////////////
// Tell the TTS task to update the off-course tones
void Adsc_Targeting_Tone( int8 suspend_flag, int16 tgt_offset );


//////////////////////////////////////////////////////////////////
// void Adsc_Targeting_Tone_Suspend( )
//
// Updates the targeting tones depending on the args:
// tgt_offset holds units off-course
// tgt_offset_dir: bit 7 is port/starbard bit, bit 0 is suspend
//
/////////////////////////////////////////////////////////////////
void Adsc_Targeting_Tone_Suspend();
