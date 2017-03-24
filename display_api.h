
#include "display_defs.h"

char *Disp_GetDigDsplyStr();
int8 *Disp_GetBarDsplyAddr();
int8 Disp_GetBarDsplyLevel();
int8 Disp_GetDgtDsplyLevel();
void Disp_ClearBarDisplay();
void Disp_SetBarLed(uint16 led_pos);
void Disp_SetBarLeds(int8 led_pos, int8 count, int8 port );
void Disp_ClrBarLed(uint16 led_pos);
void Disp_ClrBarLeds(int8 led_pos, int8 count, int8 port );
void Disp_SetBarInnerLed(uint16 led_pos);
void Disp_SetBarOuterLed(uint16 led_pos);
char *GetDsplyAsciiStr( int16 i, char *str );
char *Disp_UpdtDgtDisplayImage( int16 i );
void Disp_NoSignalDgtDisplayImg();
int16 Disp_hdg_res_adjust( int16 heading );
void Disp_SetLvlDgtDsply( int8 level );
void Disp_SetLvlBarDsply( int8 level );
void Disp_UpdtDsplyTgtImage( int16 tgt_heading, int16 tgt_offset, int16 delta );
void Disp_CnfgSweep(int16 hdg_roc);
