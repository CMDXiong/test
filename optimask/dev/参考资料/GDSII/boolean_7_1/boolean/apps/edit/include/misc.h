//! author="klaas"
//! version="1.0"
//
// misc.h
//
//
//
// Purpose:
//
// Set all defaults value used in the GDS-II datastructure. Also defines some variable
// as PointType, bool and G_BYTE, all used in the datastructure.
//

#ifndef MISC_H
#define MISC_H

#ifndef _CRT_SECURE_NO_DEPRECATE
	#define _CRT_SECURE_NO_DEPRECATE
#endif //_CRT_SECURE_NO_DEPRECATE

#ifdef __GNUG__
#pragma interface
#endif

#include "limits.h"
#define PROGRAM_NAME "GDS-II Postprocessing Tool 7.1 kbool 2.1"
#ifdef __UNIX__
int stricmp(const char*,const char*);
#endif

#ifdef  M_PI
#undef  M_PI
#endif
#define M_PI		(3.1415926535897932384626433832795028841972)

#ifdef  M_PI_2
#undef  M_PI_2
#endif
#define M_PI_2      1.57079632679489661923

#ifdef  M_PI_4
#undef  M_PI_4
#endif
#define M_PI_4      0.785398163397448309616

#ifndef NULL
#define NULL 0
#endif

//
// defaults
//

#define DEFAULT_WIDTH					0

//
//
// main program defaults
//

double Check_int(double getal);

#include "b_statusb.h"
extern StatusBar *_statusbar;

// messagehandler
#include "gdsmes.h"
extern GDSMessage *_messagehandler;

#include "gdserr.h"

#define		LINELENGTH		200

wxString ConvertToUnixPath( const wxString& dospath );

#endif

