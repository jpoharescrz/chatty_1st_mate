#ifndef FMT_API_H
#define FMT_API_H 1

#include "fmt_data_defs.h"

/////////////////////////////////////////////////////////////////////////
////                          Fmt_api.h                              ////
////   This file is the API for the FM transmitter
////  
/////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
// void FMT_Init()
// 
// Initializes the FM transmitter
///////////////////////////////////////////////////////////////////
void FMT_Init();

///////////////////////////////////////////////////////////////////
// void FMT_Mute( int8 Mute );
// 
// Mutes / UnMutes the FM transmitter
///////////////////////////////////////////////////////////////////
void FMT_Mute( int8 Mute );

///////////////////////////////////////////////////////////////////
// void FMT_reg_set( int8 addr; int8 value )
// 
// Sets the spec'd register in the FM transmitter with the spec'd value
///////////////////////////////////////////////////////////////////
void FMT_set_reg( int8 addr, int8 value );

void FMT_SetFreq( uint8 freq );

char *FMT_GetStats( int8 Clear, char *buf );

char *FMT_index_to_ascii( uint8 freq_idx );

void FMT_PowerOn( int FMT_on );

void FMT_status_report();

int8 FMT_GetStatus();

#endif