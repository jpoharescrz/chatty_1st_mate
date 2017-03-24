/////////////////////////////////////////////////////////////////////////
////                          core_utils.c                              ////
////         
//// This file is included in core.c before any user includes                                                        ////
/////////////////////////////////////////////////////////////////////////
#include <xc.h>

#include <p24FJ128GB204.h>
#include <string.h>
#include <stdlib.h>
#include "core_api.h"
#include "rs232_api.h"

uint16 AbsInt16( int16 val )
{
	if (val < 0)
		val *= -1;
			
	return( val );
}

int8 MinInt8( int8 a, int8 b )
{
 if (a < b) 
  return( a );
 else
  return( b );
}

int8 MaxInt8( int8 a, int8 b )
{
 if (a > b) 
  return( a );
 else
  return( b );
}

int16 MinInt16( int16 a, int16 b )
{
 if (a < b) 
  return( a );
 else
  return( b );
}

int16 MaxInt16( int16 a, int16 b )
{
 if (a > b) 
  return( a );
 else
  return( b );
}

int16 AddModulo( int16 initial_value, int16 add1, int16 modulo)
{	 
 signed int16 sum;

     while ( add1 > modulo ) 
       add1 -= modulo;

     sum = initial_value;

     sum += add1;
     // wrap if overflowed
     if ( sum > (modulo-1) )
     {
      sum -= modulo;
     }
     else
     {
      if ( sum < 0 )
      {
       sum += modulo;
      }
     }
     return( sum );
}

int16 SubModulo( int16 initial_value, int16 sub1, int16 modulo)
{	 
 signed int16 sum;

     while ( sub1 > modulo ) 
       sub1 -= modulo;

     sum = initial_value;

     sum -= sub1;
     // wrap if underflowed
     if ( sum < 0 )
     {
      sum += modulo;
     }
     else
     {
      if ( sum > (modulo-1) )
      {
       sum -= modulo;
      }
     }
     return( sum );
}

// returns the difference of two numbers in a modulus
// the difference can't be greater than half the modulus
// or less than half the negative modulus
// if it is then the function assumes that a counter-sweep
// has occurred and adjusts the difference
int16 Core_get_modulo_difference(  int16 p0, int16 p1, int16 mod )
{
 int16 dif, half_mod;

	dif = p0-p1;
	half_mod = (mod >> 1);
	
  	// try to detect a wrap of a modulus number
  	if (mod)
  	{
	  	// if the difference is greater than half the modulus
	  	if (dif > half_mod)
	  	{
		  	dif = -1*(half_mod - (dif - half_mod));	
		}
	  	// if the difference is less than half the minus modulus
	  	if (dif < -1*half_mod)
	  	{
		  	dif = -1*dif;
		  	dif = half_mod - (dif - half_mod);	
		}
  	}

  	return( dif );
}

int16 Core_get_modulo_abs_difference( int16 p0, int16 p1, int16 mod )
{
 int16 dif;

 	dif = Core_get_modulo_difference( p0, p1, mod );

 	if ( dif < 0 )
 	{
  		dif = dif * -1;
 	}

 	return( dif );
}

#define SubtractAbs Core_get_modulo_abs_difference

#define i8toA int8_to_ascii10
// Faster byte-to-acsii conversion routine
char *int8_to_ascii10( unsigned int8 num, char *ptr)
{
 short index=0;
 unsigned int8 number;

 	number = num;

 	// First get the hundreds
 	if (num > 99)
 	{
  		if (num < 200)
  		{
   			ptr[index++] = '1'; number -= 100; 
  		}
  		else // it's in the 200's
  		{
   			ptr[index++] = '2'; number -= 200;
  		}
  		if (number < 10)
   			ptr[index++] = '0';
 	}

 	// Now get the tens
 	if (number < 50)
 	{
  		if (number < 30)
  		{
   			if (number < 20)
   			{
    			if (number > 9) { ptr[index++] = '1'; number -= 10; }
   			}
   			else // it's in the 20s
   			{ ptr[index++] = '2'; number -= 20; }
  		}
  		else // it's in the 30s/40s
  		{
   			if (number < 40) { ptr[index++] = '3'; number -= 30; }
   			else { ptr[index++] = '4'; number -= 40; }
  		}
 	}
 	else // 50 or more
 	{
  		if (number < 80)
  		{
   			if (number < 60) { ptr[index++] = '5'; number -= 50; }
   			else
   			{  
    			if (number < 70) { ptr[index++] = '6'; number -= 60; }
    			else { ptr[index++] = '7'; number -= 70; }
   			}
  		}
  		else  // it's 80/90
  		{ 
   			if (number < 90 ) { ptr[index++] = '8'; number -= 80; }
   			else { ptr[index++] = '9'; number -= 90; }
  		}
 	} 

 	// Now do the ones
 	ptr[index++] = number | 0x30;
 	ptr[index] = 0;
 	ptr = &(ptr[index]);
 	return( ptr );
}

////////////////////////////////////////////////////////////////////////
// void write_flash_word( int32 flash_address, int32 data_32)
//
// Writes an instruction to flash
// Upper byte of data_32 is lost
// - flash_addr is a 32 bit number
// - data_32 is a 32 bit number
//
// Flash addresses instructions on 16 bit boundaries. The lower 16 bits
// of an instruction is on an even boundary. The upper 16 bits is on an
// odd boundary (+1) but the upper byte is ignored.
////////////////////////////////////////////////////////////////////////
int32 addr_page, addr_offset;
int32 data_hi, data_lo;

////////////////////////////////////////////////////////////////////////
// void erase_flash_memory_block( int32 flash_address )
//
// Erases 512 instructions (1024 memory locations, 1536 bytes)
// - flash_addr is a 32 bit number
//
////////////////////////////////////////////////////////////////////////
void erase_flash_memory_block( int32 flash_address )
{

 addr_page = (flash_address >> 16);
 addr_offset = (flash_address & 0x0000FFFF);

//#asm
 
 asm ("mov	#0x4042,W0");
 asm ("mov	W0,NVMCON");
 
 asm ("mov  _addr_page,W0");
 asm ("mov	W0,TBLPAG");
 asm ("mov  _addr_offset,W0");
 asm ("tblwtl	W0,[W0]");

 asm ("disi	#5");
 asm ("mov	#0x55,W0");
 asm ("mov	W0,NVMKEY");
 asm ("mov	#0xAA,W0");
 asm ("mov	W0,NVMKEY");
 asm ("bset	NVMCON,#15");
 asm ("nop");
 asm ("nop");
// #endasm
}

void reset_cpu()
{
	asm("RESET");	
}

unsigned int32 make8_32( uint8 byte3, uint8 byte2, uint8 byte1, uint8 byte0 )
{
 unsigned int32 data_32;

  data_32 = byte0; 
  data_32 |= (uint32)byte1 << 8;
  data_32 |= (uint32)byte2 << 16;
  data_32 |= (uint32)byte3 << 24;

  return( data_32 );
}


/////////////////////////////////////////////////////////////////
// char *strhaschar( char *ptr, char c )
// Returns a pointer to the part of the string pointed to by
// ptr which has the character spec'd in c.
// Returns 0x0000 if c not found.
/////////////////////////////////////////////////////////////////
char *strhaschar( char *ptr, char c )
{
 int8 i;

 i = strlen( ptr );
 while (i)
 { 
  if ( *ptr == c ) return(ptr);
  i--; ptr++;
 }
 return(0);
}

/////////////////////////////////////////////////////////////////
// int16 Core_Round( int16 val, int8 up, int8 interval, int16 mod)
//
// Rounds val up/down to interval and mod
//
// if mod is non-zero then a round-up will be modulo(mod)
// 
/////////////////////////////////////////////////////////////////
int16 Core_Round( int16 val, int8 up, int16 interval, int16 mod)
{
 int16 temp16;

	//first, round down
    if (interval)
        temp16 = (val/interval)*interval;
    else
        return( val );
    
    if ((up) && (temp16 != val))
    {
	 // round up
     temp16 = temp16 + interval;
     if ((mod) && (temp16 >= mod) )
       temp16 = temp16 - mod;
    }  
    return( temp16 );
}

//////////////////////////////////////////////////////////////////
// char *IntToAsciiStr( int16 i, char *str, int8 min_digs )
//
// Converts the positive integer i to an ascii string
//////////////////////////////////////////////////////////////////
#define ITOA_LIMIT 1000000
#define ITOA_MAX_WIDTH 7
char *Core_IntToAsciiStr( long i, char *str, int8 min_digs )
{
 unsigned long limit;
 int8 digits;
 char *cptr;
 
 // Get the max limit
 //for(limit=1,j=ITOA_MAX_WIDTH-1; j>0; j--)
 //    limit *= 10;
 limit = ITOA_LIMIT;

 // handle a negative
 cptr = str;
 if (i<0)
 {
     i = i*-1;
     *cptr++ = '-';
 }
 
 // Get the digit count and limit
 for( digits = ITOA_MAX_WIDTH; (i < limit) && (digits > min_digs); digits-- )
 {
     limit = limit / 10;
 }
 
 if (i>((limit*10)-1)) i = ((limit*10)-1);

 // construct the string
 for(; digits>0; digits--)
 {
     *cptr++ = (i / limit) | 0x30;
     i -= (i /limit)*limit;
     limit = limit / 10;
 }
 *cptr= 0;
 
 return( str );
}
/////////////////////////////////////////////////////////////////
// Useful lookup tables
/////////////////////////////////////////////////////////////////
//Const Byte tenths_to_eighths[] = 
//{0,1,2,2,3,4,5,6,6,7,7,7,7,7,7,7};
const char tenths_to_sixteenths[] = 
{0,1,3,4,6,8,9,0x0B,0x0C,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E};
const char sixteenths_to_tenths[] = 
{0,1,1,2,3,3,4,4,5,6,6,7,8,8,9,9};
const int16 hi_rez_decimals[] = 
{6553,655,66,6,0};
/////////////////////////////////////////////////////////////////
// char *Core_ValueToStr( int16 value, char *cptr, int8 min_digs )
//
// Converts a formatted value to an ascii string
// int(12 bits):fraction(4 bits)
// cptr points at the callers buffer to write the string
// min_digs specs a minimum number of integer digits
/////////////////////////////////////////////////////////////////
char *Core_ValueToStr( int16 value, char *cptr, int8 min_digs )
{
 int16 int_value, dec_value;
 short i;
 char *tptr;

 tptr = cptr;

 int_value = value >> 4 ;
 dec_value = value & 0x000F;
 if (int_value < 0)
 {
  *cptr++ = '-'; // prepend a minus sign
  int_value = (int_value | 0xF000) * -1;
  dec_value = (dec_value | 0xFFF0) * -1;
 }

 cptr = Core_IntToAsciiStr( (long)int_value, cptr, min_digs );
 i = strlen(cptr);
 cptr[i] = '.';
 cptr[i+1] = sixteenths_to_tenths[ dec_value & 0x000F ] | 0x30;
 cptr[i+2] = 0;
 return( tptr );
}

/////////////////////////////////////////////////////////
// int16 Core_StrToValue(char *cptr)
//
// Take a pointer to an ascii string number and
// returns low precision 16 bit value
// upper 12 bits for integer, lower four bits for 16ths
/////////////////////////////////////////////////////////
int16 Core_StrToValue(char *cptr)
{
 char *dptr, *bptr;
 int16 i, temp16 = 0;

	// check for decimal point, returns 0 if not there
    dptr = strchr( cptr, '.' );

    // if it's potentially a float
    if (dptr)
    {
    	dptr[0] = 0;         			// Null terminate integer portion
        
        if (cptr[0] == '-')             // if it's a negative
            bptr = cptr+1;              // ignore sign for now
        else
            bptr = cptr;
        
      	temp16 = atol( bptr );          // Convert the integer portion
      	temp16 = (temp16 << 4);   		// make room for sixteenths of whole
      	i = dptr[1] & 0x0F;      		// Get the tenths
      	temp16 |= tenths_to_sixteenths[i];   // Convert tenths to sixteenths
        if (cptr[0] == '-')             // if it's a negative
            temp16 *= -1;               // flip it
        
      	dptr[0] = '.';					// restore decimal point
     }
     else // no decimal point in expected position
     {
      	temp16 = atol( cptr );     // Convert the integer portion
      	temp16 = (temp16 << 4);   		// make room for sixteenths of whole (=0 here)
     }
	return( temp16 );
}

/////////////////////////////////////////////////////////////////
// char *Core_HiResValueToStr( int32 value, char *cptr, int8 min_digs )
//
// Converts a Hi Res formatted value to an ascii string
// int(16 bits):fraction(16 bits)
// cptr points at the callers buffer to write the string
// min_digs specs a minimum number of integer digits
/////////////////////////////////////////////////////////////////
char *Core_HiResValueToStr( int32 value, char *cptr, int8 min_int_digs, int8 min_dec_digs )
{
 int32 int_value, dec_value, temp32;
 short i, k;
 char *tptr;

 tptr = cptr;

 int_value = value >> 16 ;
 dec_value = value & 0xFFFF;
 
 if (int_value < 0)
 {
  *cptr++ = '-'; // prepend a minus sign
  int_value = (int_value | 0xFFFF0000) * -1;
  dec_value = (dec_value | 0xFFFF0000) * -1;
 }

 cptr = Core_IntToAsciiStr( int_value, cptr, min_int_digs );
 i = strlen(cptr);
 if (min_dec_digs > 0 )
 {    
     cptr[i++] = '.';

     for(k = 0; (k<min_dec_digs) && (k<4); k++)
     {
        temp32 = dec_value / hi_rez_decimals[k];
        cptr[i++] = (char)(temp32 | 0x30);
        dec_value -= temp32 * hi_rez_decimals[k];
     }
 }
 cptr[i] = 0;

 return( tptr );
}

/////////////////////////////////////////////////////////
// int32 Core_StrToHiResValue(char *cptr)
//
// Take a pointer to an ascii string number and
// returns high precision 32 bit value
// upper 16 bits for integer, lower 16 bits for decimals
/////////////////////////////////////////////////////////
int32 Core_StrToHiResValue(char *cptr)
{
 char *dptr, *bptr;
 int16 i,j,k;
 int32 temp32 = 0;

	// check for decimal point, returns 0 if not there
    dptr = strchr( cptr, '.' );

    // if it's potentially a float
    if (dptr)
    {
    	dptr[0] = 0;         			// Null terminate integer portion
        
        if (cptr[0] == '-')             // if it's a negative
            bptr = cptr+1;              // ignore sign for now
        else
            bptr = cptr;
        
      	temp32 = atol( bptr );          // Convert the integer portion
      	temp32 = (temp32 << 16);   		// make room for decimal
        
        for(k = 0, i=1; dptr[i] && (k<4); k++, i++)
        {
            j = dptr[i] & 0x0F;      		// Get the tenths
            j *= hi_rez_decimals[k];
            temp32 += j;
        }
        
        if (cptr[0] == '-')             // if it's a negative
            temp32 *= -1;               // flip it
        
      	dptr[0] = '.';					// restore decimal point
     }
     else // no decimal point in expected position
     {
      	temp32 = atol( cptr );          // Convert the integer portion
      	temp32 = (temp32 << 16);   		// make room for decimal
     }
	return( temp32 );
}

void bit_clear(uint16 *k, uint8 k_bit )
{
	*k = *k ^ (*k & (1 << k_bit)); 	
}

void bit_set(uint16 *k, uint8 k_bit )
{
	*k = *k | (1 << k_bit); 	
}

void Delay_us( uint16 dly_us )
{
 uint16 i;
 
 while (dly_us--)
 {
  for (i=10; i; i--)	 
   {;}
 }
	
}

void Delay_ms( uint16 dly_ms )
{
 uint16 i;
 
 while (dly_ms--)
 {
  for (i=1024; i; i--)	 
   {;}
 }
	
}

void read_program_memory( uint32 bptr, uint8 *buffer, int bytecnt )
{
 int i;
 uint32 data_32;
 uint8 *p;
 
 	p = buffer;
 
 	for( i = 0; i < bytecnt; )
 	{
		 data_32 = read_flash_word( bptr );
		 p[i] = data_32 & 0x000000FFL;
		 p[i+1] = (data_32 >> 8) & 0x000000FFL;
		 p[i+2] = (data_32 >> 16) & 0x000000FFL;
		 p[i+3] = (data_32 >> 24) & 0x000000FFL;
		 i += 4; bptr += 2;
 	}	
	
}

////////////////////////////////////////////////////////////////////////
// void write_flash_buffer( int32 flash_addr, int8 *dptr, uint16 count)
//
// Writes a buffer to flash, every fourth byte (dptr[i+3]) is wasted
// - flash_addr is a 32 bit number
// - dptr points at a byte array
// - count is a byte count
//
////////////////////////////////////////////////////////////////////////
void write_flash_buffer( uint32 flash_addr, uint8 *dptr, uint16 count)
{
 int8 i;
 unsigned int32 data_32, addr_32;
 unsigned int8 *data_ptr;

 data_ptr = dptr;
 addr_32 = flash_addr;
 for( i = 0; i<count; i += 4)
 {
  data_32 = make8_32( 0, data_ptr[2], data_ptr[1], data_ptr[0] );
  data_ptr += 4;
   
  write_flash_word( addr_32, data_32);
  addr_32 += 2;
 }
} 

void write_flash_word( int32 flash_address, int32 data_32)
{

 addr_page = (flash_address >> 16);
 addr_offset = (flash_address & 0x0000FFFF);
 data_hi = data_32 >> 16;
 data_lo = data_32 & 0x0000FFFF;

 //#asm
 asm ("mov  _addr_page,W0");
 asm ("mov	W0,TBLPAG");
 asm ("mov	_data_lo,W2");
 asm ("mov	_data_hi,W3");
 asm ("mov	_addr_offset,W0");
 asm ("nop");
 asm ("nop");
 asm ("tblwtl	W2,[W0]");
 asm ("tblwth	W3,[W0]");

 asm ("mov	#0x4003,W0");
 asm ("mov	W0,NVMCON");

 asm ("disi	#5");
 asm ("mov	#0x55,W0");
 asm ("mov	W0,NVMKEY");
 asm ("mov	#0xAA,W0");
 asm ("mov	W0,NVMKEY");
 asm ("bset	NVMCON,#15");
 asm ("nop");
 asm ("nop");
 //#endasm
}

int32 read_flash_word( int32 flash_address )
{
 uint32 data_32;

 addr_page = (flash_address >> 16);
 addr_offset = (flash_address & 0x0000FFFF);

 //#asm
 asm ("mov  	_addr_page,W0");
 asm ("mov		W0,TBLPAG");
 asm ("mov		_addr_offset,W0");
 asm ("nop");
 asm ("nop");
 asm ("tblrdl	[W0],W2");
 asm ("tblrdh	[W0],W3");

 asm ("mov		W3,_data_hi");
 asm ("mov		W2,_data_lo");
 //#endasm
 
 data_32 = data_hi << 16;
 data_32 |= data_lo & 0x0000FFFFL;
 
 return( data_32 );
}

////////////////////////////////////////////////////////////////////////
// void _stat_append( int16 i, char *p)
//
// Takes the stat, i, limits it to 999, converts it to ascii, appends it
// to the buffer pointed to by p, and then  appends a PROTOCOL_DELIM before
// terminating it.
//
// char *Core_IntToAsciiStr( int16 i, char *str, int8 min_digs )
////////////////////////////////////////////////////////////////////////
#define STATS_MAX_WIDTH 7
void _stat_append( unsigned long i, char *p, int8 append_delim, int8 width)
{
 char StatBuf[STATS_MAX_WIDTH+1];
 int j;
 long max;

    for( max = 1, j=0; j<width; j++)
        max *= 10;
 
    max--;
    if ((width > 1) && (width <= STATS_MAX_WIDTH))
    {
        if (i > max) i = max;
        j = width;
        Core_IntToAsciiStr( (long) i, StatBuf, j );
        if (append_delim)
            StatBuf[j++] = PROTOCOL_DELIM; 
        StatBuf[j] = 0;
        strcat( p, StatBuf);
    }
}

char VersionStr[5];
char *Core_GetVersion()
{
 
	strcpy(VersionStr, Version);
	return(VersionStr);
}

void piezo_on( uint8 on )
{
	//if (on)
	//{
	//	PIEZO_PORT |= (1 << PIEZO_BIT);	
	//}	
	//else
	//{
	//	PIEZO_PORT &= (1 << PIEZO_BIT) ^ 0xFFFF;
	//}
}

void debug_pulse_on( uint8 on )
{
	//if (on)
	//{
	//	DEBUG_PULSE_PORT |= (1 << DEBUG_PULSE_BIT);	
	//}	
	//else
	//{
	//	DEBUG_PULSE_PORT &= (1 << DEBUG_PULSE_BIT) ^ 0xFFFF;
	//}
}

void UnLockPIO()
{
	//#ASM
 	asm ("MOV   OSCCON,W1");
 	asm ("MOV   #0x46,W2");
 	asm ("MOV   #0x57,W3");
 	asm ("MOV   W2,[W1]");
 	asm ("MOV   W3,[W1]");
 	asm ("BCLR  OSCCON,#6");
	//#ENDASM
}

void LockPIO()
{
	//#ASM
 	asm ("MOV   OSCCON,W1");
 	asm ("MOV   #0x46,W2");
 	asm ("MOV   #0x57,W3");
 	asm ("MOV   W2,[W1]");
 	asm ("MOV   W3,[W1]");
 	asm ("BSET  OSCCON,#6");
 	
	//#ENDASM
}

//////////////////////////////////////////////////////////////////////
//Resource lock/unlock utilities
//
// Defines structures, functions, and resources for locking/unlocking
// The resources requiring locks are defined in core_reg.h
//////////////////////////////////////////////////////////////////////
//#define RSRC_I2C 0
//#define RSRC_POTS 1
//#define RSRC_TONE 2

//#define NUM_RESOURCES 1

#define RSRC_UNLOCKED 0xFF

#if NUM_RESOURCES > 0
uint8 ResourceLocks[ NUM_RESOURCES ];
#endif

void RsrcInit()
{
#if NUM_RESOURCES > 0
 int i;

 	for(i=0; i < NUM_RESOURCES; i++)
 	{
   		ResourceLocks[i] = RSRC_UNLOCKED;
  	} 	
#endif
}

#if NUM_RESOURCES > 0
//////////////////////////////////////////////////////////////////////
// int8 RsrcIsLockFree( int8 Resource )
//
// Returns TRUE if resource is not locked and FALSE otherwise
//////////////////////////////////////////////////////////////////////
int8 RsrcIsLockFree( int Resource )
{
 	if ( ResourceLocks[ Resource ] == RSRC_UNLOCKED )
  		return( TRUE );
 	else
  		return( FALSE );
}

//////////////////////////////////////////////////////////////////////
// int8 RsrcSetLockOwner( int8 Resource, int8 Owner )
//
// Attempts to lock a resource with a spec'd owner
// if the lock is free the owner gets it and the routine returns TRUE
// Otherwise, someone else has it and it returns FALSE
//////////////////////////////////////////////////////////////////////
int8 RsrcSetLockOwner( int Resource, int8 Owner )
{
 	if ( ResourceLocks[ Resource ] == RSRC_UNLOCKED )
 	{
  		ResourceLocks[ Resource ] = Owner;
  		return( TRUE );
 	}
 	else
 	if ( ResourceLocks[ Resource ] != Owner )
 	{
  		return( FALSE );
  	}	
  	return ( TRUE );
}
 
//////////////////////////////////////////////////////////////////////
// int8 RsrcGetLockOwner( int8 Resource )
//
// Returns the owner of the lock.
// if the lock is free the routine returns RSRC_UNLOCKED
//////////////////////////////////////////////////////////////////////
int8 RsrcGetLockOwner( int Resource )
{
  	return( ResourceLocks[ Resource ] );
}

//////////////////////////////////////////////////////////////////////
// int8 RsrcFreeLock( int8 Resource )
//
// Frees the lock.
// The routine sets the owner to RSRC_UNLOCKED
//////////////////////////////////////////////////////////////////////
void RsrcFreeLock( int Resource )
{
 	ResourceLocks[ Resource ] = RSRC_UNLOCKED;
}
#endif

