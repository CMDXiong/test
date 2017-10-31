/*
	Program		KEYDRIN.CPP
	Purpose		KEY input driver
*/


#ifdef __GNUG__
#pragma implementation
#endif

#include "keydrin.h"
#include "proplist.h"

#include "arc.h"
#include "boundary.h"
#include "elemlst.h"
#include "gdsii.h"
#include "path.h"
#include "point.h"
#include "segment.h"
#include "polyline.h"
#include "referenc.h"
#include "structur.h"
#include "text.h"
#include "units.h"
#include "trans.h"


// Contructor ---------------------------------------------------------------
KEY_driver_in::KEY_driver_in(const wxString& filename, double factor):KEY_parser()
{
   a = ' ';

	_back=false;

   KEY_filename=filename;

   _factor = factor;

	open(KEY_filename, ios::in);
	if (bad())
	{
		wxString errbuf;
		errbuf.Printf("Cannot open file %s!",KEY_filename.c_str() );
		throw GDS_Error((char*)errbuf.c_str(), "KEYParser: File Error", 9, 0);
	}

	_aliaslist = new AliasList();

	_keyword.Clear();
	_value.Clear();
    int i;
	for (i = 0; i < 999; i++)
		   _mapping_in[i]=-1;
	for (i = 0; i < MAX_LAYERS; i++)
		   _mapping_in[GLOB->SETUP->Get_GdsInMapping(i)]=i;
	for (i = 0; i < 999; i++)
       if (_mapping_in[i]==-1)
		   	_mapping_in[i]=0;
}


// Destructor ---------------------------------------------------------------
KEY_driver_in::~KEY_driver_in()
{
	delete _aliaslist;
}

void KEY_driver_in::IncC()
{
   a=get();
   if (eof())
       a='\0';
   if (a==EOF)
       a='\0';
}

char KEY_driver_in::PeekNextC()
{
   char p=peek();
   return p;
}

// ReadItem (parser) --------------------------------------------------------
bool KEY_driver_in::ReadItem(const wxString& type)
{
//	<item>		::=	KEYWORD VALUE
	if (_back)
	{
		if (!stricmp(_keyword,type))
			_back = false;
		else
			_back = true;
		return bool(!_back);
	}

   wxString buf;
   bool Next=true;
   do
   {
	   FlushArg();
      if( !GetCommand() )
      {
         if (strlen(GetErrorMes()) > 0 )
         {
            buf.Printf("Could not Parse line %ld: \n Error: %s",
                  _linenumber,GetErrorMes().c_str() );
            throw GDS_Error((char*)buf.c_str(), "Value Parsing Error",9,0);
         }
         return false;
      }
      _I->tohead();
      if (!_I->count())
         Next=true;
      else if (  _I->item()->GetChar(0) == '#' ) //it is comment read next are empty line
         Next=true;
      else
         Next=false;
   }
   while (Next);

   if (_I->count() > 2)
   {
      buf.Printf("need max 2 items in command %ld: \n ",_linenumber);
      throw GDS_Error((char*)buf.c_str(), "Parsing Error",9,0);
   }

   _linenumber++;
   _keyword=*_I->item();
   if (_I->count() > 1)
   {
      (*_I)++;
      _value=*_I->item();
   }
   else
   {
		wxString buf;
      buf="Argument missing (in ";
	   if (!stricmp(_keyword,type))
   	{
			if (!stricmp(_keyword,"BGNLIB"));
         else if (!stricmp(_keyword,"UNITS"));
         else if (!stricmp(_keyword,"BGNSTR"));
         else if (!stricmp(_keyword,"BOUNDARY"));
         else if (!stricmp(_keyword,"LINE"));
         else if (!stricmp(_keyword,"CIRCLE"));
         else if (!stricmp(_keyword,"BOX"));
         else if (!stricmp(_keyword,"PATH"));
         else if (!stricmp(_keyword,"TEXT"));
         else if (!stricmp(_keyword,"AREF"));
         else if (!stricmp(_keyword,"SREF"));
         else if (!stricmp(_keyword,"ARC"));
         else if (!stricmp(_keyword,"ENDEL"));
         else if (!stricmp(_keyword,"ENDLIB")); // These commands don't have arguments...
         else
         {
         	buf+=_keyword;
            buf+=")";
				throw GDS_Error((char*)buf.c_str()); // There is no argument, so throw an error...
         }
	   }
		_value.Clear();
   }

   if (!stricmp(_keyword,type))
      _back = false;
   else
      _back = true;
   return bool(!_back);
}

// Contructor ---------------------------------------------------------------
KEY_string_in::KEY_string_in(AliasList* aliaslist,double factor):KEY_parser()
{
	_back=false;

	_aliaslist = aliaslist;

   _factor = factor;

	_keyword.Clear();
	_value.Clear();

	_commands=0;
   _c=0;
   a = ' ';

	for (int i = 0; i < MAX_LAYERS; i++)
		   _mapping_in[i]=i;
}


// Destructor ---------------------------------------------------------------
KEY_string_in::~KEY_string_in()
{
}

void KEY_string_in::IncC()
{
   if (_c == 0) //get first character from buffer
   {
      _linenumber++;
      _c	= _commands;
   }
   else
   {  if (*_c!='\0')
	   	_c++;
   }
   a=*_c;
}

char KEY_string_in::PeekNextC()
{
   char p='\0';
   if (_c == 0) //get first character from buffer
       p=*_commands;
   else
   {  if (*_c!='\0')
      {
	   	_c++;
		   p=*_c--;
      }
   }
   return p;
}

// ReadItem (parser) --------------------------------------------------------
bool KEY_string_in::ReadItem(const wxString& type)
{
//	<item>		::=	KEYWORD VALUE
	if (_back)
	{
		if (!stricmp(_keyword,type))
			_back = false;
		else
			_back = true;
		return bool(!_back);
	}

   wxString buf;

   bool Next=true;
   do
   {
	   FlushArg();
      if( !GetCommand() )
      {
         if (strlen(GetErrorMes()) > 0 )
         {
            buf.Printf("Could not Parse line %ld: \n Error: %s",
                  _linenumber,GetErrorMes().c_str() );
            throw GDS_Error((char*)buf.c_str(), "Value Parsing Error",9,0);
         }
         return false;
      }
      _I->tohead();
      if (!_I->count())
         Next=true;
      else if (  *(_I->item()) == '#' ) //it is comment read next are empty line
         Next=true;
      else
         Next=false;
   }
   while (Next);

   if (_I->count() > 2)
   {
      buf.Printf("need max 2 items in command %ld: \n ",_linenumber);
      throw GDS_Error((char*)buf.c_str(), "Parsing Error",9,0);
   }

   _linenumber++;
   _keyword=(char*)_I->item()->c_str();
   if (_I->count() > 1)
   {
      (*_I)++;
      _value=(char*)_I->item()->c_str();
   }
   else
   {
		char buf[LINELENGTH];
      strcpy(buf,"Argument missing (in ");
	   if (!stricmp(_keyword,type))
   	{
			if (!stricmp(_keyword,"BGNLIB"));
         else if (!stricmp(_keyword,"UNITS"));
         else if (!stricmp(_keyword,"BGNSTR"));
         else if (!stricmp(_keyword,"BOUNDARY"));
         else if (!stricmp(_keyword,"CIRCLE"));
         else if (!stricmp(_keyword,"BOX"));
         else if (!stricmp(_keyword,"LINE"));
         else if (!stricmp(_keyword,"PATH"));
         else if (!stricmp(_keyword,"TEXT"));
         else if (!stricmp(_keyword,"AREF"));
         else if (!stricmp(_keyword,"SREF"));
         else if (!stricmp(_keyword,"ARC"));
         else if (!stricmp(_keyword,"ENDEL"));
         else if (!stricmp(_keyword,"ENDLIB")); // These commands don't have arguments...
         else
         {
         	strcat(buf,_keyword);
            strcat(buf,")");
				throw GDS_Error(buf); // There is no argument, so throw an error...
         }
	   }
		_value.Clear();
   }

   if (!stricmp(_keyword,type))
      _back = false;
   else
      _back = true;
   return bool(!_back);
}


bool KEY_string_in::ReadStructureReference(Structure* _structure,GdsII* _GdsII)
{
	if (KEY_parser::ReadStructureReference(_structure))
   {
      try
      {
      	_GdsII->LinkReferences();
      	_GdsII->SetReferenceNr();
      }
      catch (GDS_Error& _error)
      {
		   Reference* _sref= (Reference*) _structure->tailitem();
		   _structure->removetail();
         delete _sref;
         throw _error;
      }
      return true;
   }
   else
   	return false;
}

bool KEY_string_in::ReadArrayReference(Structure* _structure,GdsII* _GdsII)
{
	if (KEY_parser::ReadArrayReference(_structure))
   {
      try
      {
      	_GdsII->LinkReferences();
      	_GdsII->SetReferenceNr();
      }
      catch (GDS_Error& _error)
      {
		   Reference* _sref= (Reference*) _structure->tailitem();
		   _structure->removetail();
         delete _sref;
         throw _error;
      }
      return true;
   }
   else
   	return false;
}

// Insert --------------------------------------------------------
bool KEY_string_in::Insert(GdsII* _GdsII,const wxString& string)
{
	Structure* _structure=_GdsII->GetShowStructure();
   *this << string << '\0';
   FlushArg();

   _c=0;
   seekp(0);
   _commands= (char*) str().c_str();

//	<element>			::=	{<boundary> | <path> | <circle> | <arc> |<text> | <node> | <box>}
//									/* {<property>}* */
//									ENDEL

	while (1)
   {
      if  (ReadBoundary(_structure));
      else if (ReadPath(_structure));
      else if (ReadText(_structure));
      else if (ReadArc(_structure));
      else if (ReadCircle(_structure));
      else if (ReadBox( _structure));
      else if (ReadNode(_structure));
		else if (ReadArrayReference(_structure,_GdsII));
      else if (ReadStructureReference(_structure,_GdsII));
		else if (ReadImage(_structure));
      else if (*_c=='\0')
      	break;
      else
         return false;

      if (!ReadItem("endel"))
         throw GDS_Error("ENDEL is missing (in KEY-file) (too many points in element?)");
   }

	_GdsII->CalcBoundingBox();
   return true;
}
// Contructor ---------------------------------------------------------------
KEY_parser::KEY_parser()
{
	_linenumber=0;
	_back=false;

	_keyword.Clear();
	_value.Clear();
}


// Destructor ---------------------------------------------------------------
KEY_parser::~KEY_parser()
{
}

bool KEY_parser::SkipXYData()
{
	if (!ReadItem("xy"))
		return false;

   while (1)
	{
		if ((ReadItem("x")) ||
      	 (ReadItem("y")) ||
      	 (ReadItem("xm")) ||
      	 (ReadItem("ym")) ||
      	 (ReadItem("xo")) ||
      	 (ReadItem("yo"))
         )
         continue;
      else
         break;
	}

	return true;
}

// Read GdsII (Main routine) ------------------------------------------------
void KEY_parser::Read(GdsII* _GdsII)
{
//	<stream format>	::=	HEADER
//									BGNLIB
//									<LibrarySettings>
//									{<Structure>}*
//									ENDLIB

	if (!ReadGdsHeader(_GdsII))
		throw GDS_Error("HEADER is missing (in KEY-file)");
	if (!ReadBgnlib(_GdsII))
		throw GDS_Error("BGNLIB is missing (in KEY-file)");
	if (!ReadLibrarySettings(_GdsII))
		throw GDS_Error("LIBNAME is missing (in KEY-file)");
	while (ReadStructure(_GdsII))
		;
	if (!ReadItem("endlib"))
		throw GDS_Error("ENDLIB is missing (in KEY-file)");

	//	Process new data for ...

	// references to other structure or known
	// by name now set the pointers in those references
	// to the right structures
	_GdsII->LinkReferences();

	//the topstructure is the structure that is not referenced
	_GdsII->SetTopStructure(_GdsII->SearchTopStructure());

	_GdsII->SetShowStructure(_GdsII->GetTopStructure());

	_GdsII->SetReferenceNr();
	_GdsII->CalcBoundingBox();
}


// Header -------------------------------------------------------------------
bool KEY_parser::ReadGdsHeader(GdsII* _GdsII)
{
	if (!ReadItem("header") )	// Header expected here
		return false;

	_GdsII->SetVersion(atoi(_value));

	return true;
}


// Bgnlib -------------------------------------------------------------------
bool KEY_parser::ReadBgnlib(GdsII* _GdsII)
{
	if( !ReadItem("bgnlib") )	// BgnLib expected here
		return false;
	if( !ReadItem("lastmod") )
		throw GDS_Error("BGNLIB: LASTMOD is missing (in KEY-file)");

	ModeTime* bgnlib = _GdsII->GetBgnlib();
	wxString _int_str_ptr=_value;

	bgnlib->SetLmtYear(atoi(_int_str_ptr));
	_int_str_ptr=strstr(_int_str_ptr,"-")+1;
	bgnlib->SetLmtMonth(atoi(_int_str_ptr));
	_int_str_ptr=strstr(_int_str_ptr,"-")+1;
	bgnlib->SetLmtDay(atoi(_int_str_ptr));
	_int_str_ptr=strstr(_int_str_ptr,"  ")+2;
	bgnlib->SetLmtHour(atoi(_int_str_ptr));
	_int_str_ptr=strstr(_int_str_ptr,":")+1;
	bgnlib->SetLmtMinute(atoi(_int_str_ptr));
	_int_str_ptr=strstr(_int_str_ptr,":")+1;
	bgnlib->SetLmtSecond(atoi(_int_str_ptr));

	if( !ReadItem("lastacc") )
		throw GDS_Error("BGNLIB: LASTACC is missing (in KEY-file)");

	_int_str_ptr=_value;
	bgnlib->SetLatYear(atoi(_int_str_ptr));
	_int_str_ptr=strstr(_int_str_ptr,"-")+1;
	bgnlib->SetLatMonth(atoi(_int_str_ptr));
	_int_str_ptr=strstr(_int_str_ptr,"-")+1;
	bgnlib->SetLatDay(atoi(_int_str_ptr));
	_int_str_ptr=strstr(_int_str_ptr,"  ")+2;
	bgnlib->SetLatHour(atoi(_int_str_ptr));
	_int_str_ptr=strstr(_int_str_ptr,":")+1;
	bgnlib->SetLatMinute(atoi(_int_str_ptr));
	_int_str_ptr=strstr(_int_str_ptr,":")+1;
	bgnlib->SetLatSecond(atoi(_int_str_ptr));

	return true;
}


// Bgnstr -------------------------------------------------------------------
bool KEY_parser::ReadBgnstr(Structure* _structure)
{
	if( !ReadItem("bgnstr") )	// BgnStr expected here
		return false;

	if( !ReadItem("creation") )
		throw GDS_Error("BGNSTR: CREATION is missing (in KEY-file)");

	ModeTime* _bgnstr = _structure->GetBgnstr();
	wxString _int_str_ptr=_value;

	_bgnstr->SetLmtYear(atoi(_int_str_ptr));
	_int_str_ptr=strstr(_int_str_ptr,"-")+1;
	_bgnstr->SetLmtMonth(atoi(_int_str_ptr));
	_int_str_ptr=strstr(_int_str_ptr,"-")+1;
	_bgnstr->SetLmtDay(atoi(_int_str_ptr));
	_int_str_ptr=strstr(_int_str_ptr,"  ")+2;
	_bgnstr->SetLmtHour(atoi(_int_str_ptr));
	_int_str_ptr=strstr(_int_str_ptr,":")+1;
	_bgnstr->SetLmtMinute(atoi(_int_str_ptr));
	_int_str_ptr=strstr(_int_str_ptr,":")+1;
	_bgnstr->SetLmtSecond(atoi(_int_str_ptr));

	if( !ReadItem("lastmod") )
		throw GDS_Error("BGNSTR: LASTMOD is missing (in KEY-file)");

	_int_str_ptr=_value;
	_bgnstr->SetLatYear(atoi(_int_str_ptr));
	_int_str_ptr=strstr(_int_str_ptr,"-")+1;
	_bgnstr->SetLatMonth(atoi(_int_str_ptr));
	_int_str_ptr=strstr(_int_str_ptr,"-")+1;
	_bgnstr->SetLatDay(atoi(_int_str_ptr));
	_int_str_ptr=strstr(_int_str_ptr,"  ")+2;
	_bgnstr->SetLatHour(atoi(_int_str_ptr));
	_int_str_ptr=strstr(_int_str_ptr,":")+1;
	_bgnstr->SetLatMinute(atoi(_int_str_ptr));
	_int_str_ptr=strstr(_int_str_ptr,":")+1;
	_bgnstr->SetLatSecond(atoi(_int_str_ptr));

	return true;
}


// LibrarySettings ----------------------------------------------------------
bool KEY_parser::ReadLibrarySettings(GdsII* _GdsII)
{
/*
<LibrarySettings>	::=	LIBNAME [REFLIBS]
								[FONTS] [ATTRTABLE] [GENERATION]
								[<FormatType>] UNITS
*/
	Library* _library = _GdsII->GetLibrary();

	// Must be there
	if (!ReadLibraryName(_library))
		throw GDS_Error("LIBNAME is missing (in KEY-file)");

	// Optional
	ReadRefLibs(_library);
	ReadFonts(_library);
	ReadAttrtable(_library);
	ReadGenerations(_library);
	ReadFormatType(_library);
	// Must be there
	if (!ReadUnits(_library))
		throw GDS_Error("UNITS is missing (in KEY-file)");

	return true;
}


// LibName ------------------------------------------------------------------
bool KEY_parser::ReadLibraryName(Library* _library)
{
	if( !ReadItem("libname") )	// BgnLib expected here
		return false;

	_library->SetLibraryName(_value);

	return true;
}


// Units --------------------------------------------------------------------
bool KEY_parser::ReadUnits(Library* _library)
{
	if( !ReadItem("units") )	// Units expected here
		return false;

	if( !ReadItem("userunits") )
		throw GDS_Error("USERUNITS missing (in KEY-file)");
	_library->SetUserUnits((double)atof(_value));
   double userunits=(double)atof(_value);
	if( !ReadItem("physunits") )
		throw GDS_Error("PHYSUNITS missing (in KEY-file)");
	_library->SetPhysicalUnits(((double)atof(_value))/userunits);

	return true;
}


// Structure ----------------------------------------------------------------
bool KEY_parser::ReadStructure(GdsII* _structurelist)
{
//	<Structure>			::=	BGNSTR STRNAME [STRCLASS]
//									{<element>}* ENDSTR

	Structure* _structure = new Structure;
	if (!ReadBgnstr(_structure))
		return false;

	if (!ReadItem("strname"))
		throw GDS_Error("Structure: STRNAME missing (in KEY-file)");
	_structure->SetStructureName(_value);

	ReadItem("strclass");	// Just read it

	while (ReadElement(_structure))
		;

	if (!ReadItem("endstr"))
	{
		wxString errbuf;
		errbuf.Printf("Unknown Element: %s (in KEY-file)", _keyword.c_str() );
		throw GDS_Error((char*)errbuf.c_str());
//		throw GDS_Error("Structure: ENDSTR is missing, %s instead (in KEY-file)");
	}

  	_structurelist->insend(_structure);

	return true;
}


// ReadElement () --------------------------------------------------------------------
bool KEY_parser::ReadElement(Structure* _structure)
{
//	<element>			::=	{<boundary> | <path> | <SREF> | <AREF> | <text> | <node> | <box>}
//									/* {<property>}* */
//									ENDEL

	if  (ReadBoundary(_structure));
	else if (ReadStructureReference(_structure));
	else if (ReadPath(_structure));
	else if (ReadText(_structure));
	else if (ReadArc(_structure));
	else if (ReadCircle(_structure));
	else if (ReadLine(_structure));
	else if (ReadBox( _structure));
	else if (ReadArrayReference(_structure));
	else if (ReadNode(_structure));
	else if (ReadImage(_structure));
	else
		return false;

	if (!ReadItem("endel"))
		throw GDS_Error("ENDEL is missing (in KEY-file) (too many points in element?)");
	return true;
}


// Elements -----------------------------------------------------------------

// Boundary -----------------------------------------------------------------
bool KEY_parser::ReadBoundary(Structure* _structure)
{
// <boundary>	::=	BOUNDARY [ELFLAGS] [PLEX] LAYER DATATYPE [WIDTH] XY

	if (!ReadItem("boundary"))
		return false;

	Boundary* _boundary = new Boundary();

	// Optional
	ReadElflags(_boundary);
	ReadItem("plex");
	// Must be there
	if (!ReadLayer(_boundary))
		throw GDS_Error("Boundary: LAYER is missing (in KEY-file)");
	ReadDataType(_boundary);
//	if (!ReadDataType(_boundary))
//		throw GDS_Error("Boundary: DATATYPE is missing (in KEY-file)");
	if (ReadItem("spline"))
   {
		_boundary->SetSpline(atoi(_value) > 0);
	}
	ReadWidth(_boundary);
	if (GLOB->SETUP->Get_Read(_boundary->GetLayer()))
   {
      if (!Read(_boundary->GetPolygon()))
         throw GDS_Error("Boundary: XY is missing (in KEY-file)");

		// Add this Element to the end of the list
	  	_structure->insend(_boundary);
	 	ReadProperties(_boundary);
   }
   else
   {
		SkipXYData();
      delete _boundary;
   }


	return true;
}


// Path ---------------------------------------------------------------------
bool KEY_parser::ReadPath(Structure* _structure)
{
//	<path>				::=	PATH [ELFLAGS] [PLEX] LAYER
//									DATATYPE [PATHTYPE] [WIDTH]
//									[BGNEXTN] [ENDEXTN] XY

	if (!ReadItem("path"))
		return false;

	Path* _path = new Path();

	// Optional
	ReadElflags(_path);
	ReadItem("plex");
	// Must be there
	if (!ReadLayer(_path))
		throw GDS_Error("Path: LAYER is missing (in KEY-file)");
	ReadDataType(_path);
//	if (!ReadDataType(_path))
//		throw GDS_Error("Path: DATATYPE is missing (in KEY-file)");
	// Optional

	if (ReadItem("pathtype"))
   {
		_path->SetPathtype((PATHTYPE)atoi(_value));
	}

	if (ReadItem("spline"))
   {
		_path->SetSpline(atoi(_value) > 0);
	}

	ReadWidth(_path);
	ReadItem("bgnextn");
	ReadItem("endextn");

	if (GLOB->SETUP->Get_Read(_path->GetLayer()))
   {
      if (!Read(_path->GetPolyline()))
         throw GDS_Error("Path: XY is missing (in KEY-file)");

		// Add this Element to the end of the list
	  	_structure->insend(_path);
	 	ReadProperties(_path);
   }
   else
   {
		SkipXYData();
      delete _path;
   }

	return true;
}



// Text ---------------------------------------------------------------------
bool KEY_parser::ReadText(Structure* _structure)
{
//	<text>				::=	TEXT [ELFLAGS] [PLEX] LAYER
//									<textbody>

	if (!ReadItem("text"))
		return false;

	Text* _text = new Text();

	// Optional
	ReadElflags(_text);
	ReadItem("plex");
	// Must be there
	ReadLayer(_text);
	if (!ReadTextBody(_text))
		throw GDS_Error("Text: TEXTBODY is missing (in KEY-file)");

	if (GLOB->SETUP->Get_Read(_text->GetLayer()))
   {
		// Add this Element to the end of the list
	  	_structure->insend(_text);
	 	ReadProperties(_text);
   }
   else
      delete _text;


	return true;
}


// Arrayreference -----------------------------------------------------------
bool KEY_parser::ReadArrayReference(Structure* _structure)
{
//	<AREF>				::=	AREF [ELFLAGS] [PLEX] SNAME
//									[<strans>] COLROW XY
//	<strans>				::=	STRANS [MAG] [ANGLE]

	if (!ReadItem("aref"))
		return false;


	Point* _point;
   wxTransformMatrix*_relative_matrix;
   ArrayReference* _aref;
   Strans* _strans;
   Point* _HorPoint;
   Point* _VerPoint;
   try
   {
	   _aref = new ArrayReference();

      ReadElflags(_aref);
      ReadItem("plex");
      // Must be there

      if (!ReadItem("sname"))
         throw GDS_Error("Aref: SNAME missing (in Structure Reference)");
      _aref->SetStructureReferenceName(_value);

      _strans = new Strans();
      ReadStrans(_strans);

      if (!ReadItem("colrow"))
         throw GDS_Error("Aref: COLROW is missing (in Array Reference)");

      wxString _int_str_ptr=_value;
      _aref->SetColumns(atoi(_int_str_ptr));
      _int_str_ptr=strstr(_int_str_ptr,",")+1;
      _aref->SetRows(atoi(_int_str_ptr));

      if (!ReadItem("xy") )
         throw GDS_Error("Aref: XY missing (in Array Reference)");

      _point = new Point();
      Read(_point);

      _HorPoint = new Point();
      Read(_HorPoint);
      _aref->SetHorzSpace(_HorPoint);

      _VerPoint = new Point();
      Read(_VerPoint);
      _aref->SetVertSpace(_VerPoint);

      // Ok, now we have a strans, but we want to work with a matrix,
      // so let's convert it to one:

      _relative_matrix = new wxTransformMatrix();

      // Mirror in X if necessary
      if (_strans->GetReflection())
         _relative_matrix->Mirror();

      //  Rotate if necessary
      if (_strans->GetAbsAngle() == 0)
         _relative_matrix->Rotate(_strans->GetAngle(), 0, 0);

      // Scale if necessary
      if (_strans->GetAbsScale() == 0)
      {
         double scale = _strans->GetScale();
         _relative_matrix->Scale(scale, scale, 0, 0);
      }

      // Translate2D over XY from the structrereference
      _relative_matrix->Translate(_point->GetX(), _point->GetY());

      _aref->SetRelativeMatrix(_relative_matrix);

      _structure->insend(_aref);
      delete _point;
      delete _strans;
      delete _relative_matrix;

      ReadProperties(_aref);
   }
	catch (GDS_Error& _error)
   {
      delete _aref;
      delete _point;
      delete _HorPoint;
      delete _VerPoint;
      delete _strans;
      delete _relative_matrix;
      throw _error;
   }

	return true;
}


// StructureReference -------------------------------------------------------
bool KEY_parser::ReadStructureReference(Structure* _structure)
{
//	<SREF>				::=	SREF [ELFLAGS] [PLEX] SNAME
//									[<strans>] XY
//	<strans>				::=	STRANS [MAG] [ANGLE]

	if (!ReadItem("sref"))
		return false;
	Point* _point=0;
   wxTransformMatrix* _relative_matrix=0;
   StructureReference* _sref=0;
   Strans* _strans=0;
   try
   {
      _sref = new StructureReference();

      ReadElflags(_sref);
      ReadItem("plex");
      // Must be there

      if (!ReadItem("sname"))
         throw GDS_Error("Sref: SNAME is missing (in KEY-file)");
      _sref->SetStructureReferenceName(_value);

      _strans = new Strans();
      ReadStrans(_strans);

      if (!ReadItem("xy") )
         throw GDS_Error("Sref: XY is missing (in KEY-file)");
      _point = new Point();
      Read(_point);

      // Ok, now we have a strans, but we want to work with a matrix,
      // so let's convert it to one:

      _relative_matrix = new wxTransformMatrix();

      // Mirror in X if necessary
      if (_strans->GetReflection())
         _relative_matrix->Mirror();

      //  Rotate if necessary
      if (_strans->GetAbsAngle() == 0)
         _relative_matrix->Rotate(_strans->GetAngle(), 0, 0);

      // Scale if necessary
      if (_strans->GetAbsScale() == 0)
      {
         double scale = _strans->GetScale();
         _relative_matrix->Scale(scale, scale, 0, 0);
      }

      // Translate2D over XY from the structure reference
      _relative_matrix->Translate(_point->GetX(), _point->GetY());

      _sref->SetRelativeMatrix(_relative_matrix);

      _structure->insend(_sref);
      delete _point;
      delete _strans;
      delete _relative_matrix;

      ReadProperties(_sref);
   }
	catch (GDS_Error& _error)
   {
      delete _sref;
      delete _point;
      delete _strans;
      delete _relative_matrix;
      throw _error;
   }

	return true;
}


// Circle -----------------------------------------------------------------
bool KEY_parser::ReadCircle(Structure* _structure)
{
// <circle>	::=	CIRCLE [ELFLAGS] [PLEX] LAYER [WIDTH] XY

	if (!ReadItem("circle"))
		return false;

	Circle* _circle = new Circle();

	// Optional
	ReadElflags(_circle);
	ReadItem("plex");
	// Must be there
	if (!ReadLayer(_circle))
		throw GDS_Error("Circle: LAYER is missing (in KEY-file)");
	ReadDataType(_circle);
//	if (!ReadDataType(_circle))
//		throw GDS_Error("Circle: DATATYPE is missing (in KEY-file)");

	ReadWidth(_circle);

	if (!ReadItem("xy"))
		throw GDS_Error("Circle: XY is missing (in KEY-file)");
	Point _point;
	Read(&_point);
	_circle->SetMid(_point);
	if (!ReadItem("radius"))
		throw GDS_Error("Circle: RADIUS is missing (in KEY-file)");
	_circle->SetRadius(_factor*atof(_value));

	if (GLOB->SETUP->Get_Read(_circle->GetLayer()))
   {
		// Add this Element to the end of the list
	  	_structure->insend(_circle);
	  	ReadProperties(_circle);
   }
   else
      delete _circle;

	return true;
}


bool KEY_parser::ReadArc(Structure* _structure)
{
// <arc>			::=	ARC [ELFLAGS] [PLEX] LAYER [WIDTH] XY

	if (!ReadItem("arc"))
		return false;

	Arcseg* _Arcseg = new Arcseg();

	// Optional
	ReadElflags(_Arcseg);
	ReadItem("plex");
	// Must be there
	if (!ReadLayer(_Arcseg))
		throw GDS_Error("Arc: LAYER is missing (in KEY-file)");
	ReadDataType(_Arcseg);
//	if (!ReadDataType(_Arcseg))
//		throw GDS_Error("Arc: DATATYPE is missing (in KEY-file)");

	ReadWidth(_Arcseg);

	if (GLOB->SETUP->Get_Read(_Arcseg->GetLayer()))
   {
      if (!Read(_Arcseg->GetPolyline()))
         throw GDS_Error("Arc: XY is missing (in KEY-file)");
		// Add this Element to the end of the list
	  	_structure->insend(_Arcseg);

	  	ReadProperties(_Arcseg);
   }
   else
   {
		SkipXYData();
      delete _Arcseg;
   }
	return true;
}

bool KEY_parser::ReadNode(Structure* /*_structure*/)
{
// <node>	::=	NODE [ELFLAGS] [PLEX] LAYER NODETYPE [WIDTH] XY

	// not implemented
	if (ReadItem("node"))
		throw GDS_Error("Node: NODE not implemented yet! (in KEY-file)");
	return false;
}

// Box -----------------------------------------------------------------
bool KEY_parser::ReadBox(Structure* _structure)
{
// <box>	::=	BOX [ELFLAGS] [PLEX] LAYER BOXTYPE [WIDTH] XY

	if (!ReadItem("box"))
		return false;

	Box* _box = new Box();

	// Optional
	ReadElflags(_box);
	ReadItem("plex");

	// Must be there
	if (!ReadLayer(_box))
		throw GDS_Error("Box: LAYER is missing (in KEY-file)");
	ReadBoxType(_box);
//	if (!ReadBoxType(_box))
//		throw GDS_Error("Box: BOXTYPE is missing (in KEY-file)");

	ReadWidth(_box);

	if (GLOB->SETUP->Get_Read(_box->GetLayer()))
   {
	   G_Polygon* orgi = new G_Polygon();
      if (!Read(orgi))
         throw GDS_Error("Box: XY is missing (in KEY-file)");

      _box->Set(orgi);

      // Add this Element to the end of the list
      _structure->insend(_box);
      delete orgi;

	  	ReadProperties(_box);
   }
   else
   {
		SkipXYData();
      delete _box;
   }
	return true;
}

// SLine -----------------------------------------------------------------
bool KEY_parser::ReadLine(Structure* _structure)
{
	if (!ReadItem("line"))
		return false;

	SLine* line = new SLine(0);

	// Optional
	ReadElflags(line);
	ReadItem("plex");

	// Must be there
	if (!ReadLayer(line))
		throw GDS_Error("Line: LAYER is missing (in KEY-file)");

	ReadWidth(line);

	if (GLOB->SETUP->Get_Read(line->GetLayer()))
   {
	   G_Polyline* orgi = new G_Polyline();
      if (!Read(orgi))
         throw GDS_Error("Line: XY is missing (in KEY-file)");
      {
         TDLI<Segment> 	Iter=TDLI<Segment>(orgi);
         Iter.tohead();
         double x1=Iter.item()->GetX();
         double y1=Iter.item()->GetY();
         Iter++;
         line->Set(x1,y1,Iter.item()->GetX(),Iter.item()->GetY());
      }

      // Add this Element to the end of the list
      _structure->insend(line);
      delete orgi;

	  	ReadProperties(line);
   }
   else
   {
		SkipXYData();
      delete line;
   }
	return true;
}

// Image -----------------------------------------------------------------
bool KEY_parser::ReadImage(Structure* _structure)
{
	if (!ReadItem("image"))
		return false;

	Image* _image = new Image();

	if (!ReadLayer(_image))
		throw GDS_Error("Image: LAYER is missing (in KEY-file)");

	ReadWidth(_image);

   double w,h;
   w=h=1;
   if (ReadItem("w"))
   {
      _image->SetW(_factor*atof(_value));
      w=_factor*atof(_value);
   }

   if (ReadItem("h"))
   {
      _image->SetH(_factor*atof(_value));
      h=_factor*atof(_value);
   }

   if (ReadItem("type"))
      _image->SetPictureType(_value);

   if (ReadItem("path"))
      _image->SetPath(_value);

	Strans* _strans = new Strans();
	ReadStrans(_strans);

	if (GLOB->SETUP->Get_Read(_image->GetLayer()))
   {
      if (!ReadItem("xy"))
         throw GDS_Error("Image: XY is missing (in KEY-file)");
      Point* _point = new Point();
      Read(_point);

      // Ok, now we have a strans, but we want to work with a matrix,
      // so let's convert it to one:
      wxTransformMatrix*_relative_matrix = new wxTransformMatrix();

      // Mirror in X if necessary
      if (_strans->GetReflection())
         _relative_matrix->Mirror();


      // Scale if necessary
      if (_strans->GetAbsScale() == 0)
      {
         double scale = _strans->GetScale();
         _relative_matrix->Scale(scale,scale, 0, 0);
      }

      //  Rotate if necessary
      if (_strans->GetAbsAngle() == 0)
         _relative_matrix->Rotate(_strans->GetAngle(), 0, 0);

      // Translate2D over XY from the structrereference
      _relative_matrix->Translate(_point->GetX(), _point->GetY());

      _image->SetRelativeMatrix(_relative_matrix);

      // Add this Element to the end of the list
      _structure->insend(_image);

	  	ReadProperties(_image);
      delete _point;
      delete _relative_matrix;
   }
   else
   {
		SkipXYData();
      delete _image;
   }
	delete _strans;
	return true;
}

// End of Elements ----------------------------------------------------------




// Elflags ------------------------------------------------------------------
bool KEY_parser::ReadElflags(Element* _element)
{
	if (!ReadItem("elflags"))
		return false;

	_element->SetTemplate( (bool)(atoi(_value)&&1) );
	_element->SetExternal( (bool)(atoi(_value)&&2) );

	return true;
}


// Layer --------------------------------------------------------------------
bool KEY_parser::ReadLayer(Shape* _shape)
{
	if (!ReadItem("layer"))
		return false;
	_shape->SetLayer(_mapping_in[atoi(_value)]);
	GLOB->SETUP->Set_Available(atoi(_value), true);
	return true;
}


// Datatype -----------------------------------------------------------------
bool KEY_parser::ReadDataType(Boundary* _boundary)
{
	if (!ReadItem("datatype"))
		return false;

	_boundary->SetDatatype(atoi(_value));
	return true;
}

// Datatype -----------------------------------------------------------------
bool KEY_parser::ReadDataType(Path* _path)
{
	if (!ReadItem("datatype"))
		return false;

	_path->SetDatatype(atoi(_value));
	return true;
}


// Datatype -----------------------------------------------------------------
bool KEY_parser::ReadDataType(Arcseg* _arcseg)
{
	if (!ReadItem("datatype"))
		return false;

	_arcseg->SetDatatype(atoi(_value));
	return true;
}


// Datatype -----------------------------------------------------------------
bool KEY_parser::ReadDataType(Circle* _circle)
{
	if (!ReadItem("datatype"))
		return false;

	_circle->SetDatatype(atoi(_value));
	return true;
}


// Boxtype -----------------------------------------------------------------
bool KEY_parser::ReadBoxType(Box* _box)
{
	if (!ReadItem("boxtype"))
		return false;

	_box->SetBoxtype(atoi(_value));
	return true;
}


// Polygon ------------------------------------------------------------------
bool KEY_parser::Read(G_Polygon* _polygon)
{
	if (!ReadItem("xy"))
		return false;

	int j=atoi(_value);
	int i;

	Segment* segn;
	double x,y,xm,ym,xo,yo;

	for (i=1;i<j;i++)
	{
      HOLEFLAGS t=NORMAL_SEG;
		if (ReadItem("st")) //read segtype
      { if (stricmp(_value,"N")==0)
           t=NORMAL_SEG;
        else if (stricmp(_value,"H")==0)
        {
           _polygon->SetHoles(true);
           t=HOLE_SEG;
        }
        else if (stricmp(_value,"L")==0)
           t=LINK_SEG;
      }

		if (!ReadItem("x")) //read x
			throw GDS_Error("Polygon: X is missing (in KEY-file)");

		x=_factor*atof(_value);

		if (!ReadItem("y")) //read y
			throw GDS_Error("Polygon: Y is missing (in KEY-file)");

		y=_factor*atof(_value);

		// Read XM or X
		if (ReadItem("xm")) //an arc segment is coming
		{
         _polygon->SetArcs(true);
			Arc_Seg* seg=new Arc_Seg();
			segn=(Segment*) seg;
			xm=_factor*atof(_value);
			seg->SetX(x);
			seg->SetY(y);

			if (!ReadItem("ym")) //read ym of arc
				throw GDS_Error("Polygon: YM is missing (in KEY-file)");
			ym=_factor*atof(_value);
			seg->SetAX(xm);
			seg->SetAY(ym);

			if (!ReadItem("xo")) //read xo of arc
				throw GDS_Error("Polygon: XO is missing (in KEY-file)");
			xo=_factor*atof(_value);

			if (!ReadItem("yo")) //read yo of arc
				throw GDS_Error("Polygon: YO is missing (in KEY-file)");
			yo=_factor*atof(_value);

			seg->SetOX(xo);
			seg->SetOY(yo);
         seg->SetHoleType(t);
		}
		else if ( !stricmp(_keyword,"x") || !stricmp(_keyword,"st") )	// Read is already done
		{
			_back = true;
			Line_Seg* seg=new Line_Seg();
			segn=(Segment*) seg;
			seg->SetX(x);
			seg->SetY(y);
         seg->SetHoleType(t);
		}
		else
			throw GDS_Error("Polygon: X or XM missing (in KEY-file)");

		_polygon->AddSegment(segn);
	}
	if (!ReadItem("x"))
		throw GDS_Error("Polygon: X is missing (in KEY-file)");
	// Skip last coordinate (same as first one)
	if (!ReadItem("y"))	// Skip last X
		throw GDS_Error("Polygon: Y is missing (in KEY-file)");
	return true;
}

// Width --------------------------------------------------------------------
bool KEY_parser::ReadWidth(Shape* _shape)
{
	if (!ReadItem("width"))
		return false;

	_shape->SetWidth(_factor*atof(_value));
	return true;
}


// Polyline -----------------------------------------------------------------
bool KEY_parser::Read(G_Polyline* _polyline)
{
	if (!ReadItem("xy"))
		throw GDS_Error("Polyline: XY is missing (in KEY-file)");

	int j=atoi(_value);
	int i;
	Segment* segn;
	double x,y,xm,ym,xo,yo;

	for (i=1;i<=j;i++)
	{
		if (!ReadItem("x")) //read x
			throw GDS_Error("Polyline: X is missing (in KEY-file)");
		x=_factor*atof(_value);

		if (!ReadItem("y")) //read y
			throw GDS_Error("Polyline: Y is missing (in KEY-file)");
		y=_factor*atof(_value);

		// Read XM or X
		if (ReadItem("xm")) //an arc segment is coming
		{
         _polyline->SetArcs(true);
			Arc_Seg* seg=new Arc_Seg();
			segn=(Segment*) seg;
			xm=_factor*atof(_value);
			seg->SetX(x);
			seg->SetY(y);

			if (!ReadItem("ym")) //read ym of arc
				throw GDS_Error("Polyline: YM is missing (in KEY-file)");
			ym=_factor*atof(_value);
			seg->SetAX(xm);
			seg->SetAY(ym);

			if (!ReadItem("xo")) //read xo of arc
				throw GDS_Error("Polyline: XO is missing (in KEY-file)");
			xo=_factor*atof(_value);

			if (!ReadItem("yo")) //read yo of arc
				throw GDS_Error("Polyline: YO is missing (in KEY-file)");
			yo=_factor*atof(_value);

			seg->SetOX(xo);
			seg->SetOY(yo);
		}
		else if ( !stricmp(_keyword,"x") || !stricmp(_keyword,"endel") || !stricmp(_keyword,"property"))	// Read is already done
		{
			Line_Seg* seg=new Line_Seg();
			segn=(Segment*) seg;
			seg->SetX(x);
			seg->SetY(y);
			_back=true;
		}
		else
			throw GDS_Error("Polyline: X, XM or ENDEL missing (in KEY-file)");
		_polyline->AddSegment(segn);
	}
	// DO NOT: Skip last coordinate (is not always same as 1st)
	return true;
}


// TextBody -----------------------------------------------------------------
bool KEY_parser::ReadTextBody(Text* _text)
{
//	<textbody>			::=	TEXTTYPE [PRESENTATION] [PATHTYPE]
//									[WIDTH] [<strans>] XY STRING
	// Must be there
	if (!ReadItem("texttype"))
		return false;
	_text->SetTexttype(atoi(_value));

	// Optional
	ReadPresentation(_text);

	if (ReadItem("pathtype"))
   {
		_text->SetPathtype((TEXT_PATHTYPE)atoi(_value));
	}

	ReadWidth(_text);
	Strans* _strans = new Strans();
	ReadStrans(_strans);

	if (!ReadItem("xy"))
		throw GDS_Error("Text: XY is missing (in KEY-file)");
	Point* _point = new Point();
	Read(_point);

	// Ok, now we have a strans, but we want to work with a matrix,
   // so let's convert it to one:

   wxTransformMatrix*_relative_matrix = new wxTransformMatrix();

   // Mirror in X if necessary
   if (_strans->GetReflection())
      _relative_matrix->Mirror();

   //  Rotate if necessary
   if (_strans->GetAbsAngle() == 0)
      _relative_matrix->Rotate(_strans->GetAngle(), 0, 0);

   // Scale if necessary
   if (_strans->GetAbsScale() == 0)
   {
      double scale = _strans->GetScale();
      _relative_matrix->Scale(scale, scale, 0, 0);
   }

   // Translate2D over XY from the structrereference
   _relative_matrix->Translate(_point->GetX(), _point->GetY());

	_text->SetRelativeMatrix(_relative_matrix);

	if (!ReadItem("string") )
		throw GDS_Error("Text: STRING missing (in Text)");
	_text->SetText(_value);
	delete _strans;
   delete _point;
   delete _relative_matrix;
	return true;
}


// Presentation -------------------------------------------------------------
bool KEY_parser::ReadPresentation(Text* _text)
{
	if (!ReadItem("presentation"))
		return false;

	wxString _int_str_ptr=_value;
	_text->SetHorizontal( atoi(_int_str_ptr)%4 );
	_int_str_ptr=strstr(_int_str_ptr,",")+1;
	_text->SetVertical( atoi(_int_str_ptr)%4 );
	_int_str_ptr=strstr(_int_str_ptr,",")+1;
	_text->SetFont( atoi(_int_str_ptr)%4 );

	return true;
}


// Strans -------------------------------------------------------------------
bool KEY_parser::ReadStrans(Strans* _strans)
{
//	<strans>				::=	STRANS [MAG] [ANGLE]
	if (!ReadItem("strans"))
		return false;

	wxString _int_str_ptr=_value;

	_strans->_stransflags.bits.reflection=atoi(_int_str_ptr);
	_int_str_ptr=strstr(_int_str_ptr,",")+1;
	_strans->_stransflags.bits.abs_angle=atoi(_int_str_ptr);
	_int_str_ptr=strstr(_int_str_ptr,",")+1;
	_strans->_stransflags.bits.abs_scale=atoi(_int_str_ptr);

	if (ReadItem("mag"))
		_strans->SetScale(_factor*atof(_value));

	if (ReadItem("angle"))
		_strans->SetAngle(atof(_value));

	return true;
}


// Point --------------------------------------------------------------------
bool KEY_parser::Read(Point* _point)
{
	if (!ReadItem("x"))
		throw GDS_Error("Point: X is missing (in KEY-file)");
	_point->SetX( _factor*atof(_value));
	if (!ReadItem("y"))
		throw GDS_Error("Point: Y is missing (in KEY-file)");
	_point->SetY( _factor*atof(_value) );
	return true;
}


// RefLibs ------------------------------------------------------------------
bool KEY_parser::ReadRefLibs(Library* /*_library*/)
{
	if (!ReadItem("reflibs"))	// RefLibs optional
		return false;

	// Handle RefLibs
	return true;
}


// Fonts --------------------------------------------------------------------
bool KEY_parser::ReadFonts(Library* /*_library*/)
{
	if (!ReadItem("fonts"))
		return false;

	// Handle Fonts
	return true;
}


// Attrtable ----------------------------------------------------------------
bool KEY_parser::ReadAttrtable(Library* /*_library*/)
{
	if (!ReadItem("attrtable"))
		return false;

	// Handle AttrTable
	return true;
}


// Generations --------------------------------------------------------------
bool KEY_parser::ReadGenerations(Library* /*_library*/)
{
	if (!ReadItem("generations"))
		return false;

	// Handle Generations
	return true;
}


// FormatType ---------------------------------------------------------------
bool KEY_parser::ReadFormatType(Library* /*_library*/)
{
	if (!ReadItem("format"))
		return false;

	// Handle Format
//	throw GDS_Error("FORMAT is not used (in KEY-file)");
	if (ReadItem("mask"))
		while(!ReadItem("endmasks"))
			;	// Just do it
	return true;
}

bool KEY_parser::ReadProperties(Element* _element)
{
	if (!ReadItem("property"))
		return false;

   do{
      char name[80];
      char value[80];
      char type[80];
      strcpy(name,_value);

      if (ReadItem("proptype"))
	      strcpy(type,_value);
      if (ReadItem("propvalue"))
	      strcpy(value,_value);
	   _element->AddProperty(name,type,value);
   }
   while (ReadItem("property"));
   return true;
}
