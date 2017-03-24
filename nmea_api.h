#ifndef NMEA_API_H
#define NMEA_API_H 0

#undef NMEA_FLDREC_HIRES_VALUE

#include "rs232_api.h"
#include "nmea_cnfg.h"
#include "nmea_data_defs.h"

char *Nmea_GetMsgCopy( int UsartIdx );

int Nmea_UsartAClientReady();
int Nmea_UsartBClientReady();
void Nmea_ReportFldRec( int16 NmeaRecIndex, uint8 RptOptions );
void Nmea_RptRec( int16 index );
uint8 Nmea_SetFldRecParms( int16 rec_index, int16 spch_indices, int16 max_period, int16 min_period, int16 rpt_delta, 
						   int16 min_integers, int16 max_decimals, int16 mod );
int8 Nmea_MsgsOk();
void Nmea_ClrMsgsStatusBit( int16 index );
int32 Nmea_GetMsgActiveFlds( int16 index);
void Nmea_SetMsgsExpectedWord( int16 ival );
int8 Nmea_SetMsgFields( int16 MsgIndex, int32 ExpectedFields, int32 MsgFields, int32 RecIndexFields );
void Nmea_EnableMsg( int16 MsgIndex );
void Nmea_DisableMsg( int16 MsgIndex );
int8 Nmea_SetFldRecString( uint16 index, char *fptr );
char *Nmea_GetFldRecStringPtr(int16 index);
void Nmea_SetFldRecValue( int16 index, int16 value );
int16 Nmea_GetFldRecValue( int16 index );
void Nmea_SetFldRecCnfgStatusBit( int16 index, int8 q );
void Nmea_ClrFldRecCnfgStatusBit( int16 index, int8 q );
void Nmea_SetFldRecCnfgStatusWord( int16 index, uint16 ival );
void Nmea_SetFldRecRptDelta( int16 index, int16 value );
void Nmea_SetFldRecRptTimer( int16 index, int16 value );
int8 Nmea_IsMsgActive( uint8 i);
uint32 _nmea_append_yaw_indicator_tone(int16 delta_16 );
uint16 Nmea_GetFldRecPostIndex( int16 idx );
void Nmea_ResetFldRecMaxPeriod( uint8 index );
void Nmea_ResetFldRecMinPeriod( uint8 index );
void Nmea_InitDefaultMagneticVariation();
void Nmea_EnableFldRecRpt( uint8 index );
void Nmea_DisableFldRecRpt( uint8 index );
void Nmea_SetAudCpsDeltaTones( uint8 value );
uint8 Nmea_IsNmeaRecActive( int16 index );
void Nmea_ActivateFldRec( int16 index );
void Nmea_CnfgNmeaMsgIsData( int16 MsgIndex, uint16 val );

char *NMEA_vog_max_period_index_to_ascii( uint8 value );
char *NMEA_vog_min_period_index_to_ascii( uint8 value );
char *NMEA_vog_delta_interval_index_to_ascii( uint8 value );

//////////////////////////////////////////////////////////////////
// char *NMEA_aud_cps_delta_interval_index_to_ascii( uint8 value )
//
// Takes the audio compass interval index and returns an ascii string
// of the number it maps to
//////////////////////////////////////////////////////////////////
char *NMEA_aud_cps_delta_interval_index_to_ascii( uint8 value );

//////////////////////////////////////////////////////////////////
// char *NMEA_aud_cps_max_period_index_to_ascii( uint8 value )
//
// Takes the audio compass interval index and returns an ascii string
// of the number it maps to
//////////////////////////////////////////////////////////////////
char *NMEA_aud_cps_max_period_index_to_ascii( uint8 value );

//////////////////////////////////////////////////////////////////
// char *NMEA_aud_cps_min_period_index_to_ascii( uint8 value )
//
// Takes the audio compass interval index and returns an ascii string
// of the number it maps to
//////////////////////////////////////////////////////////////////
char *NMEA_aud_cps_min_period_index_to_ascii( uint8 value );

void Nmea_SetVogDeltaTones( uint8 value );
void Nmea_SetVogDeltaInterval( uint8 value );
void Nmea_SetVogMaxPeriod( uint8 value );
void Nmea_SetVogMinPeriod( uint8 value );

void Nmea_SetAudCpsDeltaInterval( uint8 value );
uint32 _nmea_append_inter_delta_tone( int16 fld_rec_idx, int16 delta_16 );
void Nmea_SetAudCpsMaxPeriod( uint8 value );
void Nmea_SetAudCpsMinPeriod( uint8 value );

void Nmea_SetFldRecRptCounter( int16 idx, int16 val );

int16 Nmea_GetDefaultMagneticVariation();
void Nmea_UpdRptTrueHeading( uint8 rpt_true_heading );

/////////////////////////////////////////////////////////////////////////////
// Converts a latitude/longitude in ascii to double long float
/////////////////////////////////////////////////////////////////////////////
double long Nmea_GetLatLonFromMsg( int MsgSourceIdx, int fld_idx );

void NMEA_MuteDeltaOnly();
void NMEA_UnMuteDeltaOnly();

void NMEA_SetAudCpsHighRes();
void NMEA_SetAudCpsLowRes();

#endif







