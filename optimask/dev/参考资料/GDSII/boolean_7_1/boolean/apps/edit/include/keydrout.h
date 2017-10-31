//! author="klaas"
//! version="1.0"
/*
Program		GDSDUMP.H
Purpose		Creates a plain text file from a GDS-II structure
Programmers		R. Spekreijse & N. Noorlander
Last Update		22-12-1995
*/

#ifndef _KEY_OUTDRIVER
#define _KEY_OUTDRIVER

#ifdef __GNUG__
#pragma interface
#endif

#include <string.h>
#include <fstream>

using namespace std;

#include "drivout.h"
#include "misc.h"
#include "gdserr.h"
#include "lsetup.h"
#include "proplist.h"

class Strans;

//!Output driver for KEY files
/*!
The KEY format is an ASCII file structured simular to GDSII, only it supports more
primitives and also properties to the primitives
Every object in the internal database is written in records in the KEY file
Info on the different record types that are written can be found in the KEY format manual
overloaded function from class Driver_out write the internal objects in KEY specific form
*/
class KEY_driver_out: public Driver_out
{
	public:
		//! constructor
      /*!
      \param filename: name of KEY output file
      \param only_visible: write only visible layers
      */
		KEY_driver_out(const wxString& _filename, bool only_visible = false);

		//! destructor
		~KEY_driver_out();

		//! overloaded function from class Driver
		void 	Write(ArrayReference*);
		void 	Write(Element*);
		void 	Write(Boundary*);
		void  Write(Box* _Box);
		void  Write(Circle* _Circle);
		void 	WriteFlags(Element*);
		void 	Write(GdsII*);
		void 	Write(Library*);
		void 	Write(Path*);
		void 	Write(Arcseg*);
		void 	Write(Point*);
		void  Write(Segment *_Segment);
		void 	Write(G_Polygon*);
		void 	Write(G_Polyline*);
		void 	Write(StructureReference*);
		void 	Write(Structure*);
		void 	Write(Text*);
		void  Write(Image* _image);
		void  Write(SLine* line);

	protected:
		void 	Write(Strans*);

      //!write property data of primitives
		void  Write(wxPropertySheet* propsheet);

      //!write basic header for objects (ASCII keywords)
		void  WriteHeader();

      //!output file name
		wxString		KEY_out_filename;

      //!output file handle
		ofstream	KEY_out_file;

      //! number of points on one line
		int      points_written;

      //!mapping to internal layers KEY layers
      int 			_mapping_out[1000];
};

#endif //_KEY_DRIVER
