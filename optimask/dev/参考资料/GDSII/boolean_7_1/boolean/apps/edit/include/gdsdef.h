/*
	Program	GDSDEF.H
	Purpose	Defines global constances for GDS files
	Programmers	R. Spekreijse & N. Noorlander
	Last Update	07-12-1995
*/

#ifndef GDSDEFINES_H
#define GDSDEFINES_H
#ifdef __GNUG__
#pragma interface
#endif

// constants for file i/o used by the written routines
#define GDS_ERROR	-1

// definition gdsii record constants:
#define GDS_DEFAULT_STRING_LENGTH						44		// default size of and GDS-II string
const int GDS_HEADERSIZE			  =					4;		// size of header
#define GDSTYPE_TWO_G_BYTE_UNSIGNED_INTEGER			7		// record contains 2 byte unsigned integers
#define GDSTYPE_FOUR_G_BYTE_UNSIGNED_INTEGER		  	8		// record contains 4 byte unsigned integers

// gdsii data type constants:
#define GDSTYPE_NO_DATA				  						0 		// no data in record contained
#define GDSTYPE_BIT_ARRAY				  					1		// record contains bitarray vars
#define GDSTYPE_TWO_G_BYTE_SIGNED_INTEGER			  	2		// record contains 2 byte integers
#define GDSTYPE_FOUR_G_BYTE_SIGNED_INTEGER		  	3		// record contains 4 byte integers
#define GDSTYPE_FOUR_G_BYTE_REAL		  					4		// record contains 4 byte reals
#define GDSTYPE_EIGHT_G_BYTE_REAL	        			5		// record contains 8 byte reals
#define GDSTYPE_ASCII_STRING	  							6		// record contains GDSII ASCII-strings

// gdsiifile recordtype constants:
// see calma document
#define GDS_HEADER										 	0
#define GDS_BGNLIB		 									1
#define GDS_LIBNAME										 	2
#define GDS_UNITS		 										3
#define GDS_ENDLIB											4
#define GDS_BGNSTR										 	5
#define GDS_STRNAME										 	6
#define GDS_ENDSTR										 	7
#define GDS_BOUNDARY										 	8
#define GDS_PATH											 	9
#define GDS_SREF												10
#define GDS_AREF												11
#define GDS_TEXT												12
#define GDS_LAYER												13
#define GDS_DATATYPE											14
#define GDS_WIDTH												15
#define GDS_XY													16
#define GDS_ENDEL												17
#define GDS_SNAME												18
#define GDS_COLROW											19
#define GDS_TEXTNODE											20
#define GDS_NODE												21
#define GDS_TEXTTYPE											22
#define GDS_PRESENTATION									23
#define GDS_SPACING											24
#define GDS_STRING											25
#define GDS_STRANS											26
#define GDS_MAG												27
#define GDS_ANGLE												28
#define GDS_UINTEGER											29
#define GDS_USTRING											30
#define GDS_REFLIBS											31
#define GDS_FONTS												32
#define GDS_PATHTYPE											33
#define GDS_GENERATIONS										34
#define GDS_ATTRTABLE										35
#define GDS_STYPTABLE										36
#define GDS_STRTYPE											37
#define GDS_ELFLAGS											38
#define GDS_ELKEY												39
#define GDS_LINKTYPE											40
#define GDS_LINKKEYS											41
#define GDS_NODETYPE											42
#define GDS_PROPATTR											43
#define GDS_PROPVALUE										44
#define GDS_BOX												45
#define GDS_BOXTYPE											46
#define GDS_PLEX												47
#define GDS_BGNEXTN											48
#define GDS_ENDEXTN											49
#define GDS_TAPENUM											50
#define GDS_TAPECODE											51
#define GDS_STRCLASS											52
#define GDS_RESERVED										  	53
#define GDS_FORMAT											54
#define GDS_MASK												55
#define GDS_ENDMASKS											56
// The following 3 items are from the Cadence Construct v4.0 GDS-extention
//  and not part of the original GDS-II format (added for read compatibility)
#define GDS_LIBDIRSIZE										57
#define GDS_SRFNAME											58
#define GDS_LIBSECUR											59

#endif

