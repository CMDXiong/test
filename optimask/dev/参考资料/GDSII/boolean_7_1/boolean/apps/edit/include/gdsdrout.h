//! author="klaas"
//! version="1.0"
/*
Program		GDSDROUT.H
Purpose		GdsII output driver inherited from class Driver_out (Header)
Last Update		24-09-1996
*/

#ifndef _GDS_DRIVER_OUT
#define _GDS_DRIVER_OUT
#ifdef __GNUG__
#pragma interface
#endif

//! headerfiles="drivout.h misc.h gdsio.h gdsdef.h gdserr.h lsetup.h"
//! codefiles="gdsdrout.cpp"

#include <string.h>

#include "drivout.h"
#include "gdsio.h"
#include "misc.h"
#include "gdsdef.h"
#include "gdserr.h"
#include "lsetup.h"
class Strans;

//!Output driver for GDSII files
/*!
 Every object in the internal database is written in records in the GDSII file
 Info on the different record types that are written can be found in the GDSII format manual
 All basic data types for the GDSII records are written using a specialized file stream.
 overloaded function from class Driver_out write the internal objects in GDSII specific form
\sa Gdsio
*/
class GDS_driver_out : public Driver_out
{
	public:
		//! constructor
      /*!
      \param filename: name of GDSII output file
      \param only_visible: write only visible layers
      */
		GDS_driver_out(const wxString& filename, bool only_visible=false);
		//! destructor
		~GDS_driver_out();

      //!was the file opened correctly
		bool bad();

		//! overloaded function from class Driver_out
		void 	Write(Box* _Box);
		void 	Write(Circle* _Circle);
		void 	Write(ArrayReference*);
		void 	Write(Boundary*);
		void 	Write(GdsII*);
		void 	Write(Library*);
		void 	Write(Path*);
		void 	Write(Arcseg*);
		void 	Write(Point*);
		void 	Write(Segment*);
		void 	Write(G_Polygon*);
		void 	Write(G_Polyline*);
		void 	Write(StructureReference*);
		void 	Write(Structure*);
		void 	Write(Text*);
		void  WriteFlags(Element* );

	protected:
		void 	Write(Strans*);
      //!write HEADER for GDSII records
		void	WriteHeader(TWO_G_BYTE_UNSIGNED_INTEGER, G_BYTE, G_BYTE);

      //!write string data for specific records
		void	WriteString(const wxString&);

      //!output file name
		wxString							GDS_filename;
      //!output file handle
		Gdsio								GDS_file;

		bool                     _bad;

		//!record size to write
		TWO_G_BYTE_UNSIGNED_INTEGER	_recordsize;
      //!record type to write
		G_BYTE							_recordtype;
      //!data type to write for the record
		G_BYTE							_datatype;

      //!mapping to internal layers GDSII layers
      int 								_mapping_out[1000];

      //!to scale to four byte integer if needed, this is the maximum for GDSII coordinates
      double _userunits_out;

      //!to scale to four byte integer if needed, this is the maximum for GDSII coordinates
      double _physunits_out;

      //!to scale to four byte integer if needed, this is the maximum for GDSII coordinates
      double _scale_out;
};

#endif //_GDS_DRIVER_OUT
