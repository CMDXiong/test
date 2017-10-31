//! author="klaas"
//! version="1.0"
/*
Program		KEYDRIN.H
Purpose		KEY input driver
*/

#ifndef _KEY_DRIVER_IN
#define _KEY_DRIVER_IN
#ifdef __GNUG__
#pragma interface
#endif

#include <string.h>

//! headerfiles="misc.h gdserr.h lsetup.h alias.h b_lnk_itr.h cparser.h"

#include "misc.h"
#include "gdserr.h"

using namespace std;

#include "lsetup.h"
#include "alias.h"
#include "b_lnk_itr.h"
#include "cparser.h"

class Arcseg;
class ArrayReference;
class Boundary;
class Box;
class SLine;
class Circle;
class Element;
class ElementList;
class ColumnsRows;
class Elflags;
class GdsII;
class Header;
class Library;
class Path;
class Point;
class Segment;
class Shape;
class G_Polygon;
class G_Polyline;
class Presentation;
class Reference;
class Strans;
class StructureReference;

class Structure;
class Text;
class Units;
class Image;

//!Input parser for KEY data
/*!
Every record in the KEY file/data is read/mapped into the right internal structure
The KEY file is parsed according to the BNF of the KEY format.
Info on the different record types that are read can be found in the KEY format manual
\sa CommandParser
\sa KEY_string_in
\sa KEY_driver_in
*/
class KEY_parser
{
	public:
		//! constructor
      //!\param setup: layer properties
		KEY_parser();
		//! destructor
		virtual ~KEY_parser();

      //!read complte GDSII internal structure
		void			Read(GdsII*);

	protected:

      //!read one record from the KEY Data
		virtual bool 		ReadItem(const wxString& type)=0;
      //!skip the data for one record from the KEY Data
		bool		SkipXYData();

		bool 		ReadGdsHeader(GdsII* _GdsII);
		bool 		ReadBgnlib(GdsII* _GdsII);
		bool 		ReadBgnstr(Structure* _structure);
		bool     ReadLibrarySettings(GdsII* _GdsII);
		bool 		ReadLibraryName(Library* _library);
		bool 		ReadUnits(Library* _library);
		bool 		ReadStructure(GdsII* _structurelist);
		bool 		ReadElement(Structure* _structure);

		//! Element
		bool 		ReadBoundary(Structure* _structure);
		//! Element
		bool      ReadPath(Structure* _structure);
		//! Element
		bool 		ReadText(Structure* _structure);
		//! Element
		bool 		ReadArrayReference(Structure* _structure);
		//! Element
		bool      ReadStructureReference(Structure* _structure);
		//! Element
		bool 		ReadArc(Structure* _structure);
		//! Element
		bool 		ReadCircle(Structure* _structure);
		//! Element
		bool 		ReadNode(Structure* _structure);
		//! Element
		bool 		ReadBox(Structure* _structure);
		//! Element
		bool 		ReadImage(Structure* _structure);
		//! Element
		bool 		ReadLine(Structure* _structure);

		bool 		ReadElflags(Element* _element);
		bool 		ReadLayer(Shape* _shape);
		bool 		ReadDataType(Boundary* _boundary);
		bool 		ReadDataType(Path* _path);
		bool		ReadDataType(Arcseg* _arcseg);
		bool		ReadDataType(Circle* _circle);
		bool		ReadBoxType(Box* _box);
		bool 		Read(G_Polygon* _polygon);
		bool 		ReadWidth(Shape* _shape);
		bool		Read(G_Polyline* _polyline);
		bool 		ReadTextBody(Text* _text);
		bool 		ReadPresentation(Text* _text);
		bool 		ReadStrans(Strans* _strans);
		bool     Read(Point* _point);
		bool     ReadRefLibs(Library* _library);
		bool     ReadFonts(Library* _library);
		bool 		ReadAttrtable(Library* _library);
		bool 		ReadGenerations(Library* _library);
		bool 		ReadFormatType(Library* _library);
		bool 		ReadColRow(ArrayReference* _aref);
		bool 		ReadProperties(Element* _element);

      //!keyword for record in KEY data
		wxString			_keyword;
      //!value for record in KEY data
		wxString       _value;

      //!type of the record in KEY data
		int 			         _recordtype;

      //!last read line in DATA
		long 				    	_linenumber;

      //!scaling factor of input coordinates
      double		_factor;

      //!is last record stored (means already read)
		bool 		_back;


      //!mapping of KEY layers to internal layers
      int 			_mapping_in[1000];
};

//!Input driver for KEY files
class KEY_driver_in : public KEY_parser, public CommandParser , public ifstream
{
	public:
		//! constructor
		KEY_driver_in(const wxString& filename,double factor = 1);
		//! destructor
		~KEY_driver_in();

	protected:
		bool ReadItem(const wxString& type);

      //!read next character
 		void IncC();
      //!what is the next character
		char PeekNextC();

      //!input file name
		wxString		KEY_filename;

      //!the units of the current file that is read
      GdsII*		m_cur_gdsii;
};


//!Input driver for KEY strings (the string contains KEY records)
/*!
only the following primitives can be added this way
\verbatim
	<element>			::=	<boundary> | <path> | <circle> | <arc> |<text> | <node> | <box>
									 {<property>}*
									ENDEL
\endverbatim
*/
class KEY_string_in : public KEY_parser, public CommandParser , public std::ostringstream
{
	public:
		//! constructor
		KEY_string_in(AliasList* aliaslist, double factor);
		//! destructor
		~KEY_string_in();

      //!add the KEY data in string to the internal database
		bool Insert(GdsII* _GdsII,const wxString& string);
      //!insert a reference to a structure
		bool ReadStructureReference(Structure* _structure,GdsII* _GdsII);
		bool ReadArrayReference(Structure* _structure,GdsII* _GdsII);

	protected:

		bool ReadItem(const wxString& type);

      //!read next character
 		void IncC();

      //!to evaluate alias strings
		char PeekNextC();

      //!buffer for the string to evaluate
      char*	_commands;

      //!current character
      char* 		_c;

};

#endif //_KEY_DRIVER_IN



