#include <xc.h>

#include <string.h>
#include <stdlib.h>
#include <p24FJ128GB204.h>
#include "Core_api.h"
#include "rs232_api.h"
#include "nmea_api.h"
#include "rem_cnfg_tst.h"
#include "button_api.h"
#include "display_api.h"
#include "tts_api.h"
#include "fmt_api.h"
#include "I2C_api.h"
#include"rs232_api.h"


void _rem_cnfg_tst_rsp( int8 msg_src_idx, char *dptr)
{
 char *cptr;

 	cptr = Nmea_GetMsgCopy( msg_src_idx );
 	strcat( cptr, dptr );

 	RS232_Xmt_Msg( msg_src_idx, cptr );
}

// Testing commands
// $PMTST,(PMTST_FUNC_BTN_SET=0),[button index]*  //pushes the spec'd button
//	#define   BTN_INPUTS_TGT_DN_BIT   0
//	#define   BTN_INPUTS_TGT_UP_BIT   1
//	#define   BTN_INPUTS_TGT_TGL_BIT   2
//	#define   BTN_INPUTS_CNFG_CHG_BIT   3
//	#define   BTN_INPUTS_RESET_CPU_BIT   4
//	#define   BTN_INPUTS_SRC_CHG_BIT   5
//	#define   BTN_INPUTS_CNFG_MODE_BIT   6
// 	#define   BTN_INPUTS_SPEECH_MODE_BIT 7
// $PMTST,(PMTST_FUNC_READ_DGT_DSPLY=1)*   //returns the DIGIT display as three ascii numbers
// $PMTST,(PMTST_FUNC_READ_BAR_DSPLY=2)*   //returns the bar display TBD
// $PMTST,(PMTST_FUNC_READ_DGT_DSPLY_LVL=3)*   // TBD
// $PMTST,(PMTST_FUNC_READ_BAR_DSPLY_LVL=4)*   // TBD
// $PMTST,(PMTST_FUNC_SET_PARM=5),[parm index],[value]*   // Sets the spec'd cnfg parm to spec'd value
// $PMTST,(PMTST_FUNC_GET_PARM=6),[parm index]*   // Returns the spec'd cnfg parm to spec'd value
// $PMTST,(PMTST_FUNC_SAVE_CNFG=7)*  // Saves the configuration
// $PMTST,(PMTST_FUNC_SET_TRACE_MASK=8),mask*
// $PMTST,(PMTST_FUNC_GET_STATS=9),(clear=0/1)*
// $PMTST,(PMTST_FUNC_SET_CNFG_FLASH_2_PARM=12),CF_PARM,[parm value in muliple bytes]*

void RemCnfg_ProcMsg( int msg_src_idx )
{
 char *cptr, *p;
 signed int16 temp16, temp16_2;
 int i,j,k;
 char cstr[80];
 
    // Get the function index
    cptr = RS232_GetToken( msg_src_idx, PMTST_FUNC_IDX );
    temp16 = atol( cptr );
    cptr = RS232_GetToken( msg_src_idx, PMTST_ARG0_IDX );
	temp16_2 = atol( cptr );

    switch (temp16)
    {
        case PMTST_FUNC_BTN_SET:
            Btn_SetRemoteButtonState( (uint16)temp16_2 );
            cstr[0] = PROTOCOL_DELIM; cstr[1] = '1';cstr[2] = 0;
            
            _rem_cnfg_tst_rsp( msg_src_idx, cstr);
            break;

        case PMTST_FUNC_READ_DGT_DSPLY:
            // Get a pointer to the digit display image
            cptr = Disp_GetDigDsplyStr();
            cptr[0] |= 0x30;
            cptr[1] |= 0x30;
            cptr[2] |= 0x30;
            // Construct the command response
            _rem_cnfg_tst_rsp( msg_src_idx, cptr);
            break;

        case PMTST_FUNC_READ_BAR_DSPLY:
            cptr = Disp_GetBarDsplyAddr();
            break;

        case PMTST_FUNC_READ_DGT_DSPLY_LVL:
            i = Btn_GetParm(BTN_PARM_LVL_DGT_DISP_IDX);
            int8_to_ascii10( i, cstr);
            _rem_cnfg_tst_rsp( msg_src_idx, cstr);
            break;

        case PMTST_FUNC_READ_BAR_DSPLY_LVL:
            i = Btn_GetParm( BTN_PARM_LVL_BAR_DISP_IDX );
            int8_to_ascii10( i, cstr);
            _rem_cnfg_tst_rsp( msg_src_idx, cstr);
            break;

        case PMTST_FUNC_SET_PARM:
            // Get the index to the parm to set
            // cptr points at ARG0
            // temp16_2;

            // Get the value to use
            cptr = RS232_GetToken( msg_src_idx, PMTST_ARG1_IDX );
            j = atol(cptr);

            cstr[0] = PROTOCOL_DELIM;
            k = Btn_SetParm( temp16_2, j, FALSE);
            if ( k )
            {
                cstr[1] = '1';cstr[2] = 0;
            }
            else
            {
                cstr[1] = '0';cstr[2] = 0;
            }
            _rem_cnfg_tst_rsp( msg_src_idx, cstr);
            break;

        case PMTST_FUNC_GET_PARM:
            // Get the index to the parm to get
            // cptr points at ARG0
            //i = atol(cptr);

            k = Btn_GetParm( temp16_2 );
            cstr[0] = PROTOCOL_DELIM;
            Core_IntToAsciiStr( (long)k, cstr+1, 2 );
            _rem_cnfg_tst_rsp( msg_src_idx, cstr);

            break;

        case PMTST_FUNC_SAVE_CNFG:
            Cnfg_SaveSettings( FALSE );
            Cnfg_SaveCnfgFlash2Settings();
            cstr[0] = PROTOCOL_DELIM;
            cstr[1] = '1'; cstr[2] = 0;
            _rem_cnfg_tst_rsp( msg_src_idx, cstr);
            break;

        case PMTST_FUNC_SET_TRACE_MASK:
            // Get the trace mask
            // cptr points at ARG0
            // i = atol(cptr);
            
            if (temp16_2)
            {
                IsdTrace_SetMask( ISDTRACE_TTS_TO_ISD_MASK );
                IsdTrace_SetPort( msg_src_idx );
            }
            else
                IsdTrace_ClrMask();

            cstr[0] = PROTOCOL_DELIM;
            cstr[1] = 'O'; cstr[2] = 'K'; cstr[3] = 0;
            _rem_cnfg_tst_rsp( msg_src_idx, cstr);
            break;

        case PMTST_FUNC_GET_STATS:
            // Get the statistics from everyone and send out
            // a report
            // Get the statistics clear flag
            // $PMTST,(PMTST_FUNC_GET_STATS=9),(clear=0/1)*,(reports=0-3),(report stats)*
            // Msg=0: RS232 Stats:
            // 	USART A: Overrun Errors, Framing Errors, Parity Erors, Protocol Frame Errors
            // 	USART B: Overrun Errors, Framing Errors, Parity Erors, Protocol Frame Errors
            // Msg=1: Isd Stats:
            //	Voice Msg Timeouts
            //	Event Collisions
            //	Command Bad States
            // Msg=2: Tts Stats:
            //	TTS Watchdog Errors
            //	CME Events
            //	Buffer Full Errors
            //	Bad Events
            //	Bad Commands
            // 	Buffer 0 unacked commands
            // 	Buffer 1 unacked commands
            // Msg=3: Core Stats
            //	1 ms slip errors
            // Msg=4: FMT Stats
            //	Status: (OnAir, OffAir, Errored, NotPresent)
            //	Frequency: (16 bit int)
            //	CEX: (8 bit int)
            //	Power Level: (0.5, 1.0, 2.0 mW)
            //	Input Gain: (50, 100, 200 mV %100 modulation)
            ////////////////////////////////////////////////////////////////////////

            cstr[0] = PROTOCOL_DELIM;
            cstr[1] = 0;
            // Get the Clear Stats Flag
            // cptr points at ARG0
            i = cptr[0] & 0x0F;

            // Get the Msg Number
            cptr = RS232_GetToken( msg_src_idx, PMTST_ARG1_IDX );
            j = cptr[0] & 0x0F;

            switch ( j )
            {
                // 	USART A: Overrun Errors, Framing Errors, Parity Erors, Protocol Frame Errors
                case PMTST_STATS_RS232_A:	//RS232 A
                    RS232_GetStats( USARTA, i, cstr+1 );
                    break;
                // 	USART B: Overrun Errors, Framing Errors, Parity Erors, Protocol Frame Errors
                case PMTST_STATS_RS232_B: //RS232 B
                    RS232_GetStats( USARTB, i, cstr+1 );
                    break;
                // 	USART USB: Overrun Errors, Framing Errors, Parity Erors, Protocol Frame Errors
                case PMTST_STATS_RS232_USB: //RS232 USB
                    RS232_GetStats( USART_USB, i, cstr+1 );
                    break;                    

                // Event collisions, Serial Cmd Bad States, Bad signals, Bad Msgs
                case PMTST_STATS_ISD: // ISD
                    Isd_GetStats( i, cstr+1 );
                    break;

                case PMTST_STATS_TTS:	// TTS
                    Tts_GetStats( i, cstr+1 );
                    break;

                default:
                case PMTST_STATS_CORE:	// Core
                    Core_GetStats( i, cstr+1 );
                    break;

                case PMTST_STATS_FMT: //FMT
                    FMT_GetStats( i, cstr+1 );
                    break;

            }

            cstr[0] = PROTOCOL_DELIM;
            _rem_cnfg_tst_rsp( msg_src_idx, cstr);

            break;

        case PMTST_FUNC_SET_FMT_FREQ:
            // cptr points at ARG0
            // utemp16 = atol( cptr );
            FMT_SetFreq( (uint8)temp16_2 );

            cstr[0] = PROTOCOL_DELIM;
            cstr[1] = '1'; cstr[2] = 0;
            _rem_cnfg_tst_rsp( msg_src_idx, cstr);
            break;
            
#ifdef USE_PMTST_FUNC_SET_USART_SWITCHING
        case PMTST_FUNC_SET_USART_SWITCHING:
            // cptr points at ARG0
            // i = atol( cptr );		// Get the port number
            // Get the switching parms
            cptr = RS232_GetToken( msg_src_idx, PMTST_ARG1_IDX );
            j = atol(cptr);

            switch( temp16_2 )
            {
                case USARTA:

                    if (j & USARTA_BIT_MASK)
                    {
                        Btn_SetParm(BTN_PARM_XNG_LOOP_A_IDX, 1, 0);
                    }
                    else
                    {
                        Btn_SetParm(BTN_PARM_XNG_LOOP_A_IDX, 0, 0);
                    }
                    if (j & USARTB_BIT_MASK)
                    {
                        Btn_SetParm(BTN_PARM_XNG_A2B_IDX, 1, 0);
                    }
                    else
                    {
                        Btn_SetParm(BTN_PARM_XNG_A2B_IDX, 0, 0);
                    }
                    if (j & USART_USB_BIT_MASK)
                    {
                        Btn_SetParm(BTN_PARM_XNG_A2U_IDX, 1, 0);
                    }
                    else
                    {
                        Btn_SetParm(BTN_PARM_XNG_A2U_IDX, 0, 0);
                    }                      
                    break;

                case USARTB:

                    if (j & USARTA_BIT_MASK)
                    {
                        Btn_SetParm(BTN_PARM_XNG_B2A_IDX, 1, 0);
                    }
                    else
                    {
                        Btn_SetParm(BTN_PARM_XNG_B2A_IDX, 0, 0);
                    }
                    if (j & USARTB_BIT_MASK)
                    {
                        Btn_SetParm(BTN_PARM_XNG_LOOP_B_IDX, 1, 0);
                    }
                    else
                    {
                        Btn_SetParm(BTN_PARM_XNG_LOOP_B_IDX, 0, 0);
                    }
                    if (j & USART_USB_BIT_MASK)
                    {
                        Btn_SetParm(BTN_PARM_XNG_B2U_IDX, 1, 0);
                    }
                    else
                    {
                        Btn_SetParm(BTN_PARM_XNG_B2U_IDX, 0, 0);
                    }                    
                    break;
                    
                case USART_USB:

                    if (j & USARTA_BIT_MASK)
                    {
                        Btn_SetParm(BTN_PARM_XNG_U2A_IDX, 1, 0);
                    }
                    else
                    {
                        Btn_SetParm(BTN_PARM_XNG_U2A_IDX, 0, 0);
                    }
                    if (j & USARTB_BIT_MASK)
                    {
                        Btn_SetParm(BTN_PARM_XNG_U2B_IDX, 1, 0);
                    }
                    else
                    {
                        Btn_SetParm(BTN_PARM_XNG_U2B_IDX, 0, 0);
                    }
                    if (j & USART_USB_BIT_MASK)
                    {
                        Btn_SetParm(BTN_PARM_XNG_LOOP_USB_IDX, 1, 0);
                    }
                    else
                    {
                        Btn_SetParm(BTN_PARM_XNG_LOOP_USB_IDX, 0, 0);
                    }             
                    
                default:
                    break;
            }

            RS232_UpdateSwitching();

            cstr[0] = PROTOCOL_DELIM;
            cstr[1] = '1'; cstr[2] = 0;
            _rem_cnfg_tst_rsp( msg_src_idx, cstr);        			
            break;
#endif
            
        case PMTST_FUNC_SET_CNFG_FLASH_2_PARM:
            // cptr points at ARG0
            // i = atol( cptr );		// Get the parm number

            switch( temp16_2 ) // ARG0 is temp16_2
            {
                //$PMTST,PMTST_FUNC_SET_CNFG_FLASH_2_PARM,CNFG_FLASH_2_DEFAULT_MAG_VAR_PFL_IDX,+/-XXX.Y*
                case CNFG_FLASH_2_DEFAULT_MAG_VAR_PFL_IDX:

                    // Get the switching parms
                    cptr = RS232_GetToken( msg_src_idx, PMTST_ARG1_IDX );

                    temp16 = Core_StrToValue( cptr );

                    cstr[1] = temp16 >> 8;
                    cstr[2] = temp16 & 0xFF;
                    Cnfg_WriteCnfgFlash2Parm( CNFG_FLASH_2_DEFAULT_MAG_VAR_PFL_IDX, (uint8 *)cstr );
                    
                    cstr[0] = PROTOCOL_DELIM;
                    cstr[1] = '1'; cstr[2] = 0;
                    _rem_cnfg_tst_rsp( msg_src_idx, cstr);

                    Nmea_InitDefaultMagneticVariation();
                    break;

                    //$PMTST,PMTST_FUNC_SET_CNFG_FLASH_2_PARM,CNFG_FLASH_2_MSG_OPTIONS_PFL_IDX(ARG0),
                    //  Save to flash(ARG1), 
                    // HDM Talker ID (ARG2), HDM Msg Options(ARG3),
                    // HDT Talker ID (ARG2+2), HDT Msg Options(ARG3+2),
                    // HDG Talker ID (ARG2+4), HDG Msg Options(ARG3+4),
                    // RMB Talker ID (ARG2+6), RMB Msg Options(ARG3+6),
                    // RMC Talker ID (ARG2+8), RMC Msg Options(ARG3+8),
                    // VTG Talker ID (ARG2+10), VTG Msg Options(ARG3+10)
                case CNFG_FLASH_2_RS232_MSG_OPTIONS_PFL_IDX:
                    
                    j = RS232_GetTokenCnt( msg_src_idx );
                    if ( j == 16)
                        {
                        j = atol(RS232_GetToken( msg_src_idx, PMTST_ARG1_IDX ));  // get save to flash
                        p = (char *)Cnfg_GetCnfgFlash2ParmPtr( CNFG_FLASH_2_RS232_MSG_OPTIONS_PFL_IDX );
                        for ( i=0; i<6; i++)
                        {
                            cptr = RS232_GetToken( msg_src_idx, PMTST_ARG2_IDX+(i*2) ); // pointer to the talker id token
                            p += 4;
                            p[0] = cptr[0];   // load talker ID
                            p[1] = cptr[1];   // Load talker ID
                            cptr = RS232_GetToken( msg_src_idx, PMTST_ARG3_IDX+(i*2) ); // pointer to the msg options token
                            p[2] = (uint8)atol(cptr);
                        }
                        j = Cnfg_GetCnfgFlash2ParmChecksum( CNFG_FLASH_2_RS232_MSG_OPTIONS_PFL_IDX );
                        p = (char *)Cnfg_GetCnfgFlash2ParmPtr( CNFG_FLASH_2_RS232_MSG_OPTIONS_PFL_IDX );
                        p[0] = (uint8)j;
                        if (j)
                        {
                            Cnfg_WriteCnfgFlash2Parm( CNFG_FLASH_2_RS232_MSG_OPTIONS_PFL_IDX, 0 );
                        }
                        RS232_load_msg_options();
                        cstr[0] = PROTOCOL_DELIM;
                        cstr[1] = '1'; cstr[2] = 0;
                    }
                    else
                    {
                        cstr[0] = PROTOCOL_DELIM;
                        cstr[1] = '0'; cstr[2] = 0;
                    }
                    
                default:

                    _rem_cnfg_tst_rsp( msg_src_idx, cstr);
                    break;
            }
            break;	

        case PMTST_FUNC_GET_CNFG_FLASH_PARM:
            // cptr points at ARG0
            // i = atol( cptr );		// Get the parm number

            cstr[0] = PROTOCOL_DELIM;

            switch( temp16_2 )			// temp16_2 has ARG0
            {
                case CNFG_FLASH_2_DEFAULT_MAG_VAR_PFL_IDX:

                    if (Cnfg_IsCnfgFlash2ParmValid( CNFG_FLASH_2_DEFAULT_MAG_VAR_PFL_IDX ))
                    {
                        p = (char *)Cnfg_GetCnfgFlash2ParmPtr( CNFG_FLASH_2_DEFAULT_MAG_VAR_PFL_IDX );

                        temp16 = (p[1] << 8) | p[2];
                        Core_ValueToStr( temp16, cstr+1, 3 );
                    }
                    else
                    {
                        cstr[1] = '0';
                        cstr[2] = 0;
                    }
                    _rem_cnfg_tst_rsp( msg_src_idx, cstr);
                    break;

                    //$PMTST,PMTST_FUNC_GET_CNFG_FLASH_2_PARM,CNFG_FLASH_2_MSG_OPTIONS_PFL_IDX(ARG0),
                    // HDM Talker ID (ARG1), HDM Msg Options(ARG2),
                    // HDT Talker ID (ARG1+2), HDT Msg Options(ARG2+2),
                    // HDG Talker ID (ARG1+4), HDG Msg Options(ARG2+4),
                    // RMB Talker ID (ARG1+6), RMB Msg Options(ARG2+6),
                    // RMC Talker ID (ARG1+8), RMC Msg Options(ARG2+8),
                    // VTG Talker ID (ARG1+10), VTG Msg Options(ARG2+10)
                case CNFG_FLASH_2_RS232_MSG_OPTIONS_PFL_IDX:
                    
                    if (Cnfg_IsCnfgFlash2ParmValid( CNFG_FLASH_2_RS232_MSG_OPTIONS_PFL_IDX ))
                    {
                        p = (char *)Cnfg_GetCnfgFlash2ParmPtr( CNFG_FLASH_2_RS232_MSG_OPTIONS_PFL_IDX );
                        for ( i=0, j=1; i<6; i++)
                        {
                            p += 4;
                            cstr[j++] = p[0];   // load talker ID
                            cstr[j++] = p[1];   // Load talker ID
                            cstr[j++] = PROTOCOL_DELIM;
                            Core_IntToAsciiStr( (int)p[2], cstr+j, 3 );   // msg options
                            j += 3;
                            cstr[j++] = PROTOCOL_DELIM;
                        }
                        cstr[j++] = '1';
                        cstr[j] = 0;
                    }
                    else
                    {
                        cstr[1] = '0';
                        cstr[2] = 0;
                    }
                    _rem_cnfg_tst_rsp( msg_src_idx, cstr);
                    break;                        
              
                    
                default:
                    cstr[0] = PROTOCOL_DELIM;
                    cstr[1] = '1'; cstr[2] = 0;
                    _rem_cnfg_tst_rsp( msg_src_idx, cstr);
                    break;
            }
            break;	

        case PMTST_FUNC_SET_TTS_MUTE_PARM:
            // cptr points at ARG0
            // i = atol( cptr );		// Get the parm number

            switch( temp16_2 )			// temp16_2 has ARG0
            {
                default:
                case 0:	// TTS mute off
                    Tts_UnMute();
                    break;

                case 1: 	// TTS mute on
                    Tts_Mute(600);	// TTS mute for 600 seconds
                    break;
            }
            cstr[0] = PROTOCOL_DELIM;
            cstr[1] = '1'; cstr[2] = 0;
            _rem_cnfg_tst_rsp( msg_src_idx, cstr);
            break;

        case PMTST_FUNC_WRITE_I2C_REGISTER:
            #define i2c_addr i
            #define i2c_reg j
            #define i2c_reg_value k
            ///////////////////////////////////////////////////////////////////
            // void I2C_Set_RegValue( int8 SlaveAddr, int8 Reg, int8 Value )
            // 
            // Sends the msg to the slave in one atomic function
            ///////////////////////////////////////////////////////////////////
            // uint8 I2C_Set_RegValue( int8 SlaveAddr, int8 Reg, uint8 Value)
            // ARG0 = I2c address
            // ARG1 = I2c register
            // ARG2 = I2c register value
            i2c_addr = temp16_2;
            cptr = RS232_GetToken( msg_src_idx, PMTST_ARG1_IDX );
            i2c_reg = atol(cptr);
            cptr = RS232_GetToken( msg_src_idx, PMTST_ARG2_IDX );
            i2c_reg_value = atol(cptr);
            I2C_Set_RegValue( i2c_addr, i2c_reg, i2c_reg_value);

            break;

        case PMTST_FUNC_I2C_ADDR_CHK:
            ///////////////////////////////////////////////////////////////////
            // int8 I2C_SlaveAttached( int8 SlaveAddr )
            // 
            // Returns true if the slave is attached
            ///////////////////////////////////////////////////////////////////
            //int8 I2C_SlaveAttached( int8 SlaveAddr )
            cstr[0] = PROTOCOL_DELIM;
            i2c_addr = temp16_2;
            if (I2C_SlaveAttached(i2c_addr))
            {
                cstr[1] = '1'; 
            }
            else
            {
                cstr[1] = '0';
            }
            cstr[2] = 0;
            _rem_cnfg_tst_rsp( msg_src_idx, cstr);
            break;
    }
}
