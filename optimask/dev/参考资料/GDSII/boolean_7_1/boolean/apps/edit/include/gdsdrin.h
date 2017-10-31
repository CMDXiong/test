//! author="klaas"
//! version="1.0"
/*
Program		GDSDRIN.H
Purpose		GdsII input driver inherited from class Driver_in (Header)
Last Update		25-09-1996
*/


#ifndef _GDS_DRIVER_IN
#define _GDS_DRIVER_IN

#ifdef __GNUG__
#pragma interface
#endif

//! headerfiles="gdsio.h misc.h gdsdef.h gdserr.h lsetup.h"
//! codefiles="gdsdrin.cpp"

#include <string.h>

#include "gdsio.h"
#include "misc.h"
#include "gdsdef.h"
#include "gdserr.h"

#include "lsetup.h"

class ArrayReference;
class Boundary;
class Element;
class ElementList;
class ColumnsRows;
class Elflags;
class GdsII;
class Library;
class Path;
class Box;
class Point;
class Segment;
class Shape;
class G_Polygon;
class G_Polyline;
class Reference;
class StructureReference;
class Structure;
class Text;

class Strans;

//!Input driver for GDSII files
/*!
Every record in the GDSII file is read/mapped into the right internal structure
The GDSII file is parsed according to the BNF of the GDSII format.
Info on the different record types that are read can be found in the GDSII format manual
All basic data types are read using a specialized file stream.
\sa Gdsio
*/
class GDS_driver_in
{
	public:
		//! constructor
      /*!
      \param setup: layer properties
      \param filename: name of GDSII input file
      */
      GDS_driver_in(const wxString& filename);
		~GDS_driver_in();

      //!read in total GDSII file
      /*!
      \param _GdsII: Internal database structure pointer
      */
      void Read(GdsII* _GdsII);

   protected:
		bool ReadBgnlib(GdsII* _GdsII);
		bool ReadBgnstr(Structure* _struct);
		bool ReadProperty(Structure* _structure);
		bool ReadBoundary(Structure* _structure);
      bool ReadPath(Structure* _structure);
      bool ReadText(Structure* _structure);
      bool ReadStructureReference(Structure* _structure);
      bool ReadArrayReference(Structure* _structure);
		bool ReadNode(Structure* _structure);
		bool ReadBox(Structure* _structure);
		bool ReadElflags(Element* _Element);
		bool ReadGdsHeader(GdsII* _GdsII);
		bool ReadLayer(Shape* _Shape);
		bool ReadLibraryName(Library* _Library);
		bool ReadLibrarySettings(GdsII* _GdsII);
		bool Read(Point* _Point);
		bool ReadPresentation(Text* _Text);
		bool ReadStrans(Strans* _Strans);
		bool ReadStructure(GdsII*      _structurelist);
		bool ReadTexttype(Text* _Text);
		bool ReadElement(Structure* _structure);
		bool ReadUnits(Library* _Library);
		bool ReadWidth(Shape* _Shape);
		bool ReadRefLibs(Library* _Library);
		bool ReadFonts(Library* _Library);
		bool ReadAttrtable(Library* _Library);
		bool ReadGenerations(Library* _Library);
		bool ReadDatatype(Boundary* _Boundary);
		bool ReadDatatype(Path* _Path);
		bool ReadBoxtype(Box* _Box);
		bool ReadTextBody(Text* _text);

		void Read(Segment* _Seg);
		bool Read(G_Polygon* _Polygon);
		bool Read(G_Polyline* _Polyline);

      //!read the specified record type from the GDSII file
      //!if the last record is not this type, it is stored for the next read
		bool ReadHeader(int type);

      //!skip the date of the last read record
		void	SkipData();

      //!read string data for Text element, structure names etc.
		void  ReadString(wxString& a_string);

      //!input file name
		wxString							GDS_filename;

      //!input file handle
		Gdsio								GDS_file;

      //!last read record is stored
		bool							_back;
      //!size of the last read record
		TWO_G_BYTE_UNSIGNED_INTEGER	_recordsize;
      //!recordtype of last read record
		G_BYTE							_recordtype;
      //!the type of the data in the last read record
		G_BYTE							_datatype;
      //!mapping of GDSII layers to internal layers
      int 								_mapping_in[1000];
      //!the units of the current file that is read
      GdsII*							m_cur_gdsii;
};

#endif //_GDS_DRIVER_IN
