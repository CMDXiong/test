/*
Program		GDSDROUT.CPP
Purpose		GdsII output driver inherited from class Driver_out
Last Update		20-03-1998
*/


#ifdef __GNUG__
#pragma implementation
#endif

#include "limits.h"
#include "gdsdrout.h"
#include "valuesvc.h"

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
#include <math.h>
#include "units.h"
#include "trans.h"

GDS_driver_out::GDS_driver_out(const wxString& filename, bool only_visible)
{
   GDS_filename=filename;
	_only_visible = only_visible;
	_bad=false;
	GDS_file.open(GDS_filename, ios::binary | ios::out);
	if (GDS_file.bad())
		_bad=true;

	for (int i = 0; i < MAX_LAYERS; i++)
		   _mapping_out[i]=GLOB->SETUP->Get_GdsOutMapping(i);

	_userunits_out=1;
	_physunits_out=1;
   _scale_out=1;
}

GDS_driver_out::~GDS_driver_out()
{
	GDS_file.close();
}

bool GDS_driver_out::bad()
{ return(_bad);
}

void GDS_driver_out::WriteHeader(TWO_G_BYTE_UNSIGNED_INTEGER  nw_recordsize,
									  G_BYTE nw_datatype,
									  G_BYTE nw_recordtype)
{
	// a string must be odd length
	if (nw_datatype == GDSTYPE_ASCII_STRING &&
		 (nw_recordsize % 2) != 0)
			nw_recordsize++;

	GDS_file << (TWO_G_BYTE_UNSIGNED_INTEGER) (nw_recordsize + GDS_HEADERSIZE);
	GDS_file << nw_recordtype;
	GDS_file << nw_datatype;

	_recordsize = (TWO_G_BYTE_UNSIGNED_INTEGER) nw_recordsize;
}

void GDS_driver_out::WriteString(const wxString& a_string)
{
	int value = a_string.Len();
	for (int i = 0 ; i < value; i++)
 		GDS_file.put(a_string[i]);
	if (value % 2 != 0)
   {
 		GDS_file.put( 0 );
   }
}


// write operations for GDSII-files

// GdsII --------------------------------------------------------------------
void GDS_driver_out::Write(GdsII* _GdsII)
{

//first check if the dat is not to big to fit in four byte integers
//this is the maximum for vertexes in GDSII data

   BoundingBox drawing=_GdsII->GetBoundingBox();

   _userunits_out=_GdsII->GetUserUnits();
  	drawing.SetMin(drawing.GetMin().GetX()/_userunits_out,drawing.GetMin().GetY()/_userunits_out);
   drawing.SetMax(drawing.GetMax().GetX()/_userunits_out,drawing.GetMax().GetY()/_userunits_out);
   BoundingBox maxint(LONG_MIN,LONG_MIN, LONG_MAX,LONG_MAX);

   int i=0;
	while (maxint.Intersect(drawing,0) != _IN)
   {
   	drawing.SetMin(drawing.GetMin().GetX()/10.0,drawing.GetMin().GetY()/10.0);
      drawing.SetMax(drawing.GetMax().GetX()/10.0,drawing.GetMax().GetY()/10.0);
      i++;
   }

   _scale_out=pow(10.0,i);
   _physunits_out=_GdsII->GetPhysicalUnits()*_userunits_out*_scale_out;

	WriteHeader(2, GDSTYPE_TWO_G_BYTE_SIGNED_INTEGER, GDS_HEADER);

	GDS_file << (TWO_G_BYTE_SIGNED_INTEGER)_GdsII->GetVersion();

	WriteHeader(12 * sizeof(TWO_G_BYTE_SIGNED_INTEGER),
					GDSTYPE_TWO_G_BYTE_SIGNED_INTEGER,
					GDS_BGNLIB);

	GDS_file	<< (TWO_G_BYTE_SIGNED_INTEGER)_GdsII->GetBgnlib()->GetLmtYear()
				<< (TWO_G_BYTE_SIGNED_INTEGER)_GdsII->GetBgnlib()->GetLmtMonth()
				<< (TWO_G_BYTE_SIGNED_INTEGER)_GdsII->GetBgnlib()->GetLmtDay()
				<< (TWO_G_BYTE_SIGNED_INTEGER)_GdsII->GetBgnlib()->GetLmtHour()
				<< (TWO_G_BYTE_SIGNED_INTEGER)_GdsII->GetBgnlib()->GetLmtMinute()
				<< (TWO_G_BYTE_SIGNED_INTEGER)_GdsII->GetBgnlib()->GetLmtSecond()
				<< (TWO_G_BYTE_SIGNED_INTEGER)_GdsII->GetBgnlib()->GetLatYear()
				<< (TWO_G_BYTE_SIGNED_INTEGER)_GdsII->GetBgnlib()->GetLatMonth()
				<< (TWO_G_BYTE_SIGNED_INTEGER)_GdsII->GetBgnlib()->GetLatDay()
				<< (TWO_G_BYTE_SIGNED_INTEGER)_GdsII->GetBgnlib()->GetLatHour()
				<< (TWO_G_BYTE_SIGNED_INTEGER)_GdsII->GetBgnlib()->GetLatMinute()
				<< (TWO_G_BYTE_SIGNED_INTEGER)_GdsII->GetBgnlib()->GetLatSecond();

	Write(_GdsII->GetLibrary());

  	TDLI<Structure> I(_GdsII);

	I.toroot();
	while (I.iterate())
   {
      if (_GdsII->GetMultiple())
      {
         if (I.item()->GetStructureName().Cmp("_top_")!=0)
       		Write(I.item());
      }
      else
       		Write(I.item());
   }
	WriteHeader(0, 0, GDS_ENDLIB);
}

// Arrayreference -----------------------------------------------------------
void GDS_driver_out::Write(ArrayReference* _ArrayReference)
{
	WriteHeader(0, 0, GDS_AREF);

	WriteFlags(_ArrayReference);

	TWO_G_BYTE_UNSIGNED_INTEGER value = (TWO_G_BYTE_UNSIGNED_INTEGER) strlen(_ArrayReference->GetStructureReferenceName());
	if (value % 2 != 0)
		value++;
	WriteHeader(value, GDSTYPE_ASCII_STRING, GDS_SNAME);

	WriteString(_ArrayReference->GetStructureReferenceName());

   Strans* _strans = new Strans();
	_strans->MakeStrans(_ArrayReference);

	if (_strans->GetStrans())
		Write(_strans);


	WriteHeader(2 * sizeof(TWO_G_BYTE_SIGNED_INTEGER),
					GDSTYPE_TWO_G_BYTE_SIGNED_INTEGER,
					GDS_COLROW);

	GDS_file << (TWO_G_BYTE_SIGNED_INTEGER) _ArrayReference->GetColumns();
	GDS_file << (TWO_G_BYTE_SIGNED_INTEGER) _ArrayReference->GetRows();

	WriteHeader(6 * sizeof(FOUR_G_BYTE_SIGNED_INTEGER),
					GDSTYPE_FOUR_G_BYTE_SIGNED_INTEGER,
					GDS_XY);


   Point offset;
   _ArrayReference->GetPoint(offset);
	Write(&offset);
	Write(_ArrayReference->GetHorzPoint());
	Write(_ArrayReference->GetVertPoint());

	WriteHeader(0, 0, GDS_ENDEL);
	delete _strans;
}

// Boundary------------------------------------------------------------------
void GDS_driver_out::Write(Boundary* _Boundary)
{
	WriteHeader(0, 0, GDS_BOUNDARY);

	WriteFlags(_Boundary);

	WriteHeader(sizeof(TWO_G_BYTE_SIGNED_INTEGER),
					GDSTYPE_TWO_G_BYTE_SIGNED_INTEGER,
					GDS_LAYER);

	GDS_file << (TWO_G_BYTE_SIGNED_INTEGER)_mapping_out[_Boundary->GetLayer()];

	WriteHeader(sizeof(TWO_G_BYTE_SIGNED_INTEGER),
					GDSTYPE_TWO_G_BYTE_SIGNED_INTEGER,
					GDS_DATATYPE);

	GDS_file << (TWO_G_BYTE_SIGNED_INTEGER)_Boundary->GetDatatype();

	Write(_Boundary->GetPolygon());

	WriteHeader(0, 0, GDS_ENDEL);
}

// Text ---------------------------------------------------------------------
void GDS_driver_out::Write(Text* _Text)
{
	WriteHeader(0, 0, GDS_TEXT);

	WriteFlags(_Text);

	WriteHeader(sizeof(TWO_G_BYTE_SIGNED_INTEGER),
					GDSTYPE_TWO_G_BYTE_SIGNED_INTEGER,
					GDS_LAYER);

	GDS_file << (TWO_G_BYTE_SIGNED_INTEGER)_mapping_out[_Text->GetLayer()];

   // Texttype
   WriteHeader(sizeof(TWO_G_BYTE_SIGNED_INTEGER),
               GDSTYPE_TWO_G_BYTE_SIGNED_INTEGER,
               GDS_TEXTTYPE);

   GDS_file << (TWO_G_BYTE_SIGNED_INTEGER)_Text->GetTexttype();

	if (_Text->GetPresentationFlags())
   {
      // write only when a_presentation isn't default
      if (( (int)_Text->GetFont() != DEFAULT_PRESENTATION_FONT) ||
         ((int)_Text->GetVertical() != DEFAULT_PRESENTATION_VERTICAL) ||
         ((int)_Text->GetHorizontal() != DEFAULT_PRESENTATION_HORIZONTAL))
      {
         TWO_G_BYTE_SIGNED_INTEGER value = _Text->GetPresentationFlags();

         WriteHeader(sizeof(TWO_G_BYTE_SIGNED_INTEGER),
                     GDSTYPE_BIT_ARRAY,
                     GDS_PRESENTATION);
         GDS_file << value;
      }
   }

	if (_Text->GetPathtype())
   {
      WriteHeader(sizeof(TWO_G_BYTE_SIGNED_INTEGER),
                  GDSTYPE_TWO_G_BYTE_SIGNED_INTEGER,
                  GDS_PATHTYPE);

      GDS_file << (TWO_G_BYTE_SIGNED_INTEGER)_Text->GetPathtype();
	}

	if (_Text->GetWidth() != DEFAULT_WIDTH)
	{
		WriteHeader(sizeof(FOUR_G_BYTE_SIGNED_INTEGER),
						GDSTYPE_FOUR_G_BYTE_SIGNED_INTEGER,
						GDS_WIDTH);
		GDS_file << (FOUR_G_BYTE_SIGNED_INTEGER) (_Text->GetWidth() /  _userunits_out/_scale_out);
	}

	Strans* _strans = new Strans();
	_strans->MakeStrans(_Text);


	if (_strans->GetStrans())
		Write(_strans);

	// write the coordinate to the outputstream
	WriteHeader(2 * sizeof(FOUR_G_BYTE_SIGNED_INTEGER),
					GDSTYPE_FOUR_G_BYTE_SIGNED_INTEGER,
					GDS_XY);

   Point offset;
	_Text->GetPoint(offset);
	Write(&offset);

	// writes the text
	TWO_G_BYTE_UNSIGNED_INTEGER value = (TWO_G_BYTE_UNSIGNED_INTEGER)strlen(_Text->MyText());
	if (value % 2 != 0)
		value++;
	WriteHeader(value, GDSTYPE_ASCII_STRING, GDS_STRING);

	WriteString(_Text->MyText());

	WriteHeader(0, 0, GDS_ENDEL);
	delete _strans;
}

// Elflags ------------------------------------------------------------------
void GDS_driver_out::WriteFlags(Element* _Element)
{
	if ((_Element->GetTemplate() != 0) ||
		(_Element->GetExternal() != 0))
	{
		TWO_G_BYTE_SIGNED_INTEGER value = 0;

		if (_Element->GetTemplate())
			value += 1<<1;

		if (_Element->GetExternal())
			value += 1<<2;

		WriteHeader(sizeof(TWO_G_BYTE_SIGNED_INTEGER),
						GDSTYPE_BIT_ARRAY,
						GDS_ELFLAGS);

		GDS_file << value;
	}
}

// Library ------------------------------------------------------------------
void GDS_driver_out::Write(Library* _Library)
{
	TWO_G_BYTE_UNSIGNED_INTEGER value = (TWO_G_BYTE_UNSIGNED_INTEGER)strlen(_Library->GetLibraryName());
	if (value % 2 != 0)
		value++;
	WriteHeader(value, GDSTYPE_ASCII_STRING, GDS_LIBNAME);

	WriteString(_Library->GetLibraryName());
	WriteHeader(2 * sizeof(EIGHT_G_BYTE_REAL),
					GDSTYPE_EIGHT_G_BYTE_REAL,
					GDS_UNITS);

	GDS_file << (EIGHT_G_BYTE_REAL)_userunits_out
				<< (EIGHT_G_BYTE_REAL)_physunits_out;
}

// Path ---------------------------------------------------------------------
void GDS_driver_out::Write(Path* _Path)
{
	WriteHeader(0, 0, GDS_PATH);

	WriteFlags(_Path);

	WriteHeader(sizeof(TWO_G_BYTE_SIGNED_INTEGER),
					GDSTYPE_TWO_G_BYTE_SIGNED_INTEGER,
					GDS_LAYER);

	GDS_file << (TWO_G_BYTE_SIGNED_INTEGER)_mapping_out[_Path->GetLayer()];

	WriteHeader(sizeof(TWO_G_BYTE_SIGNED_INTEGER),
					GDSTYPE_TWO_G_BYTE_SIGNED_INTEGER,
					GDS_DATATYPE);

	GDS_file << (TWO_G_BYTE_SIGNED_INTEGER)_Path->GetDatatype();

	if (_Path->GetPathtype())
   {
      WriteHeader(sizeof(TWO_G_BYTE_SIGNED_INTEGER),
                  GDSTYPE_TWO_G_BYTE_SIGNED_INTEGER,
                  GDS_PATHTYPE);

      GDS_file << (TWO_G_BYTE_SIGNED_INTEGER)_Path->GetPathtype();
	}

	if (_Path->GetWidth() != DEFAULT_WIDTH)
	{
		WriteHeader(sizeof(FOUR_G_BYTE_SIGNED_INTEGER),
						GDSTYPE_FOUR_G_BYTE_SIGNED_INTEGER,
						GDS_WIDTH);
		GDS_file << (FOUR_G_BYTE_SIGNED_INTEGER) ((_Path->GetWidth() /  _userunits_out/_scale_out) + 0.5 );
	}

   if (_Path->GetSpline())
   {
      G_Polyline*	copy1 = new G_Polyline;
      double width=_Path->GetWidth();
      _Path->SetWidth(0); //in order to convert to polyline instead of polygon
      _Path->Convert((G_Polygon*)copy1,GLOB->DISPLAYABER());
      Write(copy1);
      delete copy1;
      _Path->SetWidth(width);
   }
   else
   	Write(_Path->GetPolyline());

	WriteHeader(0, 0, GDS_ENDEL);
}

// Arc ----------------------------------------------------------------------
void GDS_driver_out::Write(Arcseg* _Arcseg)
{
	WriteHeader(0, 0, GDS_BOUNDARY);

	WriteHeader(sizeof(TWO_G_BYTE_SIGNED_INTEGER),
					GDSTYPE_TWO_G_BYTE_SIGNED_INTEGER,
					GDS_LAYER);

	GDS_file << (TWO_G_BYTE_SIGNED_INTEGER)_mapping_out[_Arcseg->GetLayer()];

	WriteHeader(sizeof(TWO_G_BYTE_SIGNED_INTEGER),
					GDSTYPE_TWO_G_BYTE_SIGNED_INTEGER,
					GDS_DATATYPE);

	GDS_file << (TWO_G_BYTE_SIGNED_INTEGER)_Arcseg->GetDatatype();

   G_Polygon* orgi = new G_Polygon();
	// Convert circle to polygon
	_Arcseg->Convert(orgi, GLOB->DISPLAYABER());
	orgi->ConvertArcs(GLOB->DISPLAYABER());

	// write as polygon
	Write(orgi);
   delete orgi;

	WriteHeader(0, 0, GDS_ENDEL);
}

// Box ---------------------------------------------------------------------
void GDS_driver_out::Write(Box* _Box)
{
	WriteHeader(0, 0, GDS_BOX);

	WriteFlags(_Box);

	WriteHeader(sizeof(TWO_G_BYTE_SIGNED_INTEGER),
					GDSTYPE_TWO_G_BYTE_SIGNED_INTEGER,
					GDS_LAYER);

	GDS_file << (TWO_G_BYTE_SIGNED_INTEGER)_mapping_out[_Box->GetLayer()];

	WriteHeader(sizeof(TWO_G_BYTE_SIGNED_INTEGER),
					GDSTYPE_TWO_G_BYTE_SIGNED_INTEGER,
					GDS_BOXTYPE);

	GDS_file << (TWO_G_BYTE_SIGNED_INTEGER)_Box->GetBoxtype();

   G_Polygon* orgi = new G_Polygon();
	_Box->Convert(orgi,0);
	Write(orgi);
   delete orgi;

	WriteHeader(0, 0, GDS_ENDEL);
}

// Circle ---------------------------------------------------------------------
void GDS_driver_out::Write(Circle* _Circle)
{
	WriteHeader(0, 0, GDS_BOUNDARY);

	WriteHeader(sizeof(TWO_G_BYTE_SIGNED_INTEGER),
					GDSTYPE_TWO_G_BYTE_SIGNED_INTEGER,
					GDS_LAYER);

	GDS_file << (TWO_G_BYTE_SIGNED_INTEGER)_mapping_out[_Circle->GetLayer()];

	WriteHeader(sizeof(TWO_G_BYTE_SIGNED_INTEGER),
					GDSTYPE_TWO_G_BYTE_SIGNED_INTEGER,
					GDS_DATATYPE);

	GDS_file << (TWO_G_BYTE_SIGNED_INTEGER)_Circle->GetDatatype();

   G_Polygon* orgi = new G_Polygon();
	// Convert circle to polygon
	_Circle->Convert(orgi, GLOB->DISPLAYABER());
	orgi->ConvertArcs(GLOB->DISPLAYABER());

	// write as polygon
	Write(orgi);
   delete orgi;

	WriteHeader(0, 0, GDS_ENDEL);
}


// Point --------------------------------------------------------------------
void GDS_driver_out::Write(Point *_Point)
{
   FOUR_G_BYTE_SIGNED_INTEGER x;
   FOUR_G_BYTE_SIGNED_INTEGER y;
   if ( _Point->GetX() > 0 )
   	x = (FOUR_G_BYTE_SIGNED_INTEGER) ((_Point->GetX() /  _userunits_out/_scale_out) + 0.5);
   else
   	x = (FOUR_G_BYTE_SIGNED_INTEGER) ((_Point->GetX() /  _userunits_out/_scale_out) - 0.5);
   if ( _Point->GetY() > 0 )
   	y = (FOUR_G_BYTE_SIGNED_INTEGER) ((_Point->GetY() /  _userunits_out/_scale_out) + 0.5);
   else
   	y = (FOUR_G_BYTE_SIGNED_INTEGER) ((_Point->GetY() /  _userunits_out/_scale_out) - 0.5);

	GDS_file << x << y;
}

// Segment  -----------------------------------------------------------------
void GDS_driver_out::Write(Segment *_Segment)
{

   FOUR_G_BYTE_SIGNED_INTEGER x;
   FOUR_G_BYTE_SIGNED_INTEGER y;
   if ( _Segment->GetX() > 0 )
   	x = (FOUR_G_BYTE_SIGNED_INTEGER) ((_Segment->GetX() /  _userunits_out/_scale_out) + 0.5);
   else
   	x = (FOUR_G_BYTE_SIGNED_INTEGER) ((_Segment->GetX() /  _userunits_out/_scale_out) - 0.5);
   if ( _Segment->GetY() > 0 )
   	y = (FOUR_G_BYTE_SIGNED_INTEGER) ((_Segment->GetY() /  _userunits_out/_scale_out) + 0.5);
   else
   	y = (FOUR_G_BYTE_SIGNED_INTEGER) ((_Segment->GetY() /  _userunits_out/_scale_out) - 0.5);

	GDS_file << x << y;
}

// Polygon ------------------------------------------------------------------
void GDS_driver_out::Write(G_Polygon* _Polygon)
{
   TDLI<Segment> 	Iter=TDLI<Segment>(_Polygon); // first make iterator

	long help,points;

	points=Iter.count();

	//write one extra point
	help=2 * sizeof(FOUR_G_BYTE_SIGNED_INTEGER) * (points + 1);

	if (help > 65530L)
	{
		_messagehandler->info("Boundary record length to big (skipping last points)","gdsout");
		help=65530L;
		points=help/(2 * sizeof(FOUR_G_BYTE_SIGNED_INTEGER))-1;
	}

	WriteHeader((TWO_G_BYTE_UNSIGNED_INTEGER)help,GDSTYPE_FOUR_G_BYTE_SIGNED_INTEGER,GDS_XY);

	Iter.tohead();
	while (!Iter.hitroot())
	{
		Write( Iter.item());
		Iter++;
	}
	Iter.tohead();
	// write the first point again to close the polygon
	Write(Iter.item());
}

// Polyline -----------------------------------------------------------------
void GDS_driver_out::Write(G_Polyline* _Polyline)
{
   TDLI<Segment> 	Iter=TDLI<Segment>(_Polyline); // first make iterator
	long help,points;

	points=Iter.count();

	help=2 * sizeof(FOUR_G_BYTE_SIGNED_INTEGER) * points;

	if (help > 65530L)
	{
		_messagehandler->info("Polyline record length to big (skipping last points)","gdsout");
		help=65530L;
		points=help/(2 * sizeof(FOUR_G_BYTE_SIGNED_INTEGER));
	}
	WriteHeader((TWO_G_BYTE_UNSIGNED_INTEGER)help,GDSTYPE_FOUR_G_BYTE_SIGNED_INTEGER,GDS_XY);

	Iter.tohead();
	while (!Iter.hitroot())
	{
		Write(Iter.item());
		Iter++;
	}
}

// Strans -------------------------------------------------------------------
void GDS_driver_out::Write(Strans* _Strans)
{
	// writes only when strans isn't default
	if ((_Strans->_stransflags.bits.abs_angle != DEFAULT_STRANS_ABS_ANGLE) ||
		(_Strans->_stransflags.bits.abs_scale != DEFAULT_STRANS_ABS_SCALE) ||
		(_Strans->_stransflags.bits.reflection != DEFAULT_STRANS_REFLECTION) ||
		(_Strans->GetScale() != DEFAULT_STRANS_SCALE) ||
		(_Strans->GetAngle() != DEFAULT_STRANS_ANGLE))
	{
		TWO_G_BYTE_SIGNED_INTEGER value = _Strans->_stransflags.total;

		WriteHeader(sizeof(TWO_G_BYTE_SIGNED_INTEGER),
						GDSTYPE_BIT_ARRAY,
						GDS_STRANS);
		GDS_file << value;


		// writes only the scale when different from default
		if (_Strans->GetScale() != DEFAULT_STRANS_SCALE)
		{
			WriteHeader(sizeof(EIGHT_G_BYTE_REAL),
							GDSTYPE_EIGHT_G_BYTE_REAL,
							GDS_MAG);
			GDS_file << (EIGHT_G_BYTE_REAL)_Strans->GetScale();

		}

		// writes only the angle when different from default
		if (_Strans->GetAngle() != DEFAULT_STRANS_ANGLE)
		{
			WriteHeader(sizeof(EIGHT_G_BYTE_REAL),
							GDSTYPE_EIGHT_G_BYTE_REAL,
							GDS_ANGLE);
			GDS_file << (EIGHT_G_BYTE_REAL)_Strans->GetAngle();
		}
	}
}

// StructureReference -------------------------------------------------------
void GDS_driver_out::Write(StructureReference* _StructureReference)
{
	WriteHeader(0, 0, GDS_SREF);

	WriteFlags(_StructureReference);

	TWO_G_BYTE_UNSIGNED_INTEGER value = (TWO_G_BYTE_UNSIGNED_INTEGER)strlen(_StructureReference->GetStructureReferenceName());
	if (value % 2 != 0)
		value++;
	WriteHeader(value, GDSTYPE_ASCII_STRING, GDS_SNAME);

	WriteString(_StructureReference->GetStructureReferenceName());

	Strans* _strans = new Strans();

	_strans->MakeStrans(_StructureReference);



	if (_strans->GetStrans())
		Write(_strans);

	WriteHeader(2 * sizeof(FOUR_G_BYTE_SIGNED_INTEGER),
					GDSTYPE_FOUR_G_BYTE_SIGNED_INTEGER,
					GDS_XY);

   Point offset;
   _StructureReference->GetPoint(offset);
	Write(&offset);

	WriteHeader(0, 0, GDS_ENDEL);
	delete _strans;
}

// Structure ----------------------------------------------------------------
//	<Structure>			::=	BGNSTR STRNAME [STRCLASS] {<element>}* ENDSTR
void GDS_driver_out::Write(Structure* _Structure)
{
	// BGNSTR
	WriteHeader(12 * sizeof(TWO_G_BYTE_SIGNED_INTEGER),
					GDSTYPE_TWO_G_BYTE_SIGNED_INTEGER,
					GDS_BGNSTR);

	GDS_file	<< (TWO_G_BYTE_SIGNED_INTEGER)_Structure->GetBgnstr()->GetLmtYear()
				<< (TWO_G_BYTE_SIGNED_INTEGER)_Structure->GetBgnstr()->GetLmtMonth()
				<< (TWO_G_BYTE_SIGNED_INTEGER)_Structure->GetBgnstr()->GetLmtDay()
				<< (TWO_G_BYTE_SIGNED_INTEGER)_Structure->GetBgnstr()->GetLmtHour()
				<< (TWO_G_BYTE_SIGNED_INTEGER)_Structure->GetBgnstr()->GetLmtMinute()
				<< (TWO_G_BYTE_SIGNED_INTEGER)_Structure->GetBgnstr()->GetLmtSecond()
				<< (TWO_G_BYTE_SIGNED_INTEGER)_Structure->GetBgnstr()->GetLatYear()
				<< (TWO_G_BYTE_SIGNED_INTEGER)_Structure->GetBgnstr()->GetLatMonth()
				<< (TWO_G_BYTE_SIGNED_INTEGER)_Structure->GetBgnstr()->GetLatDay()
				<< (TWO_G_BYTE_SIGNED_INTEGER)_Structure->GetBgnstr()->GetLatHour()
				<< (TWO_G_BYTE_SIGNED_INTEGER)_Structure->GetBgnstr()->GetLatMinute()
				<< (TWO_G_BYTE_SIGNED_INTEGER)_Structure->GetBgnstr()->GetLatSecond();

	// STRNAME
	TWO_G_BYTE_UNSIGNED_INTEGER value = (TWO_G_BYTE_UNSIGNED_INTEGER) strlen(_Structure->GetStructureName());
	WriteHeader(value, GDSTYPE_ASCII_STRING, GDS_STRNAME);
	WriteString(_Structure->GetStructureName());

	// [STRCLASS]
	// Nothing (yet)

	// {<element>}*
	OWrite((ElementList*)_Structure);

	//	ENDSTR
	WriteHeader(0, 0, GDS_ENDSTR);
}




