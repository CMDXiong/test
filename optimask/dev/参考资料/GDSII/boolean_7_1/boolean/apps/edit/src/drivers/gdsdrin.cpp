/*
Program		GDSDRV.CPP
Purpose		GdsII parser
Last Update		17-3-1998
*/


#ifdef __GNUG__
#pragma implementation
#endif

#include "gdsdrin.h"

#include "boundary.h"
#include "elemlst.h"
#include "gdsii.h"
#include "path.h"
#include "arc.h"
#include "point.h"
#include "segment.h"
#include "polyline.h"
#include "referenc.h"
#include "structur.h"
#include "text.h"
#include "units.h"
#include "trans.h"

/*
GDS-II:
<stream format>	::=	HEADER BGNLIB LIBNAME [REFLIBS]
								[FONTS] [ATTRTABLE] [GENERATION]
								[<FormatType>] UNITS {<Structure>}*
								ENDLIB
<FormatType>		::=	FORMAT | FORMAT {MASK}+ ENDMASK
<Structure>			::=	BGNSTR STRNAME [STRCLASS]
								{<element>}* ENDSTR
<element>			::=	{<boundary> | <path> | <SREF> |
								<AREF> | <text> | <node> | <bbox>}
								{<property>}* ENDEL
<boundary>			::=	BOUNDARY [ELFLAGS] [PLEX] LAYER
								DATATYPE XY
<path>				::=	PATH [ELFLAGS] [PLEX] LAYER
								DATATYPE [PATHTYPE] [WIDTH]
								[BGNEXTN] [ENDEXTN] XY
<SREF>				::=	SREF [ELFLAGS] [PLEX] SNAME
								[<strans>] XY
<AREF>				::=	AREF [ELFLAGS] [PLEX] SNAME
								[<strans>] COLROW XY
<text>				::=	TEXT [ELFLAGS] [PLEX] LAYER
								<textbody>
<node>				::=	NODE [ELFLAGS] [PLEX] LAYER
								NODETYPE XY
<box>					::=	BOX [ELFLAGS] [PLEX] LAYER
								BOXTYPE XY
<textbody>			::=	TEXTTYPE [PRESENTATION] [PATHTYPE]
								[WIDTH] [<strans>] XY STRING
<property>			::=	PROPATTR PROPVALUE
*/


GDS_driver_in::GDS_driver_in(const wxString& filename)
{
   GDS_filename=filename;

	_recordsize      =0;
	_recordtype      =0;
	_datatype        =0;
   _back            = false;

	int i;
    for (i = 0; i < 999; i++)
		   _mapping_in[i]=-1;
	for (i = 0; i < MAX_LAYERS; i++)
		   _mapping_in[GLOB->SETUP->Get_GdsInMapping(i)]=i;
	for (i = 0; i < 999; i++)
       if (_mapping_in[i]==-1)
		   	_mapping_in[i]=0;

	GDS_file.open(GDS_filename, ios::binary | ios::in);
	if (GDS_file.bad())
	{
		char errbuf[100];
		sprintf(errbuf, "Cannot open file %s!",GDS_filename.c_str() );
		throw GDS_Error(errbuf, "GDSParser: File Error", 9, 0);
	}

}

GDS_driver_in::~GDS_driver_in()
{
	GDS_file.close();
}

bool GDS_driver_in::ReadHeader(int type)
{
	if (_back)
	{  if (_recordtype == type)
		{
			_back=false;
			return true;
		}
	}
	else
	{
		if (GDS_file.eof())
			throw GDS_Error("Unexpected end of GDSII-file!","Driver Error", 9, 0);

		GDS_file >> &_recordsize
					>> &_recordtype
					>> &_datatype;

		_recordsize -= (TWO_G_BYTE_UNSIGNED_INTEGER) GDS_HEADERSIZE;

		if (_recordtype == type)
			return true;
		if (_recordtype>56)
		{
//			throw GDS_Error("Unknown Record Type! Record is not part of the GDS-II Stream Format v5.2","Driver Error", 9, 0);
			SkipData();
		}
		_back=true;
	}
	return false;
}

void GDS_driver_in::SkipData()
{
	G_BYTE a_byte;
	for (int i=0;i < _recordsize; i++)
	{
		if (GDS_file.eof())
			throw GDS_Error("Unexpected end of file while reading!");
		GDS_file >> &a_byte;
	}

	_back=false;
	_recordsize      =0;
	_recordtype      =0;
	_datatype        =0;
}

void GDS_driver_in::ReadString(wxString& a_string)
{  int i;
	for (i = 0 ; i < _recordsize; i++)
		if (GDS_file.eof())
			throw GDS_Error("Unexpected end of file");
		else
      {
         char a;
			GDS_file.get(a);
         if ( a != 0 )
            a_string += a;
      }
}

// Read operations for GDSII-files



// Bgnlib -------------------------------------------------------------------
bool GDS_driver_in::ReadBgnlib(GdsII* _GdsII)
{
	if (!ReadHeader(GDS_BGNLIB))
       return(false);

	ModeTime* _Bgnlib = _GdsII->GetBgnlib();

	TWO_G_BYTE_SIGNED_INTEGER value;
	GDS_file >> &value;
	_Bgnlib->SetLmtYear(value);
	GDS_file >> &value;
	_Bgnlib->SetLmtMonth(value);
	GDS_file >> &value;
	_Bgnlib->SetLmtDay(value);
	GDS_file >> &value;
	_Bgnlib->SetLmtHour(value);
	GDS_file >> &value;
	_Bgnlib->SetLmtMinute(value);
	GDS_file >> &value;
	_Bgnlib->SetLmtSecond(value);
	GDS_file >> &value;
	_Bgnlib->SetLatYear(value);
	GDS_file >> &value;
	_Bgnlib->SetLatMonth(value);
	GDS_file >> &value;
	_Bgnlib->SetLatDay(value);
	GDS_file >> &value;
	_Bgnlib->SetLatHour(value);
	GDS_file >> &value;
	_Bgnlib->SetLatMinute(value);
	GDS_file >> &value;
	_Bgnlib->SetLatSecond(value);

	return(true);
}


// Bgnstr -------------------------------------------------------------------
bool GDS_driver_in::ReadBgnstr(Structure* _struct)
{
	if (!ReadHeader(GDS_BGNSTR))
		 return(false);

	ModeTime* _bgnstr = _struct->GetBgnstr();

	TWO_G_BYTE_SIGNED_INTEGER value;
	GDS_file >> &value;
	_bgnstr->SetLmtYear(value);
	GDS_file >> &value;
	_bgnstr->SetLmtMonth(value);
	GDS_file >> &value;
	_bgnstr->SetLmtDay(value);
	GDS_file >> &value;
	_bgnstr->SetLmtHour(value);
	GDS_file >> &value;
	_bgnstr->SetLmtMinute(value);
	GDS_file >> &value;
	_bgnstr->SetLmtSecond(value);
	GDS_file >> &value;
	_bgnstr->SetLatYear(value);
	GDS_file >> &value;
	_bgnstr->SetLatMonth(value);
	GDS_file >> &value;
	_bgnstr->SetLatDay(value);
	GDS_file >> &value;
	_bgnstr->SetLatHour(value);
	GDS_file >> &value;
	_bgnstr->SetLatMinute(value);
	GDS_file >> &value;
	_bgnstr->SetLatSecond(value);

	return(true);
}


// Read Element -------------------------------------------------------------
bool GDS_driver_in::ReadElement(Structure* _structure)
{
//   static int i=0;
	if  (ReadBoundary(_structure));
	else if (ReadPath(_structure));
	else if (ReadStructureReference(_structure));
	else if (ReadArrayReference(_structure));
	else if (ReadText(_structure));
	else if (ReadNode(_structure));
	else if (ReadBox( _structure));
	else
		return false;

	while (ReadProperty(_structure))
		;

//   i++;
	if (!ReadHeader(GDS_ENDEL))
		throw GDS_Error("ENDEL missing (in Element)");

	return true;
}


// Property -----------------------------------------------------------------
bool GDS_driver_in::ReadProperty(Structure* /*_structure*/)
{  int i;
	if (ReadHeader(GDS_PROPATTR))
	{
		// Read Property Attribute
		TWO_G_BYTE_SIGNED_INTEGER _propattr;
		if (GDS_file.eof())
			throw GDS_Error("Unexpected end of file while reading Property Atribute!");
		GDS_file >> &_propattr;
		_back=false;

		// Check (for) Property Value
		if (!ReadHeader(GDS_PROPVALUE))
			throw GDS_Error("PROPVALUE missing (in Property)");

		// Read Property Value
		G_BYTE _propval[126];
		for (i=0;i < _recordsize; i++)
		{
			if (GDS_file.eof())
				throw GDS_Error("Unexpected end of file while reading reading Property Value!");
			GDS_file >> &_propval[i];
		}
		_back=false;
		//	End string
		if (i<=126)
			_propval[i]=0;

		return true;	//	Found one!
	}
	return false;	// Didn't find one.
}


// Boundary -----------------------------------------------------------------
bool GDS_driver_in::ReadBoundary(Structure* _structure)
{
	// Grammar: BOUNDARY [ELFLAGS] [PLEX] LAYER DATATYPE XY
	if (!ReadHeader(GDS_BOUNDARY))
	  return false;

	// Create a new Boundary
	Boundary* _Boundary = new Boundary();

	ReadElflags(_Boundary);

	if (ReadHeader(GDS_PLEX))
		SkipData();

	if (!ReadLayer(_Boundary))
			throw GDS_Error("Boundary: LAYER missing (in GDS-file)");

	if (!ReadDatatype(_Boundary))
		throw GDS_Error("Boundary: DATATYPE missing (in GDS-file)");

	if (GLOB->SETUP->Get_Read(_Boundary->GetLayer()))
   {
		if (!Read(_Boundary->GetPolygon()))
			throw GDS_Error("Header: XY missing (in Boundary)");

		// Add this Element to the end of the list
	  	_structure->insend(_Boundary);
   }
   else
   {
		ReadHeader(GDS_XY);
		SkipData();
      delete _Boundary;
   }

	return true;
}


// Path ---------------------------------------------------------------------
bool GDS_driver_in::ReadPath(Structure* _structure)
{
	// Grammar: PATH [ELFLAGS] [PLEX] LAYER DATATYPE
	//				[PATHTYPE] [WIDTH] [BGNEXTN] [ENDEXTN] XY

	if (!ReadHeader(GDS_PATH))
	  return false;

	// Create a new Path
	Path* _Path = new Path();

	ReadElflags(_Path);

	if (ReadHeader(GDS_PLEX))
		SkipData();

	if (!ReadLayer(_Path))
			throw GDS_Error("record LAYER missing (in Boundary)");

	if (!ReadDatatype(_Path))
		throw GDS_Error("record DATATYPE missing (in Boundary)");

	if (ReadHeader(GDS_PATHTYPE))
   {
      TWO_G_BYTE_SIGNED_INTEGER value;
      GDS_file >> &value;

      _Path->SetPathtype((PATHTYPE)value);
   }

	ReadWidth(_Path);

	if (ReadHeader(GDS_BGNEXTN))
		SkipData();

	if (ReadHeader(GDS_ENDEXTN))
		SkipData();

	if (GLOB->SETUP->Get_Read(_Path->GetLayer()))
   {
		if (!Read(_Path->GetPolyline()))
			throw GDS_Error("Header: XY missing (in Path)");

		// Add this Element to the end of the list
	  	_structure->insend(_Path);
   }
   else
   {
		ReadHeader(GDS_XY);
		SkipData();
      delete _Path;
   }
	return true;
}


// Text ---------------------------------------------------------------------
bool GDS_driver_in::ReadText(Structure* _structure)
{
	// Grammar: TEXT [ELFLAGS] [PLEX] LAYER
	//				TEXTTYPE [PRESENTATION] [PATHTYPE] [WIDTH]
	//				[<strans>] XY STRING
	//
	//				<strans> = STRANS [MAG] [ANGLE]

	if (!ReadHeader(GDS_TEXT))
	  return false;

	// Create a new Text
	Text* _Text = new Text();

	ReadElflags(_Text);

	if (ReadHeader(GDS_PLEX))
		SkipData();

	if (!ReadLayer(_Text))
			throw GDS_Error("Text: LAYER is missing (in GDS-file)");

	if (!ReadTextBody(_Text))
		throw GDS_Error("Text: TEXTBODY is missing (in GDS-file)");

	if (GLOB->SETUP->Get_Read(_Text->GetLayer()))
   {
		// Add this Element to the end of the list
	  	_structure->insend(_Text);
   }
   else
      delete _Text;

	return true;
}


// StructureReference -------------------------------------------------------
bool GDS_driver_in::ReadStructureReference(Structure* _structure)
{
//	<SREF>				::=	SREF [ELFLAGS] [PLEX] SNAME
//									[<strans>] XY
//	<strans>				::=	STRANS [MAG] [ANGLE]

	if (!ReadHeader(GDS_SREF))
		return false;
	Point* _point=0;
   wxTransformMatrix*_relative_matrix=0;
   StructureReference* _sref=0;
   Strans* _strans=0;
   try
   {
      _sref = new StructureReference();

      ReadElflags(_sref);

      if (ReadHeader(GDS_PLEX))
         SkipData();

      if (!ReadHeader(GDS_SNAME))
         throw GDS_Error("Header: SNAME missing (in Structure Reference)");

      wxString strname;
      ReadString(strname);
      _sref->SetStructureReferenceName(strname);

      _strans = new Strans();
      ReadStrans(_strans);

      if (!ReadHeader(GDS_XY))
         throw GDS_Error("Header: XY missing (in Structure Reference)");

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


// Arrayreference -----------------------------------------------------------
bool GDS_driver_in::ReadArrayReference(Structure* _structure)
{

	// Grammar: AREF [ELFLAGS] [PLEX] SNAME [<strans>]
	//				COLROW XY

	if (!ReadHeader(GDS_AREF))
	  return false;

	Point* _Point;
   wxTransformMatrix*_relative_matrix;
   ArrayReference* _aref;
   Strans* _strans=0;
   Point* _HorPoint;
   Point* _VerPoint;
   try
   {
      // Create a new Aref
      _aref = new ArrayReference();

      ReadElflags(_aref);

      if (ReadHeader(GDS_PLEX))
         SkipData();

      if (!ReadHeader(GDS_SNAME))
         throw GDS_Error("Header: SNAME missing (in Structure Reference)");

      wxString strname;
      ReadString(strname);
      _aref->SetStructureReferenceName(strname);

      Strans* _strans = new Strans();
      ReadStrans(_strans);


		if (!ReadHeader(GDS_COLROW))
         throw GDS_Error("Header: COLROW missing (in Array Reference)");

      TWO_G_BYTE_SIGNED_INTEGER value;
		GDS_file >> &value;
		_aref->SetColumns(value);
		GDS_file >> &value;
		_aref->SetRows(value);

      if (!ReadHeader(GDS_XY))
         throw GDS_Error("Header: XY missing (in Array Reference)");

      _Point = new Point();
      Read(_Point);

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
         EIGHT_G_BYTE_REAL scale = _strans->GetScale();
         _relative_matrix->Scale(scale, scale, 0, 0);
      }

      // Translate2D over XY from the structurereference
      _relative_matrix->Translate(_Point->GetX(), _Point->GetY());

      _aref->SetRelativeMatrix(_relative_matrix);

      // Add this Element to the end of the list
      _structure->insend(_aref);

      delete _Point;
      delete _strans;
      delete _relative_matrix;
   }
	catch (GDS_Error& _error)
   {
      delete _aref;
      delete _Point;
      delete _HorPoint;
      delete _VerPoint;
      delete _strans;
      delete _relative_matrix;
      throw _error;
   }

	return true;
}


bool GDS_driver_in::ReadNode(Structure* /*_structure*/)
{
	// not implemented
	return false;
}


// Box ----------------------------------------------------------------------
bool GDS_driver_in::ReadBox(Structure* _structure)
{
	// Grammar: BOX [ELFLAGS] [PLEX] LAYER BOXTYPE XY
	if (!ReadHeader(GDS_BOX))
	  return false;

	// Create a new Boundary
	Box* _Box = new Box();

	ReadElflags(_Box);

	if (ReadHeader(GDS_PLEX))
		SkipData();

	if (!ReadLayer(_Box))
			throw GDS_Error("Box: LAYER missing (in GDS-file)");

	if (!ReadBoxtype(_Box))
		throw GDS_Error("Box: BOXTYPE missing (in GDS-file)");


   G_Polygon* orgi = new G_Polygon();
	if (!Read(orgi))
		throw GDS_Error("Box:  XY missing (in GDS-file)");

   _Box->Set(orgi);

	// Add this Element to the end of the list
  	_structure->insend(_Box);

   delete orgi;

	return true;
}

// Elflags ------------------------------------------------------------------
bool GDS_driver_in::ReadElflags(Element* _Element)
{
	if (!ReadHeader(GDS_ELFLAGS))
		return false;

	TWO_G_BYTE_SIGNED_INTEGER value;
	GDS_file >> &value;

#ifdef __UNIX__
	_Element->SetTemplate((bool) (value && 2));
	_Element->SetExternal((bool) (value && 1));
#else
	_Element->SetTemplate((bool) (value && 1));
	_Element->SetExternal((bool) (value && 2));
#endif

	return true;
}


// GdsII --------------------------------------------------------------------
void GDS_driver_in::Read(GdsII* _GdsII)
{
	// Grammar: HEADER BGNLIB LIBNAME [REFLIBS] [FONTS]
	//				[ATTRTABLE] [GENERATION] [<FormatType>]
	//				UNITS {<Structure>}* ENDLIB
	//


	if (!ReadGdsHeader(_GdsII))
		throw GDS_Error("GDS_HEADER record missing (in GDSII)", "Error");

	if (!ReadBgnlib(_GdsII))
		throw GDS_Error("BGNLIB record missing (in GDSII)");

	if (!ReadLibrarySettings(_GdsII))
		throw GDS_Error("LIBNAME missing (in GDSII)");

	while (ReadStructure(_GdsII))
		;

	if (!ReadHeader(GDS_ENDLIB))
		throw GDS_Error("GDS_ENDLIB record missing (in GDSII)", "Error");

	//references to other structure or known
	//by name now set the pointers in those references
	//to the right structures
	_GdsII->LinkReferences();

	//the topstructure is the structure that is not referenced
	_GdsII->SetTopStructure(_GdsII->SearchTopStructure());

	_GdsII->SetShowStructure(_GdsII->GetTopStructure());

	_GdsII->SetReferenceNr();
	_GdsII->CalcBoundingBox();
}


// Header -------------------------------------------------------------------
bool GDS_driver_in::ReadGdsHeader(GdsII* _GdsII)
{
	m_cur_gdsii=_GdsII;
	if (!ReadHeader(GDS_HEADER))
      return false;

	TWO_G_BYTE_SIGNED_INTEGER version;
	GDS_file >> &version;
	_GdsII->SetVersion(version);

   return true;
}


// Layer --------------------------------------------------------------------
bool GDS_driver_in::ReadLayer(Shape* _Shape)
{
	if (!ReadHeader(GDS_LAYER))
		return false;

	TWO_G_BYTE_SIGNED_INTEGER layer;
	GDS_file >> &layer;
	GLOB->SETUP->Set_Available(_mapping_in[layer], true);

	_Shape->SetLayer(_mapping_in[layer]);
	return(true);
}


// Library ------------------------------------------------------------------
bool GDS_driver_in::ReadLibraryName(Library* _Library)
{
	if (!ReadHeader(GDS_LIBNAME))
		return false;

	wxString libname;

	ReadString(libname);
	_Library->SetLibraryName(libname);

	return(true);
}


// Library ------------------------------------------------------------------
bool GDS_driver_in::ReadLibrarySettings(GdsII* _GdsII)
{
	Library* _Library = _GdsII->GetLibrary();

	if (!ReadLibraryName(_Library))
		throw GDS_Error("GDS_LIBNAME record missing (in GDSII)", "Error");

   ReadRefLibs(_Library);
   ReadFonts(_Library);
   ReadAttrtable(_Library);
	ReadGenerations(_Library);

	if (ReadHeader(GDS_FORMAT))
		SkipData();

	if (!ReadUnits(_Library))
		throw GDS_Error("UNITS missing (in GDSII)");

	return(true);
}

// Point --------------------------------------------------------------------
bool GDS_driver_in::Read(Point* _Point)
{
	FOUR_G_BYTE_SIGNED_INTEGER value;
	GDS_file >> &value;
	_Point->SetX(value * m_cur_gdsii->GetUserUnits());
	GDS_file >> &value;
	_Point->SetY(value * m_cur_gdsii->GetUserUnits());
	return true;
}


// Point --------------------------------------------------------------------
void GDS_driver_in::Read(Segment* _Seg)
{
	FOUR_G_BYTE_SIGNED_INTEGER value;
	GDS_file >> &value;
	_Seg->SetX(value * m_cur_gdsii->GetUserUnits());
	GDS_file >> &value;
	_Seg->SetY(value * m_cur_gdsii->GetUserUnits());
}


// Polygon ------------------------------------------------------------------
bool GDS_driver_in::Read(G_Polygon* _Polygon)
{
	if (!ReadHeader(GDS_XY))
		return false;

	int points = _recordsize / (2 * sizeof(FOUR_G_BYTE_SIGNED_INTEGER))-1;

	if (points < 2)
    {   
        if ( points == 1 )
        {
		    Line_Seg* seg = new Line_Seg();
		    Read( seg);
            wxString error;
            error.Printf("Only one point in Polygon: %d, %d ", seg->GetX(), seg->GetY() );
            _messagehandler->info( (char*) error.c_str(), "GDSII error" );

            _Polygon->AddSegment(seg);
		    Line_Seg* seg2 = new Line_Seg( seg->GetX(), seg->GetY()+1000 );
            _Polygon->AddSegment(seg2);
		    seg2 = new Line_Seg( seg->GetX()-1000, seg->GetY() );
            _Polygon->AddSegment(seg2);
        }
        else
    		throw GDS_Error("Too few points in Polygon.", "Fatal GDSII error", 0, 1);
    }
    else
    {
	    Line_Seg* seg;
	    for (int i = 0; i < points; i++)
	    {
		    seg = new Line_Seg();
		    Read( seg);
		    _Polygon->AddSegment(seg);
	    }

	    Line_Seg segend;
	    Read(&segend); // Last point matches first one read: skip
    }
	return true;
}


// Polyline -----------------------------------------------------------------
bool GDS_driver_in::Read(G_Polyline* _Polyline)
{
	if (!ReadHeader(GDS_XY))
		return false;

	int points = _recordsize / (2 * sizeof(FOUR_G_BYTE_SIGNED_INTEGER));
	if (points < 2)
    {   
        if ( points == 0 )
        {
		    Line_Seg* seg = new Line_Seg();
		    Read( seg);
            wxString error;
            error.Printf("Only one point in Polyline: %d, %d ", seg->GetX(), seg->GetY() );
            _messagehandler->info( (char*)error.c_str(), "GDSII error" );

            _Polyline->AddSegment(seg);
		    Line_Seg* seg2 = new Line_Seg( seg->GetX(), seg->GetY()+1000 );
            _Polyline->AddSegment(seg2);
		    seg2 = new Line_Seg( seg->GetX()-1000, seg->GetY() );
            _Polyline->AddSegment(seg2);
        }
        else
    		throw GDS_Error("Too few points in Polyline.", "Fatal GDSII error", 0, 1);
    }
    else
    {
	    Line_Seg* seg;
	    for (int i = 0; i < points; i++)
	    {
		    seg = new Line_Seg();
		    Read( seg);
		    _Polyline->AddSegment(seg);
	    }
    }
	return true;
}


// Presentation -------------------------------------------------------------
bool GDS_driver_in::ReadPresentation(Text* _Text)
{
	if (!ReadHeader(GDS_PRESENTATION))
      return false;

   TWO_G_BYTE_SIGNED_INTEGER value;
	GDS_file >> &value;
	_Text->SetPresentationFlags(value);
   return true;
}



// Strans -------------------------------------------------------------------
bool GDS_driver_in::ReadStrans(Strans* _Strans)
{
	if (!ReadHeader(GDS_STRANS))
      return(false);

	EIGHT_G_BYTE_REAL real;

   TWO_G_BYTE_SIGNED_INTEGER value;
	GDS_file >> &value;
	_Strans->_stransflags.total=value;

	if (ReadHeader(GDS_MAG))
	{
		GDS_file >> &real;
		_Strans->SetScale(real);
	}

	if (ReadHeader(GDS_ANGLE))
	{
		GDS_file >> &real;
		_Strans->SetAngle(real);
	}

	return true;
}


// Structure ----------------------------------------------------------------
bool GDS_driver_in::ReadStructure(GdsII*	_structurelist)
{
	// Grammar: BGNSTR STRNAME [STRCLASS] {<element>}*
	//				ENDSTR

	Structure* _Structure = new Structure;

	if (!ReadBgnstr(_Structure))
	{
		// No BGNSTR, probably end of Library
		delete _Structure;
		return false;
	}

	if (!ReadHeader(GDS_STRNAME))
		throw GDS_Error("STRNAME record missing (in Structure)");

	wxString strucname;
	ReadString(strucname);
	_Structure->SetStructureName(strucname);

	if(ReadHeader(GDS_STRCLASS))
		SkipData();

	while (ReadElement(_Structure));

	if(!ReadHeader(GDS_ENDSTR))
		throw GDS_Error("ENDSTR record missing (in GDSII)");

  	_structurelist->insend(_Structure);

	return true;
}

// Units --------------------------------------------------------------------
bool GDS_driver_in::ReadUnits(Library* _Library)
{
	if (!ReadHeader(GDS_UNITS))
		 return(false);


	EIGHT_G_BYTE_REAL value;
	GDS_file >> &value;
	_Library->SetUserUnits(value);
	EIGHT_G_BYTE_REAL value1;
	GDS_file	>> &value1;
	_Library->SetPhysicalUnits(value1/value);

	 return(true);
}


// Width --------------------------------------------------------------------
bool GDS_driver_in::ReadWidth(Shape* _Shape)
{
	if (!ReadHeader(GDS_WIDTH))
		return false;

	FOUR_G_BYTE_SIGNED_INTEGER value;
	GDS_file >> &value;
	_Shape->SetWidth(value* m_cur_gdsii->GetUserUnits());

	return true;
}


bool GDS_driver_in::ReadRefLibs(Library* /*_Library*/)
{
	if (!ReadHeader(GDS_REFLIBS))
		return false;
	SkipData();
	return true;
}


bool GDS_driver_in::ReadFonts(Library* /*_Library*/)
{
	if (!ReadHeader(GDS_FONTS))
		return false;
	SkipData();
	return true;
}


bool GDS_driver_in::ReadAttrtable(Library* /*_Library*/)
{
	if (!ReadHeader(GDS_ATTRTABLE))
		return false;
	SkipData();
	return true;
}


bool GDS_driver_in::ReadGenerations(Library* /*_Library*/)
{
	if (!ReadHeader(GDS_GENERATIONS))
		return false;
	SkipData();
	return true;
}


bool GDS_driver_in::ReadDatatype(Boundary* _Boundary)
{
	if (!ReadHeader(GDS_DATATYPE))
		return false;

	TWO_G_BYTE_SIGNED_INTEGER value;
	GDS_file >> &value;

	_Boundary->SetDatatype(value);

	return true;
}


bool GDS_driver_in::ReadDatatype(Path* _Path)
{
	if (!ReadHeader(GDS_DATATYPE))
		return false;

	TWO_G_BYTE_SIGNED_INTEGER value;
	GDS_file >> &value;

	_Path->SetDatatype(value);

	return true;
}


bool GDS_driver_in::ReadBoxtype(Box* _Box)
{
	if (!ReadHeader(GDS_BOXTYPE))
		return false;

	TWO_G_BYTE_SIGNED_INTEGER value;
	GDS_file >> &value;

	_Box->SetBoxtype(value);

	return true;
}


// TextBody -----------------------------------------------------------------
bool GDS_driver_in::ReadTextBody(Text* _text)
{
//	<textbody>			::=	TEXTTYPE [PRESENTATION] [PATHTYPE]
//									[WIDTH] [<strans>] XY STRING
	// Must be there
	if (!ReadHeader(GDS_TEXTTYPE))
		return false;

	TWO_G_BYTE_SIGNED_INTEGER value;
	GDS_file >> &value;
	_text->SetTexttype(value);

	// Optional
	ReadPresentation(_text);

	if (ReadHeader(GDS_PATHTYPE))
   {
      TWO_G_BYTE_SIGNED_INTEGER value;
      GDS_file >> &value;

      _text->SetPathtype((TEXT_PATHTYPE)value);
   }

	ReadWidth(_text);
	Strans* _Strans = new Strans();
	ReadStrans(_Strans);

	if (!ReadHeader(GDS_XY))
		throw GDS_Error("Text: XY is missing (in GDS-file)");
	Point* _point = new Point();
	Read(_point);

  	// Ok, now we have a strans, but we want to work with a matrix,
   // so let's convert it to one:

   wxTransformMatrix*_relative_matrix = new wxTransformMatrix();

   // Mirror in X if necessary
   if (_Strans->GetReflection())
      _relative_matrix->Mirror();

   //  Rotate if necessary
   if (_Strans->GetAbsAngle() == 0)
      _relative_matrix->Rotate(_Strans->GetAngle(), 0, 0);

   // Scale if necessary
   if (_Strans->GetAbsScale() == 0)
   {
      EIGHT_G_BYTE_REAL scale = _Strans->GetScale();
      _relative_matrix->Scale(scale, scale, 0, 0);
   }

   // Translate2D over XY from the structrereference
   _relative_matrix->Translate(_point->GetX(), _point->GetY());

	_text->SetRelativeMatrix(_relative_matrix);

	if (!ReadHeader(GDS_STRING))
		throw GDS_Error("Text: STRING is missing (in GDS-file)");
	wxString a_string;
	ReadString(a_string);
	_text->SetText(a_string);

	delete _Strans;
   delete _relative_matrix;
   delete _point;
	return true;
}



