/*
Program	GDSIO.H
Purpose	Used by GDS driver for conversions and read en write
		GDSII elements (header)
Programmers	R. Spekreijse & N. Noorlander
Last Update	19-09-1995
*/

#ifndef GDSIO_H
#define GDSIO_H
#ifdef __GNUG__
#pragma interface
#endif

#include <fstream>

using namespace std;

#include "gdsii.h"
#include "gdserr.h"
#define MAX_G_BYTES  		8		// maximum bytes a value can use

// when creating an 16 or 32 bits application, the size of the values must be
// the same size

typedef unsigned char  					G_BYTE;		// 8 bits 	-127   	128

// 32 bits application definition
typedef unsigned short int	TWO_G_BYTE_UNSIGNED_INTEGER;
typedef unsigned int 		FOUR_G_BYTE_UNSIGNED_INTEGER;
typedef short int		TWO_G_BYTE_SIGNED_INTEGER;
typedef int				FOUR_G_BYTE_SIGNED_INTEGER;
typedef float			FOUR_G_BYTE_REAL;
typedef double			EIGHT_G_BYTE_REAL;

typedef struct _ARRAY    { unsigned char array[MAX_G_BYTES]; } _ARRAY;
typedef union a_union_value {
								TWO_G_BYTE_SIGNED_INTEGER 	two_byte_signed_integer;
								FOUR_G_BYTE_SIGNED_INTEGER 	four_byte_signed_integer;
								FOUR_G_BYTE_REAL 				four_byte_real;
								EIGHT_G_BYTE_REAL				eight_byte_real;

								_ARRAY						array;

							} a_union_value;




//!reading basic GDSII format datatypes
//!\sa GDS_driver_in
//!\sa GDS_driver_out
class Gdsio : public fstream
{
	public:
		//! convert a gdsii real to a machine real
		void gds2double(unsigned char *);
		//! convert a machine real to a gdsii real
		void double2gds(unsigned char *);

		//! input overloading
		Gdsio& operator>>(G_BYTE *);
		//! input overloading
		Gdsio& operator>>(TWO_G_BYTE_UNSIGNED_INTEGER *my_integer);
		//! input overloading
		Gdsio& operator>>(TWO_G_BYTE_SIGNED_INTEGER *);
		//! input overloading
		Gdsio& operator>>(FOUR_G_BYTE_SIGNED_INTEGER *);
		//! input overloading
		Gdsio& operator>>(FOUR_G_BYTE_REAL *);
		//! input overloading
		Gdsio& operator>>(EIGHT_G_BYTE_REAL *);

		//! output overloading
		Gdsio& operator<<(G_BYTE );
		//! output overloading
		Gdsio& operator<<(TWO_G_BYTE_UNSIGNED_INTEGER my_integer);
		//! output overloading
		Gdsio& operator<<(TWO_G_BYTE_SIGNED_INTEGER );
		//! output overloading
		Gdsio& operator<<(FOUR_G_BYTE_SIGNED_INTEGER );
		//! output overloading
		Gdsio& operator<<(FOUR_G_BYTE_REAL );
		//! output overloading
		Gdsio& operator<<(EIGHT_G_BYTE_REAL );
};
#endif

