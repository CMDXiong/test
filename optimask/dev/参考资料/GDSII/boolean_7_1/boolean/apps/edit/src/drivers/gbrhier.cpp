/*
Program	CNCHIER.CPP
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "units.h"
#include "arc.h"
#include "polyline.h"
#include "boundary.h"
#include "referenc.h"
#include "elemlst.h"
#include "gdsii.h"
#include "path.h"
#include "text.h"
#include "structur.h"

#include "gbrhier.h"
#include <iomanip>
#include <fstream>

// contructor  of screendriver
// makes a matrix and set boundingboxes
GBRDriver::GBRDriver(const wxString& _filename, bool only_visible)
{
	// holds temporary matrix from a structurereference
	// this will be modified in the references to calculate absolute positions
	_matrix = new wxTransformMatrix();
	if (_matrix == 0)
		throw GDS_Error("Cannot allocate memory for a matrix",
			"Screendriver Error", 0, 1);

	m_active=GBR_NORMAL;
	_only_visible = true;
	_visible = true;
	_pulsed  =  false;

	_AbsPolygon = new G_Polygon();

   _AI			= new TDLI<Segment>();

   _only_visible = only_visible;
	points_written = 0;
   GBR_out_filename=_filename;
	GBR_out_file.open(GBR_out_filename, ios::out);
}

// destructor: frees dynamic allocated memory
GBRDriver::~GBRDriver()
{
	delete _matrix;

   delete _AI;

	if (_AbsPolygon) delete _AbsPolygon;

	GBR_out_file.close();
}

void GBRDriver::WriteNumber(double big)
{

   if (big<0)
   {
		GBR_out_file << "-";
	   GBR_out_file.width(7);
   	GBR_out_file.fill('0');
      #ifdef __UNIX__
            GBR_out_file << (long)(-big*GLOB->PHYSUNITS()*1e6/2.54 + 0.5);  //mil
      #else
            GBR_out_file << setprecision(4) << (long)(-big*GLOB->PHYSUNITS()*1e6/2.54 + 0.5); //mil
      #endif
   } else
   {
	   GBR_out_file.width(7);
   	GBR_out_file.fill('0');
      #ifdef __UNIX__
            GBR_out_file << (long)(big*GLOB->PHYSUNITS()*1e6/2.54 + 0.5);  //mil
      #else
            GBR_out_file << setprecision(4) << (long)(big*GLOB->PHYSUNITS()*1e6/2.54 + 0.5);  //mil
      #endif
   }
}


// Point --------------------------------------------------------------------
void GBRDriver::Write(Point *a_point)
{
	points_written++;

   if (_first)
   {
      GBR_out_file <<  "G01X";              //goto first point
      WriteNumber(a_point->GetX());
      GBR_out_file << "Y";
      WriteNumber(a_point->GetY());
      GBR_out_file << "D02*" << endl;
   }
   else
   {
      GBR_out_file <<  "X";
      WriteNumber(a_point->GetX());
      GBR_out_file << "Y";
      WriteNumber(a_point->GetY());
      if (_second)
      {
	      GBR_out_file << "D01*"<< endl;
      	_second=false;
      }
      else
	      GBR_out_file << "*" << endl;
   }
}

// Segment  -----------------------------------------------------------------
void GBRDriver::Write(Segment *_Segment)
{
	points_written++;

   if (_first)
   {
      GBR_out_file <<  "G01X";              //goto first point
      WriteNumber(_Segment->GetX());
      GBR_out_file << "Y";
      WriteNumber(_Segment->GetY());
      GBR_out_file << "D02*" << endl;
      _second=true;
   }
   else
   {
      GBR_out_file <<  "X";
      WriteNumber(_Segment->GetX());
      GBR_out_file << "Y";
      WriteNumber(_Segment->GetY());
      if (_second)
      {
	      GBR_out_file << "D01*"<< endl;
      	_second=false;
      }
      else
	      GBR_out_file << "*" << endl;
   }

   if (_Segment->GetType()==ARC )
   {
      points_written++;
      Arc_Seg* aseg=(Arc_Seg*) _Segment;
      //middle gives direction
      if (_midrad < _endrad)
         GBR_out_file <<  "G03"; //counterclockwise
      else
         GBR_out_file <<  "G02"; //Clockwise
      WriteNumber(_next.GetX());
      GBR_out_file << "Y";
      WriteNumber(_next.GetY());
      GBR_out_file << "I";
      WriteNumber(_mid.GetX()-aseg->GetX());
      GBR_out_file << "J";
      WriteNumber(_mid.GetY()-aseg->GetY());
      GBR_out_file << endl;
      if (_second)
      {
	      GBR_out_file << "D01*"<< endl;
      	_second=false;
      }
      else
	      GBR_out_file << "*" << endl;
   }
}



bool GBRDriver::NeedToDraw(int layer)
{
	if (_ActiveLayer != layer)
   	return false;
	return true;
}

// Boundary -----------------------------------------------------------------
void GBRDriver::Write(Boundary* _Boundary)
{
	// There are two cases where a copy of the boundary's polygon should be made,
   // if the polygon is not absolute or if the polygon contains arc's. If one of
   // these condintions is true (or both) a copy should be made to perform operations on.
   bool ident= _matrix->IsIdentity(); //to now if we are at top level or not.

	G_Polygon* todraw = _AbsPolygon;
   bool	arcs 	= _Boundary->ContainsArcs();

	// Convert boundary to polygon with width
	if ((_Boundary->GetWidth() != 0) || (! ident))
   {
		_Boundary->Convert(_AbsPolygon, GLOB->DISPLAYABER());
		_AbsPolygon->Transform(_matrix);
      todraw = _AbsPolygon;
   } else {
   	todraw = _Boundary->GetPolygon();
   }

   _right=true;//default is right radius correction
   _pulsed=false;

	// write it's polygon
	Write( todraw);

}

void GBRDriver::Write(Path* _Path)
{
   G_Polygon*			todraw = _AbsPolygon;
   bool arcs 	= _Path->ContainsArcs();
   bool ident= _matrix->IsIdentity(); //to now if we are at top level or not.

	// Convert path to polygon with width
	if ((_Path->GetWidth() != 0) || (! ident))
   {
		_Path->Convert(_AbsPolygon, GLOB->DISPLAYABER());
		_AbsPolygon->Transform(_matrix);
      todraw = _AbsPolygon;
   } else {
      todraw = (G_Polygon*)_Path->GetPolyline();
   }

   _right=true;//default is right radius correction
   _pulsed=false;
   // draw its polyline
   if (_Path->GetWidth())
      Write(todraw);
   else
      Write((G_Polyline*) todraw);
}


void GBRDriver::Write(Arcseg* _Arcseg)

{
	// Convert arcseg to polygon
	_Arcseg->Convert(_AbsPolygon, GLOB->DISPLAYABER());

	_AbsPolygon->Transform(_matrix);

	_AbsPolygon->ConvertArcs(GLOB->DISPLAYABER());

	// write it's polygon
	Write(_AbsPolygon);
}

int GBRDriver::rad2deg(double value)
{
	return (int)(floor(value *10/M_PI*180 +  0.5));
}

// Box -----------------------------------------------------------------
void GBRDriver::Write(Box* _Box)
{
	_Box->Convert(_AbsPolygon, GLOB->DISPLAYABER());
	_AbsPolygon->Transform(_matrix);

	Write( _AbsPolygon);
}


// Circle -----------------------------------------------------------------
void GBRDriver::Write(Circle* _Circle)
{
	// Convert circle to polygon
	_Circle->Convert(_AbsPolygon, GLOB->DISPLAYABER());

	_AbsPolygon->Transform(_matrix);

	_AbsPolygon->ConvertArcs(GLOB->DISPLAYABER());

	// write as polygon
	Write(_AbsPolygon);
}

void GBRDriver::Write(Element* _Element)
{
   switch (_Element->GetType())
   {
      case BOUNDARY:
      case PATH:
      case ARCSEG:
      case TEXT:
      case BOX:
      case CIRCLE:
          {
            Shape* a_shape= (Shape*) _Element;
            // check if this layer must be written
            if (! NeedToDraw(a_shape->GetLayer())) return;
            if ((_only_visible) && (!GLOB->SETUP->Get_Visible(a_shape->GetLayer())))
                  return;
            // set a layer and layercolor
            SetLayer(a_shape->GetLayer());
          }
          break;
      case AREF:
      case SREF:
            break;
   }

   OWrite(_Element);
	UnSetLayer();
	 return;
}

// GdsII --------------------------------------------------------------------
void GBRDriver::Write(GdsII* _GdsII)
{
	//	GBR_out_file << "(* GERBER file for GDS-II postprocessing tool *)" << endl;
	GBR_out_file << "G54D980*" << endl;
	m_active=GBR_NORMAL;

	Write(_GdsII->GetLibrary());

   _showstructure=_GdsII->GetShowStructure();

   _matrix->Identity();

	// begin drawing the first structure if any
	if (_showstructure)
   {
      //setup an array with the layer order as index
		int	layerorder[MAX_LAYERS];
		for (int layer=0; layer < MAX_LAYERS; layer++)
      {  int curorder=GLOB->SETUP->Get_Order(layer);
         if (curorder < 0) //wrong order number
         {
            char buf[80];
            sprintf(buf,"Order for Layer %d, must be >=0 and < %d set to 0",layer,MAX_LAYERS);
				_messagehandler->info(buf, "Error");
				layerorder[0] = layer;
         }
         else if (curorder >= MAX_LAYERS) //wrong order number
         {
            char buf[80];
            sprintf(buf,"Order for Layer %d, must be >=0 and < %d set to MAX_LAYERS",layer,MAX_LAYERS);
				_messagehandler->info(buf, "Error");
				layerorder[MAX_LAYERS-1] = layer;
         }
         else
				layerorder[curorder] = layer;
      }

//		if (_drawstyle != FILLED)
//      	*this << _showstructure;
//		else
      {
			for (int order = MAX_LAYERS-1; order >= 0; order--)
         {
				_ActiveLayer = layerorder[order];
				if ((GLOB->SETUP->Get_Available(_ActiveLayer))&&(GLOB->SETUP->Get_Visible(_ActiveLayer)))
            {
					//GBR_out_file << "(* " << GLOB->SETUP->Get_Name(_ActiveLayer) << " " << _ActiveLayer << " *)"  << endl;
					Write(_showstructure);
            }
			}
		}
 	} else throw GDS_Error("No Structures in this GDS-II file", "Screendriver warning", 5, 0);

   //end of the file
	GBR_out_file << "X0Y0D02*M02*" << endl;
}


// Structure ----------------------------------------------------------------
void GBRDriver::Write(Structure* _Structure)
{
	_Structure->SortOnBoundingBox();

	// write the boundaries, text, paths etc
   //*this << (ElementList*)_Structure;

	TDLI<Element> I=TDLI<Element>(_Structure);
   I.toroot();
	while (I.iterate())
		Write(I.item());
}

void GBRDriver::Write(ArrayReference* _ArrayReference)
{
	// NOTE: matrix in structurereference wil be copied in
	//       the driver it's matrix!
 	//       When the function is completed it will be copied
	//			back in structurereference. (swapped)

	// do we have to draw this object?

 	// first make a copy in the driver it's matrix
	wxTransformMatrix*_backup = new wxTransformMatrix(*_matrix);
	assert(_backup != 0);

	*_matrix *= (*_ArrayReference->GetRelativeMatrix());

   // get row and column distance
   int columns=_ArrayReference->GetColumns();
   int rows=_ArrayReference->GetRows();

   Point offset;
   _ArrayReference->GetPoint(offset);
   Point hor=*_ArrayReference->GetHorzPoint();
   Point ver=*_ArrayReference->GetVertPoint();

   double dx1=(hor.GetX()-offset.GetX())/columns;
   double dy1=(hor.GetY()-offset.GetY())/columns;
   double dx2=(ver.GetX()-offset.GetX())/rows;
   double dy2=(ver.GetY()-offset.GetY())/rows;

   for (int i = 0  ; i < rows ; i++)
   {
	   for (int j = 0  ; j < columns ; j++)
      {
         // Translate2D over XY from the structurereference
			Write( _ArrayReference->GetStructure());
         _matrix->Translate(dx1,dy1);
      }
      //translate back in dx1*columns and in dy1
      _matrix->Translate(-dx1*columns + dx2,-dy1*columns +dy2);
   }

	// now copy matrix back
	*_matrix = *_backup;
	delete _backup;
}

void GBRDriver::Write(StructureReference* _StructureReference)
{
	// NOTE: matrix in structurereference wil be copied in
	//       the driver it's matrix!
 	//       When the function is completed it will be copied
	//			back in structurereference. (swapped)

	// do we have to draw this object?

	// first make a copy in the driver it's matrix
	wxTransformMatrix*_backup = new wxTransformMatrix(*_matrix);
	assert(_backup != 0);

	*_matrix *= (*_StructureReference->GetRelativeMatrix());

	Write( _StructureReference->GetStructure());

	// now copy matrix back
	*_matrix = *_backup;
	delete _backup;
}



// Polygon ------------------------------------------------------------------
void GBRDriver::Write(G_Polygon* _Polygon)
{
   TDLI<Segment> 	Iter=TDLI<Segment>(_Polygon); // first make iterator


	//GBR_out_file  << "(* polygon  XY "  << (_Polygon->count()+1) << " *)" << endl;

   _first=true;
	points_written = 0;
	Iter.tohead();
	while (!Iter.hitroot())
	{
      switch (Iter.item()->GetHoleType())
      {
         case NORMAL_SEG:
         	if (m_active != GBR_NORMAL)
            {
		         m_active=GBR_NORMAL;GBR_out_file  << "G54D980*" << endl;
            }
            break;
         case HOLE_SEG:
         	if (m_active != GBR_HOLE)
            {
		         m_active=GBR_HOLE;GBR_out_file  << "G54D981*" << endl;
            }
            break;
      }

		if (Iter.item()->GetType()==ARC )
      {
         Iter.next_wrap();
         _next.SetX(Iter.item()->GetX());
         _next.SetY(Iter.item()->GetY());
         Iter.prev_wrap();
         //calculate temp data to be able to write arc segment correctly
			_Polygon->CalcArc(&Iter,_mid,_Radius,_beginrad,_midrad,_endrad,_phit);
      }

		Write(Iter.item());
	   _first=false;
      if (Iter.item()->GetHoleType()== LINK_SEG)
      {
         _first=true;
      }
		Iter++;
	}

	Iter.tohead();

	Point end(Iter.item()->GetX(),Iter.item()->GetY());

	// write the first point again to close the polygon
	Write( &end);
}


// Polyline -----------------------------------------------------------------
void GBRDriver::Write(G_Polyline* _Polyline)
{
   TDLI<Segment> 	Iter=TDLI<Segment>(_Polyline); // first make iterator

	//GBR_out_file  << "(* polyline  XY "  << (_Polyline->count()) << " *)" << endl;

   _first=true;
	points_written = 0;
	Iter.tohead();
	while (!Iter.hitroot())
	{
		if (Iter.item()->GetType()==ARC )
      {
         Iter.next_wrap();
         _next.SetX(Iter.item()->GetX());
         _next.SetY(Iter.item()->GetY());
         Iter.prev_wrap();
         //calculate temp data to be able to write arc segment correctly
			_Polyline->CalcArc(&Iter,_mid,_Radius,_beginrad,_midrad,_endrad,_phit);
      }
		Write(Iter.item());
	   _first=false;
		Iter++;
	}
}

// Text ---------------------------------------------------------------------
void GBRDriver::Write(Text* _Text)
{
	Point min_point = _Text->GetMin();
	Point max_point = _Text->GetMax();

	GBR_out_file << "(*" << _Text->MyText() << "*)";
	GBR_out_file << endl;
}





