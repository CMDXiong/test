/*
Program	CNCHIER.CPP
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include <fstream>

using namespace std;

#include "arc.h"
#include "polyline.h"
#include "boundary.h"
#include "referenc.h"
#include "elemlst.h"
#include "gdsii.h"
#include "path.h"
#include "text.h"
#include "cnchier.h"
#include <iomanip>
#include <fstream>
#include "units.h"
#include "alias.h"
#include "proplist.h"
#include "structur.h"

// contructor  of screendriver
// makes a matrix and set boundingboxes
HierDriver::HierDriver(const wxString& _filename, bool only_visible)
{
	// holds temporary matrix from a structurereference
	// this will be modified in the references to calculate absolute positions
	_matrix = new wxTransformMatrix();
	if (_matrix == 0)
		throw GDS_Error("Cannot allocate memory for a matrix",
			"Screendriver Error", 0, 1);

	_only_visible = true;
	_visible = true;
	_pulsed  =  false;

	_AbsPolygon = new G_Polygon();

   _AI			= new TDLI<Segment>();

   _only_visible = only_visible;
	points_written = 0;
   CNC_out_filename=_filename;
	CNC_out_file.open(CNC_out_filename, ios::out);
}

// destructor: frees dynamic allocated memory
HierDriver::~HierDriver()
{
	delete _matrix;

   delete _AI;

	if (_AbsPolygon) delete _AbsPolygon;

	CNC_out_file.close();
}

void HierDriver::WriteNumber(double big)
{
      // Want floats to print as fixed point
      CNC_out_file.setf(ios::fixed,ios::floatfield);
      // Want floats to always have decimal point
      CNC_out_file.setf(ios::showpoint);
	   CNC_out_file.width(8);
#ifdef __UNIX__
      CNC_out_file << big*GLOB->PHYSUNITS()*1000;  //mm
#else
      CNC_out_file << setprecision(3) << big*GLOB->PHYSUNITS()*1000; //mm
#endif
}


// Point --------------------------------------------------------------------
void HierDriver::Write(Point *a_point)
{
	points_written++;

   if (_pulsed)
   {
      if (_first)
      {
         CNC_out_file <<  "G01 X";              //goto first point
         WriteNumber(a_point->GetX()-m_px);
         CNC_out_file << " Y";
         WriteNumber(a_point->GetY()-m_py);
         CNC_out_file << endl;
         CNC_out_file <<  "G4  X.2" << endl;        //delay
         CNC_out_file <<  "F[Q14]" << endl; //SPEED during cutting
         CNC_out_file <<  "T12" << endl;         //tool for polygons/lines cutting
         CNC_out_file <<  "M6" << endl;     //laser on
         m_px=a_point->GetX();
         m_py=a_point->GetY();
      }
      else
      {
         CNC_out_file <<  "G73 X";
         WriteNumber(a_point->GetX()-m_px);
         CNC_out_file << " Y";
         WriteNumber(a_point->GetY()-m_py);
         CNC_out_file << endl;
         m_px=a_point->GetX();
         m_py=a_point->GetY();
      }
   }
   else
   {
      if (_first)
      {
         CNC_out_file <<  "G01 X";              //goto first point
         WriteNumber(a_point->GetX()-m_px);
         CNC_out_file << " Y";
         WriteNumber(a_point->GetY()-m_py);
         CNC_out_file << endl;
         CNC_out_file <<  "G4  X.2" << endl;        //delay
         CNC_out_file <<  "F[Q12]" << endl; //SPEED during cutting
         CNC_out_file <<  "T12" << endl;         //tool for polygons/lines cutting
         if (_radcor)
         {
            if (_right)
               CNC_out_file <<  "G42 D1 ";        //radius correction right
            else
               CNC_out_file <<  "G41 D1 ";        //radius correction left
            CNC_out_file <<  "Y 0" ;
         }
         else
               CNC_out_file <<  "G40 ";        //radius non
         CNC_out_file << endl;
         CNC_out_file <<  "M4" << endl;     //laser on
         m_px=a_point->GetX();
         m_py=a_point->GetY();
      }
      else
      {
         CNC_out_file <<  "G01 X";
         WriteNumber(a_point->GetX()-m_px);
         CNC_out_file << " Y";
         WriteNumber(a_point->GetY()-m_py);
         CNC_out_file << endl;
         m_px=a_point->GetX();
         m_py=a_point->GetY();
      }
   }

}

// Segment  -----------------------------------------------------------------
void HierDriver::Write(Segment *_Segment)
{
	points_written++;

   if (_pulsed)
   {
      if (_first)
      {
         CNC_out_file <<  "G01 X";              //goto first point
         WriteNumber(_Segment->GetX()-m_px);
         CNC_out_file << " Y";
         WriteNumber(_Segment->GetY()-m_py);
         CNC_out_file << endl;
         CNC_out_file <<  "G4  X.2" << endl;        //delay
         CNC_out_file <<  "F[Q14]" << endl;      //SPEED during cutting
         CNC_out_file <<  "T12" << endl;         //tool for polygons/lines cutting
         CNC_out_file <<  "M6" << endl;         //pulsed on
      }
      else
      {
         CNC_out_file <<  "G73 X";
         WriteNumber(_Segment->GetX()-m_px);
         CNC_out_file << " Y";
         WriteNumber(_Segment->GetY()-m_py);
         CNC_out_file << endl;
      }

      if (_Segment->GetType()==ARC )
      {
         m_px=_Segment->GetX();
         m_py=_Segment->GetY();
         points_written++;
         Arc_Seg* aseg=(Arc_Seg*) _Segment;
         //middle gives direction
         if (_midrad < _endrad)
            CNC_out_file <<  "G03"; //counterclockwise
         else
            CNC_out_file <<  "G02"; //Clockwise
         CNC_out_file << endl;
         CNC_out_file <<  "G73 X"; //pulsed
         WriteNumber(_next.GetX()-m_px);
         CNC_out_file << " Y";
         WriteNumber(_next.GetY()-m_py);
         CNC_out_file << " I";
         WriteNumber(_mid.GetX()-aseg->GetX()-m_px);
         CNC_out_file << " J";
         WriteNumber(_mid.GetY()-aseg->GetY()-m_py);
         CNC_out_file << endl;
      }
      m_px=_Segment->GetX();
      m_py=_Segment->GetY();
   }
   else
   {
		if (_first)
      {
         CNC_out_file <<  "G01 X";              //goto first point
         WriteNumber(_Segment->GetX()-m_px);
         CNC_out_file << " Y";
         WriteNumber(_Segment->GetY()-m_py);
         CNC_out_file << endl;
         CNC_out_file <<  "G4  X.2" << endl;        //delay
         CNC_out_file <<  "F[Q12]" << endl;      //SPEED during cutting
         CNC_out_file <<  "T12" << endl;         //tool for polygons/lines cutting
         if (_radcor)
         {
            if (_right)
               CNC_out_file <<  "G42 D1 ";        //radius correction right
            else
               CNC_out_file <<  "G41 D1 ";        //radius correction left
            CNC_out_file <<  "Y 0" ;
         }
         else
               CNC_out_file <<  "G40 ";        //radius non
         CNC_out_file << endl;
         CNC_out_file <<  "M4" << endl;     //laser on
      }
      else
      {
         CNC_out_file <<  "G01 X";
         WriteNumber(_Segment->GetX()-m_px);
         CNC_out_file << " Y";
         WriteNumber(_Segment->GetY()-m_py);
         CNC_out_file << endl;
      }

      if (_Segment->GetType()==ARC )
      {
         m_px=_Segment->GetX();
         m_py=_Segment->GetY();
         points_written++;
         Arc_Seg* aseg=(Arc_Seg*) _Segment;
         //middle gives direction
         if (_midrad < _endrad)
            CNC_out_file <<  "G03X"; //counterclockwise
         else
            CNC_out_file <<  "G02X"; //Clockwise
         WriteNumber(_next.GetX()-m_px);
         CNC_out_file << " Y";
         WriteNumber(_next.GetY()-m_py);
         CNC_out_file << " I";
         WriteNumber(_mid.GetX()-aseg->GetX()-m_px);
         CNC_out_file << " J";
         WriteNumber(_mid.GetY()-aseg->GetY()-m_py);
         CNC_out_file << endl;
      }
      m_px=_Segment->GetX();
      m_py=_Segment->GetY();
   }
}


bool HierDriver::NeedToDraw(int layer)
{
	if (_ActiveLayer != layer)
   	return false;
	return true;
}

// Boundary -----------------------------------------------------------------
void HierDriver::Write(Boundary* _Boundary)
{
	// There are two cases where a copy of the boundary's polygon should be made,
   // if the polygon is not absolute or if the polygon contains arc's. If one of
   // these condintions is true (or both) a copy should be made to perform operations on.

	G_Polygon* todraw = _AbsPolygon;
   bool	arcs 	= _Boundary->ContainsArcs();
   bool ident= (_drawstructure==_showstructure); //to now if we are at top level or not.

	// Convert boundary to polygon with width
	if ((_Boundary->GetWidth() != 0) || (! ident))
   {
		_Boundary->Convert(_AbsPolygon, GLOB->DISPLAYABER());
		_AbsPolygon->Transform(_matrix);
      todraw = _AbsPolygon;
   } else {
   	todraw = _Boundary->GetPolygon();
   }

   _radcor=false;
   _right=true;//default is right radius correction
   _pulsed=false;
   if (_Boundary->HasPropList())
   {
      if (_Boundary->GetPropList()->HasProperty("radcor"))
      {
		   _radcor=true;
			wxProperty* aprop=_Boundary->GetPropList()->GetProperty("radcor");
         if ( !stricmp(aprop->GetValue().StringValue(),"left"))
	         _right=false;
      }
      if (_Boundary->GetPropList()->HasProperty("pulsed"))
         _pulsed=true;

   }

	// write it's polygon
	Write( todraw);

	CNC_out_file  << "G40"  << endl; //cancel radius compensation
   if (_pulsed)
		CNC_out_file  << "M7"  << endl;  //pulsed off
   else
		CNC_out_file  << "M5"  << endl;  //laser off
	CNC_out_file  << "G4 X.1"  << endl;  //delay
}

void HierDriver::Write(Path* _Path)
{
   G_Polygon*			todraw = _AbsPolygon;
   bool arcs 	= _Path->ContainsArcs();
   bool ident= (_drawstructure==_showstructure); //to now if we are at top level or not.

	// Convert path to polygon with width
	if ((_Path->GetWidth() != 0) || (! ident))
   {
		_Path->Convert(_AbsPolygon, GLOB->DISPLAYABER());
		_AbsPolygon->Transform(_matrix);
      todraw = _AbsPolygon;
   } else {
      todraw = (G_Polygon*)_Path->GetPolyline();
   }

   _radcor=false;
   _right=true;//default is right radius correction
   _pulsed=false;
   if (_Path->HasPropList())
   {
      if (_Path->GetPropList()->HasProperty("hole"))
			AsHole((G_Polyline*) todraw);
      else
      {
         if (_Path->GetPropList()->HasProperty("radcor"))
         {
			   _radcor=true;
            wxProperty* aprop=_Path->GetPropList()->GetProperty("radcor");
            if ( !stricmp(aprop->GetValue().StringValue(),"left"))
               _right=false;
         }
         if (_Path->GetPropList()->HasProperty("pulsed"))
            _pulsed=true;

         // draw its polyline
         if (_Path->GetWidth())
            Write(todraw);
         else
            Write((G_Polyline*) todraw);
         CNC_out_file  << "G40"  << endl; //cancel radius compensation
         if (_pulsed)
            CNC_out_file  << "M7"  << endl;  //pulsed off
         else
            CNC_out_file  << "M5"  << endl;  //laser off
         CNC_out_file  << "G4 X.1"  << endl;  //delay
      }
   }
   else
   {
      // draw its polyline
      if (_Path->GetWidth())
         Write(todraw);
      else
         Write((G_Polyline*) todraw);
      CNC_out_file  << "G40"  << endl; //cancel radius compensation
      if (_pulsed)
         CNC_out_file  << "M7"  << endl;  //pulsed off
      else
         CNC_out_file  << "M5"  << endl;  //laser off
      CNC_out_file  << "G4 X.1"  << endl;  //delay
   }

}


void HierDriver::Write(Arcseg* _Arcseg)

{
	// Convert arcseg to polygon
	_Arcseg->Convert(_AbsPolygon, GLOB->DISPLAYABER());

	_AbsPolygon->Transform(_matrix);

	_AbsPolygon->ConvertArcs(GLOB->DISPLAYABER());

	// write it's polygon
	Write(_AbsPolygon);
}

int HierDriver::rad2deg(double value)
{
	return (int)(floor(value *10/M_PI*180 +  0.5));
}

// Box -----------------------------------------------------------------
void HierDriver::Write(Box* _Box)
{
	_Box->Convert(_AbsPolygon, GLOB->DISPLAYABER());
	_AbsPolygon->Transform(_matrix);

   double	X[3];	double	Y[3];   // to store points(x,y)
   int Points_Written = 0;
 	TDLI<Segment> 	Iter=TDLI<Segment>(_AbsPolygon);
   Iter.tohead();
   int i;
   for (i = 0;i<3;i++) // eerste drie punten zijn belangrijk.
   {
      X[Points_Written] = Iter.item()->GetX();
      Y[Points_Written] = Iter.item()->GetY();
      Points_Written++;
      Iter++;
   }

   double 	a;
   double	w;
   double 	h;
   double	x;
   double	y;

   if (X[1] - X[0] == 0)
   {
      if (Y[1]-Y[0] > 0 )
          a=M_PI/2.0;
      else
          a=-M_PI/2.0;
   }
   else
      a = atan2(Y[1]-Y[0],X[1]-X[0]);

   w = sqrt((X[1]-X[0])*(X[1]-X[0]) + (Y[1]-Y[0])*(Y[1]-Y[0]));
   h = sqrt((X[2]-X[1])*(X[2]-X[1]) + (Y[2]-Y[1])*(Y[2]-Y[1]));
   //x = X[0] + 0.5*w*cos(a) + 0.5*h*sin(a);
   //y = Y[0] + 0.5*w*sin(a) + 0.5*h*cos(a);
   x = (X[0]+X[2])/2;
   y = (Y[0]+Y[2])/2;

   // juiste punten zijn uitgerekend. is getest.

   a=a *1/M_PI*180.0;

   CNC_out_file <<  "Q50=";
   WriteNumber(x-m_px);
   CNC_out_file <<  " Q51=";
   WriteNumber(y-m_py);
   CNC_out_file <<  " Q60=";
   m_px=x;
   m_py=y;

   WriteNumber(w);
   CNC_out_file <<  " Q61=";
   WriteNumber(h);
   CNC_out_file <<  " Q62=";
#ifdef __UNIX__
   CNC_out_file << a; //degrees
#else
   CNC_out_file << setprecision(4) << a; //degrees
#endif
   CNC_out_file << endl;
   CNC_out_file <<  "M98 (C:\\CNC\\PROGRAMS\\BOX.SUB)";
   CNC_out_file << endl;

}


// Circle -----------------------------------------------------------------
void HierDriver::Write(Circle* _Circle)
{
	// Convert circle to polygon
	_Circle->Convert(_AbsPolygon, GLOB->DISPLAYABER());

	_AbsPolygon->Transform(_matrix);

	_AbsPolygon->ConvertArcs(GLOB->DISPLAYABER());

	// write as polygon
	//*this << _AbsPolygon;

   CNC_out_file <<  "Q9=";
   WriteNumber(_Circle->GetMid().GetX()-m_px);
   CNC_out_file <<  " Q10=";
   WriteNumber(_Circle->GetMid().GetY()-m_py);
   CNC_out_file <<  " Q11=";
   m_px=_Circle->GetMid().GetX();
   m_py=_Circle->GetMid().GetY();
   WriteNumber(_Circle->GetRadius()*2);
   CNC_out_file << endl;
   CNC_out_file <<  "M98 (C:\\CNC\\PROGRAMS\\CIRCLE.SUB)";
   CNC_out_file << endl;
}

void HierDriver::Write(Element* _Element)
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

   bool ident= (_drawstructure==_showstructure); //to now if we are at top level or not.

	CNC_out_file <<  "F[Q13]" << endl;      //SPEED during move

   OWrite(_Element);
	UnSetLayer();
	 return;
}


// GdsII --------------------------------------------------------------------
void HierDriver::Write(GdsII* _GdsII)
{
   m_px=0;
   m_py=0;
   wxString aliasstring;
   if (GLOB->ALIASLIST->GetAlias("cncheader",aliasstring))
   {
      CNC_out_file << "(* CNC file for GDS-II postprocessing tool *)" << endl;
      CNC_out_file << "(* File = " << CNC_out_filename << "*)" << endl;
   	CNC_out_file << aliasstring << endl;
   	CNC_out_file << "G91" << endl; //incrimental programmed
   }
   else
   {
      CNC_out_file << "(* CNC file for GDS-II postprocessing tool *)" << endl;
      CNC_out_file << "(* File = " << CNC_out_filename << "*)" << endl;
      CNC_out_file << "Q12=150" << endl;
      CNC_out_file << "Q3=37.5  (X alignment hole1)" << endl;
      CNC_out_file << "Q4=-1.8  (Y alignment hole1)" << endl;
      CNC_out_file << "Q5=37.5  (X alignment hole2)" << endl;
      CNC_out_file << "Q6=104.2 (Y alignment hole2)" << endl;
      CNC_out_file << "Q13=5000   (speed during move)" << endl;
      CNC_out_file << "Q14=150    (speed during pulsed move)" << endl;
   // now part of allign.sub
   //	CNC_out_file << "Q30=-99.103 (X distance  CAM - LASER)" << endl;
   //	CNC_out_file << "Q31=-8.021  (Y distance  CAM - LASER)" << endl;
      CNC_out_file << "G21 (metric mode)" << endl;
      CNC_out_file << "M61 (cancel all previous scalings)" << endl;
      CNC_out_file << "G57 (Zero mask)" << endl;
      CNC_out_file << "T11" << endl;
   //	CNC_out_file << "G90 G01 F[Q13]" << endl;
   //	CNC_out_file << "X0 Y185 (position to load)" << endl;
   //	CNC_out_file << "M0  (hold -> press start)" << endl;
      CNC_out_file << "(* reference alignment *)" << endl;
      CNC_out_file << "M98 (C:\\CNC\\PROGRAMS\\ALIGN.SUB)" << endl;
      CNC_out_file << "M11 (select lens gas)" << endl;
      CNC_out_file << "M8  (gaz on)" << endl;
      CNC_out_file << "M14 (shutter open)" << endl;
      CNC_out_file << "M98 (C:\\CNC\\PROGRAMS\\REFHOLES.SUB)" << endl;
      CNC_out_file << "G71 A0.05 (default pulse spacing)  " << endl;
   	CNC_out_file << "G91" << endl; //incrimental programmed
   }

	Write( _GdsII->GetLibrary());

   _showstructure=_GdsII->GetShowStructure();
   _drawstructure=_showstructure; //to now if we are at top level or not.

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
					CNC_out_file << "(* " << GLOB->SETUP->Get_Name(_ActiveLayer) << " " << _ActiveLayer << " *)"  << endl;
					Write( _showstructure);
            }
			}
		}
 	} else throw GDS_Error("No Structures in this GDS-II file", "Screendriver warning", 5, 0);

   //end of the file
	CNC_out_file << "M15 (close shutter)" << endl;
	CNC_out_file << "M9  (gaz off)" << endl;
	CNC_out_file << "M61" << endl;
	CNC_out_file << "F[Q13]" << endl;      //SPEED during move
	CNC_out_file << "G90 X0 Y185" << endl;
	CNC_out_file << "M30" << endl;
}


// Structure ----------------------------------------------------------------
void HierDriver::Write(Structure* _Structure)
{
   Structure* backupdraw=_drawstructure;
   _drawstructure=_Structure; //to now if we are at top level or not.
   bool ident= (_drawstructure==_showstructure); //to now if we are at top level or not.

	_Structure->SortOnXYmin();

	TDLI<Element> I=TDLI<Element>(_Structure);
   I.toroot();
	while (I.iterate())
   {
		Write(I.item());
   }

   //not this (skip the element write here
	//*this << (ElementList*)_Structure;

	// Put back the old value in _clip_status
   _drawstructure=backupdraw;
}

void HierDriver::Write(ArrayReference* _ArrayReference)
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

void HierDriver::Write(StructureReference* _StructureReference)
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
void HierDriver::Write(G_Polygon* _Polygon)
{
   TDLI<Segment> 	Iter=TDLI<Segment>(_Polygon); // first make iterator


	CNC_out_file  << "(* polygon  XY "  << (_Polygon->count()+1) << " *)" << endl;

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
			_Polygon->CalcArc(&Iter,_mid,_Radius,_beginrad,_midrad,_endrad,_phit);
      }
		Write( Iter.item());
	   _first=false;
		Iter++;
	}

	Iter.tohead();

	Point end(Iter.item()->GetX(),Iter.item()->GetY());

	// write the first point again to close the polygon
	Write( &end);
}


// Polyline -----------------------------------------------------------------
void HierDriver::Write(G_Polyline* _Polyline)
{
   TDLI<Segment> 	Iter=TDLI<Segment>(_Polyline); // first make iterator

	CNC_out_file  << "(* polyline  XY "  << (_Polyline->count()) << " *)" << endl;

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
		Write( Iter.item());
	   _first=false;
		Iter++;
	}
}

// Text ---------------------------------------------------------------------
void HierDriver::Write(Text* _Text)
{
	Point min_point = _Text->GetMin();
	Point max_point = _Text->GetMax();

	CNC_out_file << "(*" << _Text->MyText() << "*)";
	CNC_out_file << endl;
}

void HierDriver::AsHole(G_Polyline* _Polyline)
{
   TDLI<Segment> 	Iter=TDLI<Segment>(_Polyline); // first make iterator

   _first=true;
	points_written = 0;
	Iter.tohead();
   if (Iter.count() != 2)
     return; //no hole
   double xm,ym,x1,y1,x2,y2,d;
   x1=Iter.item()->GetX();
   y1=Iter.item()->GetY();
   Iter++;
   x2=Iter.item()->GetX();
   y2=Iter.item()->GetY();
   xm=(x1+x2)/2.0;
   ym=(y1+y2)/2.0;
   d=sqrt(pow(x2-x1,2)+pow(y2-y1,2));
   CNC_out_file <<  "Q9=";
   WriteNumber(xm);
   CNC_out_file <<  " Q10=";
   WriteNumber(ym);
   CNC_out_file <<  " Q11=";
   WriteNumber(d);
   CNC_out_file << endl;
   CNC_out_file <<  "M98 (C:\\CNC\\PROGRAMS\\HOLE1.SUB)";
   CNC_out_file << endl;
}
/*
// contructor  of screendriver
// makes a matrix and set boundingboxes
HierDriver::HierDriver(const wxString& _filename, bool only_visible)
{
	// holds temporary matrix from a structurereference
	// this will be modified in the references to calculate absolute positions
	_matrix = new wxTransformMatrix();
	if (_matrix == 0)
		throw GDS_Error("Cannot allocate memory for a matrix",
			"Screendriver Error", 0, 1);

	_only_visible = true;
	_visible = true;
	_pulsed  =  false;

	_AbsPolygon = new G_Polygon();

   _AI			= new TDLI<Segment>();

   _only_visible = only_visible;
	points_written = 0;
   CNC_out_filename=_filename;
	CNC_out_file.open(CNC_out_filename, ios::out);
}

// destructor: frees dynamic allocated memory
HierDriver::~HierDriver()
{
	delete _matrix;

   delete _AI;

	if (_AbsPolygon) delete _AbsPolygon;

	CNC_out_file.close();
}

void HierDriver::WriteNumber(double big)
{

	   CNC_out_file.width(8);
#ifdef __UNIX__
      CNC_out_file << big*GLOB->PHYSUNITS()*1000;  //mm
#else
      CNC_out_file << setprecision(4) << big*GLOB->PHYSUNITS()*1000; //mm
#endif
}


// Point --------------------------------------------------------------------
void HierDriver::Write(Point *a_point)
{
	points_written++;

   if (_pulsed)
   {
      if (_first)
      {
         CNC_out_file <<  "G01 X";              //goto first point
         WriteNumber(a_point->GetX());
         CNC_out_file << " Y";
         WriteNumber(a_point->GetY());
         CNC_out_file << endl;
         CNC_out_file <<  "G4  X.2" << endl;        //delay
         CNC_out_file <<  "F[Q14]" << endl; //SPEED during cutting
         CNC_out_file <<  "T12" << endl;         //tool for polygons/lines cutting
         CNC_out_file <<  "M6" << endl;     //laser on
      }
      else
      {
         CNC_out_file <<  "G73 X";
         WriteNumber(a_point->GetX());
         CNC_out_file << " Y";
         WriteNumber(a_point->GetY());
         CNC_out_file << endl;
      }
   }
   else
   {
      if (_first)
      {
         CNC_out_file <<  "G01 X";              //goto first point
         WriteNumber(a_point->GetX());
         CNC_out_file << " Y";
         WriteNumber(a_point->GetY());
         CNC_out_file << endl;
         CNC_out_file <<  "G4  X.2" << endl;        //delay
         CNC_out_file <<  "F[Q12]" << endl; //SPEED during cutting
         CNC_out_file <<  "T12" << endl;         //tool for polygons/lines cutting
         if (_radcor)
         {
            if (_right)
               CNC_out_file <<  "G42 D1 ";        //radius correction right
            else
               CNC_out_file <<  "G41 D1 ";        //radius correction left
         }
         else
               CNC_out_file <<  "G40 ";        //radius non
         CNC_out_file <<  "Y" ;
         WriteNumber(a_point->GetY());
         CNC_out_file << endl;
         CNC_out_file <<  "M4" << endl;     //laser on
      }
      else
      {
         CNC_out_file <<  "G01 X";
         WriteNumber(a_point->GetX());
         CNC_out_file << " Y";
         WriteNumber(a_point->GetY());
         CNC_out_file << endl;
      }
   }

}

// Segment  -----------------------------------------------------------------
void HierDriver::Write(Segment *_Segment)
{
	points_written++;

   if (_pulsed)
   {
      if (_first)
      {
         CNC_out_file <<  "G01 X";              //goto first point
         WriteNumber(_Segment->GetX());
         CNC_out_file << " Y";
         WriteNumber(_Segment->GetY());
         CNC_out_file << endl;
         CNC_out_file <<  "G4  X.2" << endl;        //delay
         CNC_out_file <<  "F[Q14]" << endl;      //SPEED during cutting
         CNC_out_file <<  "T12" << endl;         //tool for polygons/lines cutting
         CNC_out_file <<  "M6" << endl;         //pulsed on
      }
      else
      {
         CNC_out_file <<  "G73 X";
         WriteNumber(_Segment->GetX());
         CNC_out_file << " Y";
         WriteNumber(_Segment->GetY());
         CNC_out_file << endl;
      }

      if (_Segment->GetType()==ARC )
      {
         points_written++;
         Arc_Seg* aseg=(Arc_Seg*) _Segment;
         //middle gives direction
         if (_midrad < _endrad)
            CNC_out_file <<  "G03"; //counterclockwise
         else
            CNC_out_file <<  "G02"; //Clockwise
         CNC_out_file << endl;
         CNC_out_file <<  "G73 X"; //pulsed
         WriteNumber(_next.GetX());
         CNC_out_file << " Y";
         WriteNumber(_next.GetY());
         CNC_out_file << " I";
         WriteNumber(_mid.GetX()-aseg->GetX());
         CNC_out_file << " J";
         WriteNumber(_mid.GetY()-aseg->GetY());
         CNC_out_file << endl;
      }
   }
   else
   {
		if (_first)
      {
         CNC_out_file <<  "G01 X";              //goto first point
         WriteNumber(_Segment->GetX());
         CNC_out_file << " Y";
         WriteNumber(_Segment->GetY());
         CNC_out_file << endl;
         CNC_out_file <<  "G4  X.2" << endl;        //delay
         CNC_out_file <<  "F[Q12]" << endl;      //SPEED during cutting
         CNC_out_file <<  "T12" << endl;         //tool for polygons/lines cutting
         if (_radcor)
         {
            if (_right)
               CNC_out_file <<  "G42 D1 ";        //radius correction right
            else
               CNC_out_file <<  "G41 D1 ";        //radius correction left
         }
         else
               CNC_out_file <<  "G40 ";        //radius non
         CNC_out_file <<  "Y";
         WriteNumber(_Segment->GetY());
         CNC_out_file << endl;
         CNC_out_file <<  "M4" << endl;     //laser on
      }
      else
      {
         CNC_out_file <<  "G01 X";
         WriteNumber(_Segment->GetX());
         CNC_out_file << " Y";
         WriteNumber(_Segment->GetY());
         CNC_out_file << endl;
      }

      if (_Segment->GetType()==ARC )
      {
         points_written++;
         Arc_Seg* aseg=(Arc_Seg*) _Segment;
         //middle gives direction
         if (_midrad < _endrad)
            CNC_out_file <<  "G03X"; //counterclockwise
         else
            CNC_out_file <<  "G02X"; //Clockwise
         WriteNumber(_next.GetX());
         CNC_out_file << " Y";
         WriteNumber(_next.GetY());
         CNC_out_file << " I";
         WriteNumber(_mid.GetX()-aseg->GetX());
         CNC_out_file << " J";
         WriteNumber(_mid.GetY()-aseg->GetY());
         CNC_out_file << endl;
      }
   }
}


bool HierDriver::NeedToDraw(int layer)
{
	if (_ActiveLayer != layer)
   	return false;
	return true;
}

// Boundary -----------------------------------------------------------------
void HierDriver::Write(Boundary* _Boundary)
{
	// There are two cases where a copy of the boundary's polygon should be made,
   // if the polygon is not absolute or if the polygon contains arc's. If one of
   // these condintions is true (or both) a copy should be made to perform operations on.

	G_Polygon* todraw = _AbsPolygon;
   bool	arcs 	= _Boundary->ContainsArcs();
   bool ident= (_drawstructure==_showstructure); //to now if we are at top level or not.

	// Convert boundary to polygon with width
	if ((_Boundary->GetWidth() != 0) || (! ident))
   {
		_Boundary->Convert(_AbsPolygon, GLOB->DISPLAYABER());
		_AbsPolygon->Transform(_matrix);
      todraw = _AbsPolygon;
   } else {
   	todraw = _Boundary->GetPolygon();
   }

   _radcor=false;
   _right=true;//default is right radius correction
   _pulsed=false;
   if (_Boundary->HasPropList())
   {
      if (_Boundary->GetPropList()->HasProperty("radcor"))
      {
		   _radcor=true;
			wxProperty* aprop=_Boundary->GetPropList()->GetProperty("radcor");
         if ( !stricmp(aprop->GetValue().StringValue(),"left"))
	         _right=false;
      }
      if (_Boundary->GetPropList()->HasProperty("pulsed"))
         _pulsed=true;

   }

	// write it's polygon
	Write( todraw);

	CNC_out_file  << "G40"  << endl; //cancel radius compensation
   if (_pulsed)
		CNC_out_file  << "M7"  << endl;  //pulsed off
   else
		CNC_out_file  << "M5"  << endl;  //laser off
	CNC_out_file  << "G4 X.1"  << endl;  //delay
}

void HierDriver::Write(Path* _Path)
{
   G_Polygon*			todraw = _AbsPolygon;
   bool arcs 	= _Path->ContainsArcs();
   bool ident= (_drawstructure==_showstructure); //to now if we are at top level or not.

	// Convert path to polygon with width
	if ((_Path->GetWidth() != 0) || (! ident))
   {
		_Path->Convert(_AbsPolygon, GLOB->DISPLAYABER());
		_AbsPolygon->Transform(_matrix);
      todraw = _AbsPolygon;
   } else {
      todraw = (G_Polygon*)_Path->GetPolyline();
   }

   _radcor=false;
   _right=true;//default is right radius correction
   _pulsed=false;
   if (_Path->HasPropList())
   {
      if (_Path->GetPropList()->HasProperty("hole"))
			AsHole((G_Polyline*) todraw);
      else
      {
         if (_Path->GetPropList()->HasProperty("radcor"))
         {
			   _radcor=true;
            wxProperty* aprop=_Path->GetPropList()->GetProperty("radcor");
            if ( !stricmp(aprop->GetValue().StringValue(),"left"))
               _right=false;
         }
         if (_Path->GetPropList()->HasProperty("pulsed"))
            _pulsed=true;

         // draw its polyline
         if (_Path->GetWidth())
            Write(todraw);
         else
            Write((G_Polyline*) todraw);
         CNC_out_file  << "G40"  << endl; //cancel radius compensation
         if (_pulsed)
            CNC_out_file  << "M7"  << endl;  //pulsed off
         else
            CNC_out_file  << "M5"  << endl;  //laser off
         CNC_out_file  << "G4 X.1"  << endl;  //delay
      }
   }
   else
   {
      // draw its polyline
      if (_Path->GetWidth())
         Write(todraw);
      else
         Write((G_Polyline*) todraw);
      CNC_out_file  << "G40"  << endl; //cancel radius compensation
      if (_pulsed)
         CNC_out_file  << "M7"  << endl;  //pulsed off
      else
         CNC_out_file  << "M5"  << endl;  //laser off
      CNC_out_file  << "G4 X.1"  << endl;  //delay
   }

}


void HierDriver::Write(Arcseg* _Arcseg)

{
	// Convert arcseg to polygon
	_Arcseg->Convert(_AbsPolygon, GLOB->DISPLAYABER());

	_AbsPolygon->Transform(_matrix);

	_AbsPolygon->ConvertArcs(GLOB->DISPLAYABER());

	// write it's polygon
	Write(_AbsPolygon);
}

int HierDriver::rad2deg(double value)
{
	return (int)(floor(value *10/M_PI*180 +  0.5));
}

// Box -----------------------------------------------------------------
void HierDriver::Write(Box* _Box)
{
	_Box->Convert(_AbsPolygon, GLOB->DISPLAYABER());
	_AbsPolygon->Transform(_matrix);

   double	X[3];	double	Y[3];   // to store points(x,y)
   int Points_Written = 0;
 	TDLI<Segment> 	Iter=TDLI<Segment>(_AbsPolygon);
   Iter.tohead();
   int i;
   for (i = 0;i<3;i++) // eerste drie punten zijn belangrijk.
   {
      X[Points_Written] = Iter.item()->GetX();
      Y[Points_Written] = Iter.item()->GetY();
      Points_Written++;
      Iter++;
   }

   double 	a;
   double	w;
   double 	h;
   double	x;
   double	y;

   if (X[1] - X[0] == 0)
   {
      if (Y[1]-Y[0] > 0 )
          a=M_PI/2.0;
      else
          a=-M_PI/2.0;
   }
   else
      a = atan2(Y[1]-Y[0],X[1]-X[0]);

   w = sqrt((X[1]-X[0])*(X[1]-X[0]) + (Y[1]-Y[0])*(Y[1]-Y[0]));
   h = sqrt((X[2]-X[1])*(X[2]-X[1]) + (Y[2]-Y[1])*(Y[2]-Y[1]));
   //x = X[0] + 0.5*w*cos(a) + 0.5*h*sin(a);
   //y = Y[0] + 0.5*w*sin(a) + 0.5*h*cos(a);
   x = (X[0]+X[2])/2;
   y = (Y[0]+Y[2])/2;

   // juiste punten zijn uitgerekend. is getest.

   a=a *1/M_PI*180.0;

   CNC_out_file <<  "Q50=";
   WriteNumber(x);
   CNC_out_file <<  " Q51=";
   WriteNumber(y);
   CNC_out_file <<  " Q60=";
   WriteNumber(w);
   CNC_out_file <<  " Q61=";
   WriteNumber(h);
   CNC_out_file <<  " Q62=";
#ifdef __UNIX__
   CNC_out_file << a; //degrees
#else
   CNC_out_file << setprecision(4) << a; //degrees
#endif
   CNC_out_file << endl;
   CNC_out_file <<  "M98 (C:\\CNC\\PROGRAMS\\BOX.SUB)";
   CNC_out_file << endl;

}


// Circle -----------------------------------------------------------------
void HierDriver::Write(Circle* _Circle)
{
	// Convert circle to polygon
	_Circle->Convert(_AbsPolygon, GLOB->DISPLAYABER());

	_AbsPolygon->Transform(_matrix);

	_AbsPolygon->ConvertArcs(GLOB->DISPLAYABER());

	// write as polygon
	//*this << _AbsPolygon;

   CNC_out_file <<  "Q9=";
   WriteNumber(_Circle->GetMid().GetX());
   CNC_out_file <<  " Q10=";
   WriteNumber(_Circle->GetMid().GetY());
   CNC_out_file <<  " Q11=";
   WriteNumber(_Circle->GetRadius()*2);
   CNC_out_file << endl;
   CNC_out_file <<  "M98 (C:\\CNC\\PROGRAMS\\CIRCLE.SUB)";
   CNC_out_file << endl;
}

void HierDriver::Write(Element* _Element)
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

   bool ident= (_drawstructure==_showstructure); //to now if we are at top level or not.

	CNC_out_file <<  "F[Q13]" << endl;      //SPEED during move

   OWrite(_Element);
	UnSetLayer();
	 return;
}


// GdsII --------------------------------------------------------------------
void HierDriver::Write(GdsII* _GdsII)
{
   wxString aliasstring;
   if (GLOB->ALIASLIST->GetAlias("cncheader",aliasstring))
   {
      CNC_out_file << "(* CNC file for GDS-II postprocessing tool *)" << endl;
      CNC_out_file << "(* File = " << CNC_out_filename << "*)" << endl;
   	CNC_out_file << aliasstring << endl;
   }
   else
   {
      CNC_out_file << "(* CNC file for GDS-II postprocessing tool *)" << endl;
      CNC_out_file << "(* File = " << CNC_out_filename << "*)" << endl;
      CNC_out_file << "Q12=150" << endl;
      CNC_out_file << "Q3=37.5  (X alignment hole1)" << endl;
      CNC_out_file << "Q4=-1.8  (Y alignment hole1)" << endl;
      CNC_out_file << "Q5=37.5  (X alignment hole2)" << endl;
      CNC_out_file << "Q6=104.2 (Y alignment hole2)" << endl;
      CNC_out_file << "Q13=5000   (speed during move)" << endl;
      CNC_out_file << "Q14=150    (speed during pulsed move)" << endl;
   // now part of allign.sub
   //	CNC_out_file << "Q30=-99.103 (X distance  CAM - LASER)" << endl;
   //	CNC_out_file << "Q31=-8.021  (Y distance  CAM - LASER)" << endl;
      CNC_out_file << "G21 (metric mode)" << endl;
      CNC_out_file << "M61 (cancel all previous scalings)" << endl;
      CNC_out_file << "G57 (Zero mask)" << endl;
      CNC_out_file << "T11" << endl;
   //	CNC_out_file << "G90 G01 F[Q13]" << endl;
   //	CNC_out_file << "X0 Y185 (position to load)" << endl;
   //	CNC_out_file << "M0  (hold -> press start)" << endl;
      CNC_out_file << "(* reference alignment *)" << endl;
      CNC_out_file << "M98 (C:\\CNC\\PROGRAMS\\ALIGN.SUB)" << endl;
      CNC_out_file << "M11 (select lens gas)" << endl;
      CNC_out_file << "M8  (gaz on)" << endl;
      CNC_out_file << "M14 (shutter open)" << endl;
      CNC_out_file << "M98 (C:\\CNC\\PROGRAMS\\REFHOLES.SUB)" << endl;
      CNC_out_file << "G71 A0.05 (default pulse spacing)  " << endl;
   }

	Write( _GdsII->GetLibrary());

   _showstructure=_GdsII->GetShowStructure();
   _drawstructure=_showstructure; //to now if we are at top level or not.

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
					CNC_out_file << "(* " << GLOB->SETUP->Get_Name(_ActiveLayer) << " " << _ActiveLayer << " *)"  << endl;
					Write( _showstructure);
            }
			}
		}
 	} else throw GDS_Error("No Structures in this GDS-II file", "Screendriver warning", 5, 0);

   //end of the file
	CNC_out_file << "M15 (close shutter)" << endl;
	CNC_out_file << "M9  (gaz off)" << endl;
	CNC_out_file << "M61" << endl;
	CNC_out_file << "G90 X0 Y185" << endl;
	CNC_out_file << "M30" << endl;
}


// Structure ----------------------------------------------------------------
void HierDriver::Write(Structure* _Structure)
{
   Structure* backupdraw=_drawstructure;
   _drawstructure=_Structure; //to now if we are at top level or not.
   bool ident= (_drawstructure==_showstructure); //to now if we are at top level or not.

	_Structure->SortOnXYmin();

	TDLI<Element> I=TDLI<Element>(_Structure);
   I.toroot();
	while (I.iterate())
		Write(I.item());

   //not this (skip the element write here   
	//*this << (ElementList*)_Structure;

	// Put back the old value in _clip_status
   _drawstructure=backupdraw;
}

void HierDriver::Write(ArrayReference* _ArrayReference)
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

void HierDriver::Write(StructureReference* _StructureReference)
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
void HierDriver::Write(G_Polygon* _Polygon)
{
   TDLI<Segment> 	Iter=TDLI<Segment>(_Polygon); // first make iterator


	CNC_out_file  << "(* polygon  XY "  << (_Polygon->count()+1) << " *)" << endl;

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
			_Polygon->CalcArc(&Iter,_mid,_Radius,_beginrad,_midrad,_endrad,_phit);
      }
		Write( Iter.item());
	   _first=false;
		Iter++;
	}

	Iter.tohead();

	Point end(Iter.item()->GetX(),Iter.item()->GetY());

	// write the first point again to close the polygon
	Write( &end);
}


// Polyline -----------------------------------------------------------------
void HierDriver::Write(G_Polyline* _Polyline)
{
   TDLI<Segment> 	Iter=TDLI<Segment>(_Polyline); // first make iterator

	CNC_out_file  << "(* polyline  XY "  << (_Polyline->count()) << " *)" << endl;

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
		Write( Iter.item());
	   _first=false;
		Iter++;
	}
}

// Text ---------------------------------------------------------------------
void HierDriver::Write(Text* _Text)
{
	Point min_point = _Text->GetMin();
	Point max_point = _Text->GetMax();

	CNC_out_file << "(*" << _Text->MyText() << "*)";
	CNC_out_file << endl;
}

void HierDriver::AsHole(G_Polyline* _Polyline)
{
   TDLI<Segment> 	Iter=TDLI<Segment>(_Polyline); // first make iterator

   _first=true;
	points_written = 0;
	Iter.tohead();
   if (Iter.count() != 2)
     return; //no hole
   double xm,ym,x1,y1,x2,y2,d;
   x1=Iter.item()->GetX();
   y1=Iter.item()->GetY();
   Iter++;
   x2=Iter.item()->GetX();
   y2=Iter.item()->GetY();
   xm=(x1+x2)/2.0;
   ym=(y1+y2)/2.0;
   d=sqrt(pow(x2-x1,2)+pow(y2-y1,2));
   CNC_out_file <<  "Q9=";
   WriteNumber(xm);
   CNC_out_file <<  " Q10=";
   WriteNumber(ym);
   CNC_out_file <<  " Q11=";
   WriteNumber(d);
   CNC_out_file << endl;
   CNC_out_file <<  "M98 (C:\\CNC\\PROGRAMS\\HOLE1.SUB)";
   CNC_out_file << endl;
}

*/



