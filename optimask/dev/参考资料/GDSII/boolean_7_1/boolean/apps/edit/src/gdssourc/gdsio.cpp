/*
Program	GDSIO.CPP
Purpose	Used by GDS driver for conversions and read en write
		GDSII elements
Programmers	R. Spekreijse & N. Noorlander
Last Update	19-09-1995
*/

#ifdef __GNUG__
#pragma implementation
#endif
#include "gdsio.h"
#include <iostream>

#include "wxwin.h"

// ------------- input functions -----------------

// a byte
//Gdsio Gdsio::&operator>>(Gdsio &my_stream, G_BYTE *my_byte)
Gdsio& Gdsio::operator>>(G_BYTE *my_byte)
{
	if (this->eof())
		throw GDS_Error("Unexpected end of file", "GDS Read Error", 0, 1);
	else
		*my_byte = this->get();
	return *this;
}


// 2 bytes UNsigned integer
Gdsio& Gdsio::operator>>(TWO_G_BYTE_UNSIGNED_INTEGER *my_integer)
{
	a_union_value a_value;
	a_value.eight_byte_real=0;
	if (this->eof())
		throw GDS_Error("Unexpected end of file", "GDS Read Error", 0, 1);
	else
#if wxBYTE_ORDER == wxBIG_ENDIAN
		a_value.array.array[0] = this->get();
#else
        a_value.array.array[1] = this->get();
#endif 

	if (this->eof())
		throw GDS_Error("Unexpected end of file", "GDS Read Error", 0, 1);
	else
#if wxBYTE_ORDER == wxBIG_ENDIAN
      a_value.array.array[1] = this->get();
#else
      a_value.array.array[0] = this->get();
#endif 

	*my_integer = a_value.two_byte_signed_integer;
	return *this;
}


// 2 bytes signed integer
Gdsio& Gdsio::operator>>(TWO_G_BYTE_SIGNED_INTEGER *my_integer)
{
	a_union_value a_value;
	a_value.eight_byte_real=0;

	if (this->eof())
		throw GDS_Error("Unexpected end of file", "GDS Read Error", 0, 1);
	else
#if wxBYTE_ORDER == wxBIG_ENDIAN
      a_value.array.array[0] = this->get();
#else
      a_value.array.array[1] = this->get();
#endif 
	if (this->eof())
		throw GDS_Error("Unexpected end of file", "GDS Read Error", 0, 1);
	else
#if wxBYTE_ORDER == wxBIG_ENDIAN
      a_value.array.array[1] = this->get();
#else
      a_value.array.array[0] = this->get();
#endif 
	*my_integer = a_value.two_byte_signed_integer;
	return *this;
}



// 4 byte signed integer
Gdsio& Gdsio::operator>>(FOUR_G_BYTE_SIGNED_INTEGER *my_integer)
{
	a_union_value a_value;

	// reads a GdsII variable and convers to a dos-variable
	// the variable consist of 4 bits, numbered 0 1 2 3 , this
	// will be converted to the following sequence: 3 2 1 0
#if wxBYTE_ORDER == wxBIG_ENDIAN
	for(short int i=0; i<=3; i++)
#else
   for(short int i=3; i>=0; i--)
#endif

		if (this->eof())
			throw GDS_Error("Unexpected end of file", "GDS Read Error", 0, 1);
		else
            a_value.array.array[i] = this->get();

	*my_integer = a_value.four_byte_signed_integer;
	return *this;
}

// 4 byte byte real
Gdsio& Gdsio::operator>>(FOUR_G_BYTE_REAL *my_real)
{
	a_union_value a_value;

	// reads a GdsII variable and convers to a dos-variable
	// the variable consist of 4 bits, numbered 0 1 2 3 , this
	// will be converted to the following sequence: 3 2 1 0
#if wxBYTE_ORDER == wxBIG_ENDIAN
	for(short int i=0; i<=3; i++)
#else
   for(short int i=3; i>=0; i--)
#endif
		if (this->eof())
			throw GDS_Error("Unexpected end of file", "GDS Read Error", 0, 1);
		else
            a_value.array.array[i] = this->get();

	// NOTE: in the original written code in C, a four byte real isn't convert
	//       like a eight byte real. Cause a four byte real isn't use in the
	//       gdsii, there's no problem. maybe in future versions, a four byte
	//       real must also converted. (the function gds2double must be changed
	//       for using 4 bytes instead of 8).

	*my_real = a_value.four_byte_real;
	return *this;
}


// 8 byte real
Gdsio& Gdsio::operator>>(EIGHT_G_BYTE_REAL *my_real)
{
	a_union_value a_value;
	short int	i;

	// reads a GdsII variable and convers to a dos-variable
	// Reverse ordering will happen in the function gds2double

	for(i=0; i<8; i++)
		if (this->eof())
			throw GDS_Error("Unexpected end of file", "GDS Read Error", 0, 1);
		else
            a_value.array.array[i] = this->get();

	// convert gdsii-real into a machine double
	this->gds2double(a_value.array.array);

	*my_real = a_value.eight_byte_real;
	return *this;
}


// -------------- output -----------------

// a byte
Gdsio& Gdsio::operator<<(G_BYTE my_byte)
{
	this->put(my_byte);
	return *this;
}

// 2 bytes UNsigned integer
Gdsio& Gdsio::operator<<(TWO_G_BYTE_UNSIGNED_INTEGER my_integer)
{
	a_union_value a_value;
	a_value.two_byte_signed_integer = my_integer;
#if wxBYTE_ORDER == wxBIG_ENDIAN
	this->put(a_value.array.array[0]);
	this->put(a_value.array.array[1]);
#else
	this->put(a_value.array.array[1]);
	this->put(a_value.array.array[0]);
#endif
	return *this;
}

// 2 bytes signed integer
Gdsio& Gdsio::operator<<(TWO_G_BYTE_SIGNED_INTEGER my_integer)
{
	a_union_value a_value;
	a_value.two_byte_signed_integer = my_integer;
#if wxBYTE_ORDER == wxBIG_ENDIAN
	this->put(a_value.array.array[0]);
	this->put(a_value.array.array[1]);
#else
	this->put(a_value.array.array[1]);
	this->put(a_value.array.array[0]);
#endif
	return *this;
}


// 4 byte signed integer
Gdsio& Gdsio::operator<<(FOUR_G_BYTE_SIGNED_INTEGER my_integer)
{
	a_union_value a_value;
	a_value.four_byte_signed_integer = my_integer;

	// writes a GdsII variable and convers to a dos-variable
	// the variable consist of 4 bits, numbered 0 1 2 3 , this
	// will be converted to the following sequence: 3 2 1 0

#if wxBYTE_ORDER == wxBIG_ENDIAN
	for(short int i=0; i<=3; i++)
#else
	for(short int i=3; i>=0; i--)
#endif
		this->put(a_value.array.array[i]);

	return *this;
}


// 4 byte byte real
Gdsio& Gdsio::operator<<(FOUR_G_BYTE_REAL my_real)
{
	a_union_value a_value;
	a_value.four_byte_real = my_real;

	// writes a GdsII variable and convers to a dos-variable
	// the variable consist of 4 bits, numbered 0 1 2 3 , this
	// will be converted to the following sequence: 3 2 1 0
#if wxBYTE_ORDER == wxBIG_ENDIAN
	for(short int i=0; i<=3; i++)
#else
	for(short int i=3; i>=0; i--)
#endif
		this->put(a_value.array.array[i]);

	return *this;
}


// 8 byte real
Gdsio& Gdsio::operator<<(EIGHT_G_BYTE_REAL my_real)
{
	a_union_value a_value;
	a_value.eight_byte_real = my_real;

	short int	i;

	// convers the machine double into GdsII-real
	this->double2gds(a_value.array.array);

	// writes a GdsII variable and convers to a dos-variable
	// Reverse ordering will happen in the function gds2double
	for(i=0; i<8; i++)
		this->put(a_value.array.array[i]);

	return *this;
}


/*
To convert GDSII doubles to machine doubles :
void gds_double( fb)

To convert machine doubles to GDSII doubles :
void double_gds( fb)


GDSII floating point representation (8-byte REAL)

SEEEEEEE MMMMMMMM MMMMMMMM MMMMMMMM
MMMMMMMM MMMMMMMM MMMMMMMM MMMMMMMM

Sign         = S=1 negatif S=0 positif
Expo         = (E-64)^16
Mantisse     = (1/16 <= M < 1)
Double value = (-1)^S * Expo * Mantisse
Zero         = all bits of M zero

unix (Apollo) or wxBIG_ENDIAN (8-byte REAL)

SEEEEEEE EEEEMMMM MMMMMMMM MMMMMMMM
MMMMMMMM MMMMMMMM MMMMMMMM MMMMMMMM

Sign         = S=1 negatif S=0 positif
Expo         = (E-1023)^2
Mantisse     = (1.M)binary
Double value = (-1)^S * Expo * Mantisse
Zero         = E=0 and M=0
+-Infinit    = E=2047 and M=0

notice:
	a wxUint32 in turbo C =  in memory for instance like this
	m=40 m+1=99 m+2=94 m+3=93

	then    (long) & 0xFF000000 gives the result
	m=40 m+1=99 m+2=94 m+3=00

	in most other machine this is
	m=00 m+1=99 m+2=94 m+3=93

	all of this is due to that the bytes of types are in reverse order
	in memory.
*/


// convert a gds double to a machine double
// input
// 	arguments  : unsigned char fb[]  8byte array with gds double
// output
// 	arguments  : unsigned char fb[]  8byte array with machine double
void Gdsio::gds2double(unsigned char fb[])
{
	union bldun
	{
		unsigned char by[8];
		wxUint32 fl[2];
		double machdb;
	} bld;

	int j;
	wxUint32 carry, e, mantissa[2], s;
#	if defined(ultrix) || wxBYTE_ORDER == wxLITTLE_ENDIAN
	wxUint32 swap;
#	endif

#if wxBYTE_ORDER == wxLITTLE_ENDIAN
	// for turbo c real,integer,doubles are saved in memory
	// in reverse order, so first reverse bytes
	for (j = 0; j < 8; j++)
		bld.by[j]=fb[7-j];
#else
	for (j = 0; j < 8; j++)
		bld.by[j]=fb[j];
#endif


#ifdef vax
	if( bld.fl[0] == 0 && bld.fl[1] == 0) return;

	// mask highest bit (bit 32) and shift to bit1
	s = (bld.fl[1] & 0x80000000L) >> 31;
	// same for exponent bits
	e = (bld.fl[1] & 0x7F000000L) >> 24;

	// mask mantisse fo GDS double
	mantissa[0] = bld.fl[1] & 0x00FFFFFFL;
	mantissa[1] = bld.fl[0];

	// 2^(4*(E-64))=GDStrue exp  +128 for VAX
	e = 4 * (e - 64) + 128;
	if (mantissa[0] == 0 && mantissa[1] == 0 && e != 0)
	{
		// the number = zero
		bld.fl[1] = 0;
		// result back in byte array
		for (j = 0; j < 8; j++)
			fb[j]=bld.by[j];
		return;
	}

	// change mant. to 0.1mant
	while( mantissa[0] < 0x00800000L )
	{
		// times 2
		carry = mantissa[1] >> 31;
		// carry to next four bytes
		mantissa[0] = mantissa[0] << 1 | carry;
		// bit not available become zero
		mantissa[1] = mantissa[1] << 1;

		// without loss in accurancy
		// so lower exponent by one
		e--;
	}

	// mantisse is now 0.1mant
	//  - mask highest redundant bit of mantisse
	//  - swap word (2 bytes) positions
	//  - put sign and exponent on right place
	bld.fl[0] = (mantissa[0] & 0x007F0000L) >> 16 |
		(mantissa[0] & 0x0000FFFFL) << 16 |
		s << 15 | e << 7;

	// swap word positions
	bld.fl[1] = mantissa[1] << 16 | mantissa[1] >> 16;
	// result back in byte array
	for (j = 0; j < 8; j++)
		fb[j] = bld.by[j];

#else
	// convert GDSII double to _G_UNIX (Apollo) double
   //if all is zero, it is true zero value
	if (bld.fl[0] == 0 && bld.fl[1] == 0 )
		return;

#if wxBYTE_ORDER == wxLITTLE_ENDIAN  || defined(ultrix)
	// swap the longs
	swap = bld.fl[0] ; bld.fl[0] = bld.fl[1] ; bld.fl[1] = swap ;
#endif
	// mask sign bit and shift to bit 1
	s = (bld.fl[0] & 0x80000000L) >> 31;
	// same for exponent bits
	e = (bld.fl[0] & 0x7F000000L) >> 24;

	// mask mantisse bits
	mantissa[0] = bld.fl[0] & 0x00FFFFFFL;
	mantissa[1] = bld.fl[1];


/* THE NUMBER IS WRONG BUT IT DOES NOT HARM ME, SO I OUTCOMENTED IT
   WE just shift what is in the mantisse some more until
   we reach one that is set.
	// if < 1/16 is not a right number
	if (mantissa[0] < 0x00100000L)
	{
		bld.fl[0] = 0x80000000L;
		bld.fl[1] = 0x00000000L;

#if wxBYTE_ORDER == wxBIG_ENDIAN
#		if defined(ultrix)
		swap = bld.fl[0];
		bld.fl[0] = bld.fl[1];
		bld.fl[1] = swap;
#		endif
		// result back in byte array
		for (j = 0; j < 8; j++)
			fb[j]=bld.by[j];
		return;
	}
*/

	// 2^(4*(e-64))=GDS  +1023=for unix
	e = 4*(e - 64) + 1023;

	if (mantissa[0] == 0 && mantissa[1] == 0 && e != 0)
	{  //this is a bad number, the mantisse is total zero
      //so  i decide to return true zero as result
		bld.fl[0] = 0x00000000L;
		bld.fl[1] = 0x00000000L;

		// result back in byte array
		for (j=0;j<8;j++)
			fb[j]=bld.by[j];
		return;
	}

   // change mant to 1.mant
   while (mantissa[0] < 0x01000000L)
   {
      // times two
      carry = mantissa[1] >> 31;
      mantissa[0] = (mantissa[0] << 1) | carry;
      mantissa[1] = mantissa[1] << 1;

      // so minus one
      e--;
   }

	// out of range
	if( e < 1 || e > 2047 )
	{
		bld.fl[0] = 0x80000000L;
		bld.fl[1] = 0x00000000L;

#if wxBYTE_ORDER == wxLITTLE_ENDIAN || defined(ultrix)
		swap = bld.fl[0];
		bld.fl[0] = bld.fl[1];
		bld.fl[1] = swap;
#		endif

		// result back in byte array
		for (j=0;j<8;j++)
			fb[j]=bld.by[j];
		return;
	}

	// now skip highest redundant bit of mantisseand shift mantisse
	// to the right place in the longs
	bld.fl[0] = ((mantissa[0] & 0x00FFFFFFL) >> 4) | (s << 31) | (e << 20);
	bld.fl[1] = (mantissa[0] << 28) | (mantissa[1] >> 4);

#if wxBYTE_ORDER == wxLITTLE_ENDIAN || defined(ultrix)
	swap = bld.fl[0];
	bld.fl[0] = bld.fl[1];
	bld.fl[1] = swap;
#	endif
	// result back in byte array
	for (j = 0; j < 8; j++)
		fb[j]=bld.by[j];
#endif  vax
}


// convert a machine double to gds double
// usage: void   double_gds( fb )
// input
//   arguments  : unsigned char fb[]  8byte array with machine double
// output
//   arguments  : unsigned char fb[]  8byte array with gds double

/* Important note:
		The number of mantissa bit in VAX representation is
		one less than the number of mantissa bits in GDSII
		representation. This routine is not able to generate the
		last 2 bits of the GDSII representation exactly.
		There does not exist a general solution for this problem!!
		In order to overcome the single problem that we are aware of,
		the UNITs record that is sometimes compared to a UNITs record,
		the solution is hard coded in this routine.
		The last 8 bits of a number representing 1E-9 result in 53,
		but should produce 51 (See GDSII documentation of UNIT record
		and the given example of it).

		!! The same story holds for APOLLO and wxBIG_ENDIAN,
		but even worse. 1E-3 results in
		F0 for the last 8 bits instead of EF. Also corrected.
*/

void Gdsio::double2gds(unsigned char fb[])

{
	union bldun
	{
		unsigned char	by[8];
		wxUint32	fl[2];
		double			machdb;
	} bld;

	union
	{
		wxUint32	f[2];
		double			check;
	} hard_check;
	int j;

#ifdef vax
	wxUint32 carry, e, f[2], s;

	// make copy into union
	for (j = 0; j < 8; j++)
		bld.by[j] = fb[j];

	hard_check.f[0] = bld.fl[0];
	hard_check.f[1] = bld.fl[1];

	// take care of the words places
	s = (bld.fl[0] & 0x00008000L) >> 15;
	// mask sign and exponent and shift to bit1
	e = (bld.fl[0] & 0x00007F80L) >> 7;

	if( s == 0 & e == 0)
	{
		bld.fl[0] = bld.fl[1] = 0;
		// result back in byte array
		for (j=0;j<8;j++)
			fb[j]=bld.by[j];
		return;
	}

	// make mant 0.1mant
	f[0] = (bld.fl[0] & 0x0000007FL) << 16 | bld.fl[0] >> 16 | 0x00800000L;
	// swap words
	f[1] = bld.fl[1] << 16 | bld.fl[1] >> 16;

	// true exponent
	e = e - 128;

	// new exponent 16^exp
	while ((e % 4) != 0)
	{
		// divide by two
		carry = f[0] & 0x00000001L;
		f[0] = f[0] >> 1;
		f[1] = f[1] >> 1 | carry << 31;
		// so add 1
		e++;
	}

	// now change 2^expo to 16^exp for GDS
	e = e/4 + 64;

	// put bits on right place
	bld.fl[1] = f[0] | e << 24 | s << 31;
	bld.fl[0] = f[1];

	// Hard coded exception begin!
	if (hard_check.check == 1E-9)
		bld.fl[0] = bld.fl[0] & 0XFFFFFF51L;
	// End of hard coded exception
	for (j = 0; j < 8; j++)
		bld.by[j] = fb[j];

#else
	//convert unix (Apollo,wxLITTLE_ENDIAN) double to GDSII double
	wxUint32 carry, f[2], s;
#if wxBYTE_ORDER == wxLITTLE_ENDIAN || defined(ultrix)
	wxUint32 swap;
#	endif

	long e;

	for (j = 0; j < 8; j++)
		bld.by[j]=fb[j];

	// backup of original machine number
	hard_check.f[0] = bld.fl[0];
	hard_check.f[1] = bld.fl[1];

#if wxBYTE_ORDER == wxLITTLE_ENDIAN || defined(ultrix)
	swap = bld.fl[0];
	bld.fl[0] = bld.fl[1];
	bld.fl[1] = swap;
#	endif

	// mask sign bit
	s = (bld.fl[0] & 0x80000000L) >> 31;
	// mask exponent
	e = (bld.fl[0] & 0x7FF00000L) >> 20;

	// number to small
	if( (s == 0) & (e == 0))
	{
		// assume it is zero
		bld.fl[0] = bld.fl[1] = 0;

#if wxBYTE_ORDER == wxLITTLE_ENDIAN || defined(ultrix)
		// for turbo c real, integer,d oubles are saved in memory
		// in reverse order, so first reverse bytes
		for (j=0;j<8;j++)
			fb[j]=bld.by[7-j];
#	else
		for (j=0;j<8;j++)
			fb[j]=bld.by[j];
#	endif

		return;
	}

	// 52 bits mantisse mask
	f[0] = (bld.fl[0] & 0x000FFFFFL);
	f[1] = bld.fl[1];

	// add 2 ^ 0 bit that is not yet represented because it was redundant
	f[0] |= 0x00100000L;

	//
	//	 -- Now bitposition for 2 ^ -1 is bit 12. It should be 8.
	//	 -- Instead of shifting 4 bits to the left and than normalize the
	//	 -- exponent by shifting 1 to 4 times to the right, there is only
	//	 -- one shift of 0 to 3 time to the left. This is reflected in the
	//	 -- exponent that will be 1 greater in the end
	//

	// true exponent
	e = e - 1023;

	// exponent should be 2^(4*newexp)
	while( (e % 4) != 0 )
	{
		// divide by two
		carry = f[1] >> 31;
		f[0] <<= 1;
		f[1] <<= 1;
		f[0] |= carry;
		// so minus one
		e--;
	}

	// because of trick add one more
	e = e/4 + 65;

	// number to big are small
	if( e < 0 || e > 127 )
	{
		bld.fl[0] = 0x80000000L;
		bld.fl[1] = 0x00000000L;

#if wxBYTE_ORDER == wxLITTLE_ENDIAN || defined(ultrix)
		swap = bld.fl[0];
		bld.fl[0] = bld.fl[1];
		bld.fl[1] = swap;
#		endif

#if wxBYTE_ORDER == wxLITTLE_ENDIAN || defined(ultrix)
		// for turbo c real, integer, doubles are saved in memory
		// in reverse order, so first reverse bytes
		for (j=0;j<8;j++)
			fb[j]=bld.by[7-j];
#		else
		for (j=0;j<8;j++)
			fb[j]=bld.by[j];
#		endif

		return;
	}

	// put bits into right place for GDS
	bld.fl[0] = f[0] | (e << 24) | (s << 31);
	bld.fl[1] = f[1];

	// Hard coded exception begin!
	if( hard_check.check == 1E-3)
		bld.fl[1] = (bld.fl[1] & 0XFFFFFF00L) | 0X000000EFL;
	if( hard_check.check == 1E-9)
		bld.fl[1] = (bld.fl[1] & 0XFFFFFF00L) | 0X00000051L;
	// End of hard coded exception

#if wxBYTE_ORDER == wxLITTLE_ENDIAN || defined(ultrix)
	swap = bld.fl[0];
	bld.fl[0] = bld.fl[1];
	bld.fl[1] = swap;
#	endif
#if wxBYTE_ORDER == wxLITTLE_ENDIAN
	// for turbo c real,integer,doubles are saved in memory
	// in reverse order, so first reverse bytes
	for (j = 0; j < 8; j++)
		fb[j] = bld.by[7-j];
#	else
	for (j = 0; j < 8; j++)
		fb[j] = bld.by[j];
#	endif

#endif
}

