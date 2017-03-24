#ifndef BUTTON_API_H
#define BUTTON_API_H

#include "button_data_defs.h"

uint8 Btn_SetParm(uint16 index, uint8 ival, uint8 save);
uint8 Btn_GetParm(int index);
uint8 Btn_GetFmtFreq();
int16 Btn_GetHdgCalAdj();
void Btn_SetFmtFreq( uint8 freq );
int8 Btn_IsSpchMuted();

void Btn_TargetOff();
void Btn_SetRemoteButtonState( uint16 Button );
void Cnfg_SaveSettings( int8 );

void Btn_SetFMT_Present();

void Btn_SetGPS_Present();
uint16 Btn_DialogInProgress();
void Btn_AnnounceSpfcMode(int8 ItemIndex, int8 force );
uint8 Cnfg_IsCnfgFlash2ParmValid( uint8 parm_idx );
void Cnfg_WriteCnfgFlash2Parm( uint8 parm_idx, uint8 *pp );
uint8 *Cnfg_GetCnfgFlash2ParmPtr( uint8 parm_idx );
uint8 Cnfg_GetCnfgFlash2ParmChecksum( uint8 parm_idx );
void Cnfg_SaveCnfgFlash2Settings();
////////////////////////////////////////////////////////////////////////
// void Btn_report_hw_cnfg_status()
//
// Reports the Product, version #, and state of the FMT, GPS, and Data source
//
////////////////////////////////////////////////////////////////////////
void Btn_report_hw_cnfg_status();

#ifdef BUTTON
// Internal prototypes
void _cnfg_load_settings_from_flash();
void _btn_SetBuzzerVolume( int8 vol );
int8 _btn_event_switch( int8 event );
void _btn_new_menu_item( int8 index );
void _cnfg_LoadImageFromCnfgFlash2();
void _cnfg_WriteImageToCnfgFlash2();

#endif

#endif

