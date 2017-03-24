/////////////////////////////////////////////////////////////////////////
////                          isd_api.h                              ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////

#ifndef ISD_API_H
#define ISD_API_H

int IsIsdReady(uint8 Owner);
void Isd_SignalTask( uint8 CmdSrc );
int Isd_UsartAClientReady();
int Isd_UsartBClientReady();
int Isd_UsartUsbClientReady();

#define ISDTRACE_TTS_TO_ISD 1
#define ISDTRACE_TTS_TO_ISD_MASK (1 << ISDTRACE_TTS_TO_ISD)

void IsdTrace_SetBit( int tbit );
void IsdTrace_ClrBit( int tbit );
void IsdTrace_SetPort( int port );
void IsdTrace_SetMask( int tmask );
void IsdTrace_ClrMask();
char *Isd_GetStats( int8 Clear, char *buf );

void isd_INT0_Isr(void);

#ifdef ISD
void _isd_free_port_buffer( int8 CmdSrc );
#endif

#endif
