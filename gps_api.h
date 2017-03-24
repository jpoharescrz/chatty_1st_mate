#ifndef GPS_API_H
#define GPS_API_H 1

#include "gps_cnfg.h"
#include "gps_data_defs.h"

/////////////////////////////////////////////////////////////////////////
////                          gps_api.h                              ////
////   This file is the API for the GPS Receiver
////  
/////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
// void GPS_Init()
// 
// Initializes the FM transmitter
///////////////////////////////////////////////////////////////////
void GPS_Init();

char *GPS_GetStats( int8 Clear, char *buf );

int8 GPS_Attached();
void GPS_on();
void GPS_off();
void GPS_on_off_chg( uint8 gps_on );
void GPS_status_report();
void GPS_Upd_Int_RMB_fields( long double lat, long double lon );

// Internal prototypes
#ifdef GPS
void GPS_UpdateWptBtw( long double lat, long double lon );
void GPS_UpdateWptDtw( long double lat, long double lon );
void GPS_UpdateWptVtw( long double lat, long double lon );
void GPS_UpdateWptXte( long double lat, long double lon );
#endif

#define SGNL_UPD_INT_RMB_MASK	SGNL_GENERIC_MASK
#define _SGNL_UPD_INT_RMB_MASK	_SGNL_GENERIC_MASK
#define SGNL_UPD_INT_RMB_BIT	SGNL_GENERIC_BIT

#endif
