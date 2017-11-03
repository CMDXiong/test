/*
	Program		GDSDUMP.CPP
	Purpose		Creates an output file from GDS-II in plain text
*/


#ifdef __GNUG__
#pragma implementation
#endif

#include "keydrout.h"
#include "trans.h"

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

// constructor with the filename
KEY_driver_out::KEY_driver_out(const wxString& _filename, bool only_visible)
{
   _only_visible = only_visible;
	points_written = 0;
   KEY_out_filename=_filename;
	KEY_out_file.open(KEY_out_filename, ios::out);
	WriteHeader();

	for (int i = 0; i < MAX_LAYERS; i++)
		   _mapping_out[i]=GLOB->SETUP->Get_GdsOutMapping(i);

}

KEY_driver_out::~KEY_driver_out()
{
	KEY_out_file.close();
}

void KEY_driver_out::WriteHeader()
{
	// show some respect: do not modify
	KEY_out_file << "# KEY file for GDS-II postprocessing tool" << endl;
	KEY_out_file << "# File = " << KEY_out_filename << endl;
	KEY_out_file << "# ====================================================================" << endl << endl;
}

// write operations for GDSII-dumpfiles

// Arrayreference -----------------------------------------------------------
void KEY_driver_out::Write(ArrayReference* _ArrayReference)
{
	KEY_out_file  << endl << "AREF; " << endl;

	WriteFlags(_ArrayReference);

	KEY_out_file << "SNAME " << _ArrayReference->GetStructureReferenceName().c_str() << ";  ";

   Strans* _strans = new Strans();
	_strans->MakeStrans(_ArrayReference);

	points_written = 0;

	if (_strans->GetStrans())
		Write(_strans);

	KEY_out_file << "COLROW {"
					 << _ArrayReference->GetColumns() << " , "
					 << _ArrayReference->GetRows() << "};  ";

	KEY_out_file << endl << "   XY 3; " << endl;

   Point offset;
   _ArrayReference->GetPoint(offset);
	Write(&offset);
	Write(_ArrayReference->GetHorzPoint());
	Write(_ArrayReference->GetVertPoint());

   Write(_ArrayReference->HasPropList());

	KEY_out_file << endl << "ENDEL; " << endl;
	delete _strans;
}



// Boundary -----------------------------------------------------------------
void KEY_driver_out::Write(Boundary* _Boundary)
{
	KEY_out_file << endl << "BOUNDARY; ";

	WriteFlags(_Boundary);

	KEY_out_file << "LAYER " << _mapping_out[_Boundary->GetLayer()] << ";  ";
	KEY_out_file << "DATATYPE " << _Boundary->GetDatatype() << ";  ";

	if (_Boundary->GetSpline())
		KEY_out_file << "SPLINE 1 ;";

	if( _Boundary->GetWidth() != DEFAULT_WIDTH )
		KEY_out_file << "WIDTH " << _Boundary->GetWidth() << "; ";

	Write(_Boundary->GetPolygon());

   Write(_Boundary->HasPropList());

	KEY_out_file << endl << "ENDEL; " << endl;
}

// Box -----------------------------------------------------------------
void KEY_driver_out::Write(Box* _Box)
{
	KEY_out_file << endl << "BOX; ";

	WriteFlags(_Box);

	KEY_out_file << "LAYER " << _mapping_out[_Box->GetLayer()] << ";  ";
	KEY_out_file << "BOXTYPE " << _Box->GetBoxtype() << ";  ";

	if( _Box->GetWidth() != DEFAULT_WIDTH )
		KEY_out_file << "WIDTH " << _Box->GetWidth() << "; ";

   G_Polygon* orgi = new G_Polygon();
	_Box->Convert(orgi,0);
	Write(orgi);
   delete orgi;

   Write(_Box->HasPropList());

	KEY_out_file << endl << "ENDEL; " << endl;
}

// SLine -----------------------------------------------------------------
void KEY_driver_out::Write(SLine* line)
{
	KEY_out_file << endl << "LINE; ";

	WriteFlags(line);

	KEY_out_file << "LAYER " << _mapping_out[line->GetLayer()] << ";  ";

	if( line->GetWidth() != DEFAULT_WIDTH )
		KEY_out_file << "WIDTH " << line->GetWidth() << "; ";

   G_Polygon* orgi = new G_Polygon();
	line->Convert(orgi,0);
	Write(orgi);
   delete orgi;

   Write(line->HasPropList());

	KEY_out_file << endl << "ENDEL; " << endl;
}

// Circle -----------------------------------------------------------------
void KEY_driver_out::Write(Circle* _Circle)
{
	KEY_out_file << endl << "CIRCLE; ";

	WriteFlags(_Circle);

	KEY_out_file << "LAYER " << _mapping_out[_Circle->GetLayer()] << ";  ";
	KEY_out_file << "DATATYPE " << _Circle->GetDatatype() << "; ";

	if (_Circle->GetWidth() != DEFAULT_WIDTH)
		KEY_out_file << "WIDTH " << _Circle->GetWidth() << "; ";

	// write the coordinate to the outputstream
	points_written = 0;

	KEY_out_file << endl << "   XY 1; " << endl;

	Write( &_Circle->GetMid());

	KEY_out_file << endl << "RADIUS " << _Circle->GetRadius() << ";  " << endl;

   Write(_Circle->HasPropList());

	KEY_out_file << endl << "ENDEL; " << endl;
}

// Elflags ------------------------------------------------------------------
void KEY_driver_out::WriteFlags(Element* _Element)
{
	if ((_Element->GetTemplate() != 0) ||
		(_Element->GetExternal() != 0))
	{
		int value = 0;

		if (_Element->GetTemplate())
			value += 2^1;

		if (_Element->GetExternal())
			value += 2^2;

		KEY_out_file << "ELFLAGS " << value << ";  ";
	}
}

// GdsII --------------------------------------------------------------------
void KEY_driver_out::Write(GdsII* _GdsII)
{
	ModeTime* _Bgnlib=_GdsII->GetBgnlib();
	KEY_out_file << "HEADER " << _GdsII->GetVersion() << "; # version " << endl;
	KEY_out_file << "BGNLIB; " << endl;
	KEY_out_file << "LASTMOD {"
					 << _Bgnlib->GetLmtYear() << "-"
					 << _Bgnlib->GetLmtMonth() << "-"
					 << _Bgnlib->GetLmtDay() << "  "
					 << _Bgnlib->GetLmtHour() << ":"
					 << _Bgnlib->GetLmtMinute() << ":"
					 << _Bgnlib->GetLmtSecond() << "}; # last modification time" << endl;
	KEY_out_file << "LASTACC {"
					 << _Bgnlib->GetLatYear() << "-"
					 << _Bgnlib->GetLatMonth() << "-"
					 << _Bgnlib->GetLatDay() << "  "
					 << _Bgnlib->GetLatHour() << ":"
					 << _Bgnlib->GetLatMinute() << ":"
					 << _Bgnlib->GetLatSecond() << "}; # last access time" << endl;
	Write(_GdsII->GetLibrary());

	TDLI<Structure> I(_GdsII);

	I.toroot();
	while (I.iterate())
   {
      if (_GdsII->GetMultiple())
      {
         if (I.item()->GetStructureName().Cmp("_top_")!=0)
       		Write( I.item());
      }
      else
       		Write( I.item());
   }

	KEY_out_file << "ENDLIB; " << endl;
}


// Library ------------------------------------------------------------------
void KEY_driver_out::Write(Library* _Library)
{
	KEY_out_file << "LIBNAME " << _Library->GetLibraryName().c_str() << "; " << endl;
	KEY_out_file << "UNITS; "  << endl
					 << "USERUNITS " << _Library->GetUserUnits()     << "; "
					 << "PHYSUNITS " << _Library->GetPhysicalUnits()*_Library->GetUserUnits() << "; " << endl;
}

// Path ---------------------------------------------------------------------
void KEY_driver_out::Write(Path* _Path)
{
	KEY_out_file  << endl << "PATH; ";

	WriteFlags(_Path);

	KEY_out_file << "LAYER " << _mapping_out[_Path->GetLayer()] << "; ";
	KEY_out_file << "DATATYPE " << _Path->GetDatatype() << "; ";

	if (_Path->GetPathtype())
		KEY_out_file << "PATHTYPE " << _Path->GetPathtype() << "; ";

	if (_Path->GetSpline())
		KEY_out_file << "SPLINE 1 ;";

	if( _Path->GetWidth() != DEFAULT_WIDTH )
		KEY_out_file << "WIDTH " << _Path->GetWidth() << "; ";

	Write( _Path->GetPolyline());

   Write(_Path->HasPropList());

	KEY_out_file << endl << "ENDEL; " << endl;
}

// Arcseg ----------------------------------------------------------------------
void KEY_driver_out::Write(Arcseg* _Arcseg)
{
	KEY_out_file  << endl << "ARC; ";

	WriteFlags(_Arcseg);

	KEY_out_file << "LAYER " << _mapping_out[_Arcseg->GetLayer()] << "; ";
	KEY_out_file << "DATATYPE " << _Arcseg->GetDatatype() << "; ";

	if( _Arcseg->GetWidth() != DEFAULT_WIDTH )
		KEY_out_file << "WIDTH " << _Arcseg->GetWidth() << "; ";

	Write( _Arcseg->GetPolyline());

   Write(_Arcseg->HasPropList());

	KEY_out_file << endl << "ENDEL; " << endl;
}

// Point --------------------------------------------------------------------
void KEY_driver_out::Write(Point *a_point)
{
	if (!(points_written%2))
		KEY_out_file << endl << "   ";

	points_written++;

	char buf[40];
	sprintf(buf, "%18.6lf", a_point->GetX());

	KEY_out_file <<  "X "<< buf << "; ";

	sprintf(buf, "%18.6lf", a_point->GetY());

	KEY_out_file <<  "Y "<< buf << "; ";
}

// Segment  -----------------------------------------------------------------
void KEY_driver_out::Write(Segment *_Segment)
{
	if (!(points_written%2))
		KEY_out_file << endl << "   ";

	points_written++;

   switch (_Segment->GetHoleType())
   {
		case NORMAL_SEG:break;
		case LINK_SEG: KEY_out_file << "ST L; "; break;
		case HOLE_SEG: KEY_out_file << "ST H; "; break;
   }

	char buf[40];
	sprintf(buf, "%18.6f", _Segment->GetX());
	KEY_out_file <<  "X "<< buf << "; ";
	sprintf(buf, "%18.6f", _Segment->GetY());
	KEY_out_file <<  "Y "<< buf << ";   ";

	if (_Segment->GetType()==ARC )
	{
		if (!(points_written%2))
			KEY_out_file << endl << "   ";
		points_written++;
		Arc_Seg* aseg=(Arc_Seg*) _Segment;
		sprintf(buf, "%18.6f", aseg->GetAX());
		KEY_out_file <<  "XM "<< buf << "; ";
		sprintf(buf, "%18.6f", aseg->GetAY());
		KEY_out_file <<  "YM "<< buf << ";   ";
		if (!(points_written%2))
			KEY_out_file << endl << "   ";
		points_written++;
		sprintf(buf, "%15.3f", aseg->GetOX());
		KEY_out_file <<  "XO "<< buf << "; ";
		sprintf(buf, "%15.3f", aseg->GetOY());
		KEY_out_file <<  "YO "<< buf << ";   ";
	}
}

// Polygon ------------------------------------------------------------------
void KEY_driver_out::Write(G_Polygon* _Polygon)
{
   TDLI<Segment> 	Iter=TDLI<Segment>(_Polygon); // first make iterator

	long points;

	points=Iter.count();

	KEY_out_file  << endl << "   XY " << (points+1) << "; ";

	points_written = 0;
	Iter.tohead();
	while (!Iter.hitroot())
	{
		Write(Iter.item());
		Iter++;
	}
	Iter.tohead();

	Point end(Iter.item()->GetX(),Iter.item()->GetY());

	// write the first point again to close the polygon
	Write(&end);
}

// Polyline -----------------------------------------------------------------
void KEY_driver_out::Write(G_Polyline* _Polyline)
{
   TDLI<Segment> 	Iter=TDLI<Segment>(_Polyline); // first make iterator

	long points;

	points=Iter.count();

	KEY_out_file  << endl << "   XY " << points << "; ";

	points_written = 0;
	Iter.tohead();
	while (!Iter.hitroot())
	{
		Write(Iter.item());
		Iter++;
	}
}

// Strans -------------------------------------------------------------------
void KEY_driver_out::Write(Strans* _Strans)
{
	KEY_out_file << "STRANS "
					 << _Strans->_stransflags.bits.reflection << ","
					 << _Strans->_stransflags.bits.abs_angle << ","
					 << _Strans->_stransflags.bits.abs_scale << "; ";

	// writes only the scale when different from default
	if (_Strans->GetScale() != DEFAULT_STRANS_SCALE)
		KEY_out_file << "MAG " << _Strans->GetScale() << "; ";

	// writes only the angle when different from default
	if (_Strans->GetAngle() != DEFAULT_STRANS_ANGLE)
		KEY_out_file << "ANGLE " << _Strans->GetAngle() << "; ";
}

// StructureReference -------------------------------------------------------
void KEY_driver_out::Write(StructureReference* _StructureReference)
{
	KEY_out_file  << endl << "SREF; " << endl;

	WriteFlags(_StructureReference);

	KEY_out_file << "SNAME " << _StructureReference->GetStructureReferenceName().c_str() << "; " << endl;

   Strans* _strans = new Strans();
	_strans->MakeStrans(_StructureReference);

	if (_strans->GetStrans())
			Write(_strans);

	KEY_out_file << endl << "   XY 1; " << endl;

	points_written = 0;

   Point offset;
   _StructureReference->GetPoint(offset);
	Write( &offset);

   Write(_StructureReference->HasPropList());
	KEY_out_file << endl << "ENDEL; " << endl;
	delete _strans;
}

// Structure ----------------------------------------------------------------
void KEY_driver_out::Write(Structure* _Structure)
{
	// BGNSTR
	ModeTime* _Bgnstr=_Structure->GetBgnstr();
	KEY_out_file << endl << "BGNSTR; # Begin of structure " << endl;
	KEY_out_file << "CREATION {"
					 << _Bgnstr->GetLmtYear() << "-"
					 << _Bgnstr->GetLmtMonth() << "-"
					 << _Bgnstr->GetLmtDay() << "  "
					 << _Bgnstr->GetLmtHour() << ":"
					 << _Bgnstr->GetLmtMinute() << ":"
					 << _Bgnstr->GetLmtSecond() << "}; # creation time" << endl;
	KEY_out_file << "LASTMOD  {"
					 << _Bgnstr->GetLatYear() << "-"
					 << _Bgnstr->GetLatMonth() << "-"
					 << _Bgnstr->GetLatDay() << "  "
					 << _Bgnstr->GetLatHour() << ":"
					 << _Bgnstr->GetLatMinute() << ":"
					 << _Bgnstr->GetLatSecond() << "}; # last modification time" << endl;

	KEY_out_file << "STRNAME " << _Structure->GetStructureName().c_str() << "; " << endl;

	OWrite((ElementList*)_Structure);

	KEY_out_file << endl << "ENDSTR " << _Structure->GetStructureName().c_str() << "; " << endl;
}

// Text ---------------------------------------------------------------------
void KEY_driver_out::Write(Text* _Text)
{
	KEY_out_file  << endl << "TEXT; ";

	WriteFlags(_Text);

	KEY_out_file << "LAYER " << _mapping_out[_Text->GetLayer()] << "; "<< endl;

	KEY_out_file << "TEXTTYPE " << _Text->GetTexttype() << "; ";

	if (_Text->GetPresentationFlags())
   {
		KEY_out_file << "PRESENTATION " <<
			(int)_Text->GetHorizontal() << "," <<
			(int)_Text->GetVertical() << "," <<
			(int)_Text->GetFont() << "; ";
  }
	if (_Text->GetPathtype())
		KEY_out_file << "PATHTYPE " << _Text->GetPathtype() << "; ";

	if( _Text->GetWidth() != DEFAULT_WIDTH )
		KEY_out_file << "WIDTH " << _Text->GetWidth() << "; ";

	Strans* _strans = new Strans();
	_strans->MakeStrans(_Text);


	if (_strans->GetStrans())
		Write(_strans);

	// write the coordinate to the outputstream
	points_written = 0;

	KEY_out_file << endl << "   XY 1; " << endl;

   Point offset;
	_Text->GetPoint(offset);
	Write(&offset);

	KEY_out_file << endl << "STRING {" << _Text->MyText().c_str() << "}; ";

   Write(_Text->HasPropList());

	KEY_out_file << endl << "ENDEL; " << endl;
	delete _strans;
}

void KEY_driver_out::Write(wxPropertySheet* propsheet)
{
   if (!propsheet)
      return;


	wxList& props=propsheet->GetProperties();

    for (wxNode *node = props.GetFirst(); node; node = node->GetNext() )

    {

      wxProperty*  current = (wxProperty*)node->GetData();

		wxPropertyValueType A_VALUE= current->GetValue().Type();
      switch (A_VALUE)
      {
         case wxPropertyValueInteger:
			      KEY_out_file << endl << "PROPERTY " << current->GetName() << "; "  << endl;
			      KEY_out_file << "PROPTYPE integer; "  << endl;
			      KEY_out_file << "PROPVALUE " << current->GetValue().GetStringRepresentation().c_str() << "; " << endl;
               break;

         case wxPropertyValueReal:
			      KEY_out_file << endl << "PROPERTY " << current->GetName().c_str() << "; "  << endl;
			      KEY_out_file << "PROPTYPE real; "  << endl;
			      KEY_out_file << "PROPVALUE " << current->GetValue().GetStringRepresentation() << "; " << endl;
               break;
         case wxPropertyValuebool:
			      KEY_out_file << endl << "PROPERTY " << current->GetName().c_str() << "; "  << endl;
			      KEY_out_file << "PROPTYPE bool; "  << endl;
			      KEY_out_file << "PROPVALUE " << current->GetValue().GetStringRepresentation() << "; " << endl;
               break;
         case wxPropertyValueString:
			      KEY_out_file << endl << "PROPERTY " << current->GetName().c_str() << "; "  << endl;
			      KEY_out_file << "PROPTYPE string; "  << endl;
			      KEY_out_file << "PROPVALUE " << current->GetValue().GetStringRepresentation().c_str() << "; " << endl;
               break;
			default: //properties containing pointers to real members of the element are not stored.
         		break;
      }
    }
}


// Image ---------------------------------------------------------------------
void KEY_driver_out::Write(Image* _image)
{
	KEY_out_file  << endl << "IMAGE; ";

	WriteFlags(_image);

	KEY_out_file << "LAYER " << _mapping_out[_image->GetLayer()] << "; "<< endl;

	if( _image->GetWidth() != DEFAULT_WIDTH )
		KEY_out_file << "WIDTH " << _image->GetWidth() << "; ";

	if( _image->GetW() != 0 )
		KEY_out_file << "W " << _image->GetW() << "; ";

	if( _image->GetH() != 0 )
		KEY_out_file << "H " << _image->GetH() << "; ";

	KEY_out_file << "TYPE " << _image->GetType() << "; ";

	KEY_out_file << "PATH " << _image->GetPath() << "; ";

	Strans* _strans = new Strans();
	_strans->MakeStrans(_image);


	if (_strans->GetStrans())
		Write(_strans);

	// write the coordinate to the outputstream
	points_written = 0;

	KEY_out_file << endl << "   XY 1; " << endl;

   Point offset=_image->GetPosition();
	Write(&offset);

   Write(_image->HasPropList());

	KEY_out_file << endl << "ENDEL; " << endl;
	delete _strans;
}

