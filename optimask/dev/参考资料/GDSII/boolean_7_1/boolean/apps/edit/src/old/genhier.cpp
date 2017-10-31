/*
Program	CNCHIER.CPP
*/
#pragma implementation

#include "arc.h"
#include "polyline.h"
#include "boundary.h"
#include "referenc.h"
#include "elemlst.h"
#include "gdsii.h"
#include "path.h"
#include "text.h"

#include "cnchier.h"

class StructureIter;

// contructor  of screendriver
// makes a matrix and set boundingboxes
HierDriver::HierDriver(const wxString& _filename, bool only_visible) //Olly added parameters to solve ERR
{
	// holds temporary matrix from a structurereference
	// this will be modified in the references to calculate absolute positions
	_matrix = new Matrix<EIGHT_G_BYTE_REAL>(3,3);
	*_matrix = _matrix->identity(3);
	if (_matrix == 0)
		throw GDS_Error("Cannot allocate memory for a matrix",
			"Screendriver Error", 0, 1);

	_only_visible = true;
	_visible = true;

	_AbsPolygon = new G_Polygon();

   _AI			= new TDLI<Segment>();

}

// destructor: frees dynamic allocated memory
HierDriver::~HierDriver()
{
	delete _matrix;

   delete _AI;

	if (_AbsPolygon) delete _AbsPolygon;

	delete _CR;
	delete _CR_list;

	delete _AET;
	delete _AET_list;

}


// screendriver needs the units
void HierDriver::SetUnits(Units* units)
{
	_units=units;
}

// return the boundingbox from the GDS-II
BoundingBox* HierDriver::GetVirtualSizeScreen()
{
	return _virtualsize;
}


// return the inverse mapping matrix for zooming or coordinates
Matrix<EIGHT_G_BYTE_REAL>* HierDriver::GetInverseMappingMatrix()
{
	return _inverse_mapping;
}

Matrix<EIGHT_G_BYTE_REAL>* HierDriver::GetMappingMatrix()
{
	return _mapping_matrix;
}

// multiplies the relative matrix with the X and Y
// Then there are no relative structures anymore
void HierDriver::Transform(EIGHT_G_BYTE_REAL &x,
										  EIGHT_G_BYTE_REAL &y)
{
	/*
	This is how it used to be but because of speed we only multiply
	the fields needed (see below this comment)

	Matrix<EIGHT_G_BYTE_REAL>* _temp = new Matrix<EIGHT_G_BYTE_REAL>(3, 1);
	assert(_temp != 0);

	try
	{
		(*_temp)[0][0] = x;
		(*_temp)[1][0] = y;
		(*_temp)[2][0] = 1;

		*_temp = *_matrix * (*_temp); // _temp *= _matrix is not correct!
		x = (*_temp)[0][0];
		y = (*_temp)[1][0];
	}
	catch(GDS_Error &_error)
	{
		MessageBox(0, _error.GetMessage(), "Make absolute", MB_OK | MB_ICONHAND);
		exit(1);
	}

	delete _temp;
	*/

	// here is the optimized code for the code above
	EIGHT_G_BYTE_REAL temp = (*_matrix)[0][0] * x + (*_matrix)[0][1] * y + (*_matrix)[0][2];
	y = (*_matrix)[1][0] * x + (*_matrix)[1][1] * y + (*_matrix)[1][2];
	x = temp;
}


// see comment above
void HierDriver::Transform(EIGHT_G_BYTE_REAL &x1,
										  EIGHT_G_BYTE_REAL &y1,
										  EIGHT_G_BYTE_REAL &x2,
										  EIGHT_G_BYTE_REAL &y2)
{
	Transform(x1, y1);
	Transform(x2, y2);
}


// maps a point in GDS-II coordiantes to a point in screen coordinates
void HierDriver::MakeWindowCoordinates(EIGHT_G_BYTE_REAL &x,
													  EIGHT_G_BYTE_REAL &y)
{
	/*
	This is how it used to be but because of speed we only multiply
	the fields needed (see below this comment)

	Matrix<EIGHT_G_BYTE_REAL>* _temp = new Matrix<EIGHT_G_BYTE_REAL>(3, 1);
	assert(_temp != 0);

	try
	{
		(*_temp)[0][0] = x;
		(*_temp)[1][0] = y;
		(*_temp)[2][0] = 1;

		*_temp = *_mapping_matrix * (*_temp); // _temp *= _mapping_matrix is not correct!
		x = (*_temp)[0][0];
		y = (*_temp)[1][0];
	}
	catch(GDS_Error &_error)
	{
		MessageBox(0, _error.GetMessage(), "Make Window Coordinates", MB_OK | MB_ICONHAND);
		exit(1);
	}

	delete _temp;*/

	// here is the optimized code for the code above
/*
	x = (*_mapping_matrix)[0][0] * x + (*_mapping_matrix)[0][2];
	x = (x - floor(x) < .5) ? floor(x) : ceil(x);
	y = (*_mapping_matrix)[1][1] * y + (*_mapping_matrix)[1][2];
	y = (y - floor(y) < .5) ? floor(y) : ceil(y);
*/
	// here is the even more optimized code
	x = floor((*_mapping_matrix)[0][0] * x + (*_mapping_matrix)[0][2] + 0.5);
	y = floor((*_mapping_matrix)[1][1] * y + (*_mapping_matrix)[1][2] + 0.5);
}

// see two functions above
void HierDriver::MakeWindowCoordinates(EIGHT_G_BYTE_REAL &x1,
													  EIGHT_G_BYTE_REAL &y1,
													  EIGHT_G_BYTE_REAL &x2,
													  EIGHT_G_BYTE_REAL &y2)
{
	MakeWindowCoordinates(x1, y1);
	MakeWindowCoordinates(x2, y2);
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
	if ((_Boundary->GetWidth() != 0) || (arcs) || (! ident))
   {
		_Boundary->Convert(_AbsPolygon, DISPLAYABER);
		_AbsPolygon->Transform(_matrix);
      todraw = _AbsPolygon;
   } else {
   	todraw = _Boundary->GetPolygon();
   }

	// write it's polygon
	*this << todraw;

	// set _clip_status back to old value
}


// Box -----------------------------------------------------------------
void HierDriver::Write(Box* _Box)
{
   G_Polygon*			todraw = _AbsPolygon;
//   bool 	ident = _matrix->is_identity();
   bool ident= (_drawstructure==_showstructure); //to now if we are at top level or not.

	if ((_Box->GetWidth() != 0) || (! ident)) {
		_Box->Convert(_AbsPolygon, DISPLAYABER);
		_AbsPolygon->Transform(_matrix);
      todraw = _AbsPolygon;
   } else {
      todraw = _Box->GetPolygon();
   }

	// write it's polygon
	*this << todraw;
}


// Circle -----------------------------------------------------------------
void HierDriver::Write(Circle* _Circle)
{
	// Convert circle to polygon
	_Circle->Convert(_AbsPolygon, DISPLAYABER);

	_AbsPolygon->Transform(_matrix);

	_AbsPolygon->ConvertArcs(DISPLAYABER);

	// write it's polygon
	*this << _AbsPolygon;
}


// ElementList -------------------------------------------------------------
void HierDriver::Write(ElementList* _ElementList)
{
  	TDLI<Element> I=TDLI<Element>(_ElementList);
   bool ident= (_drawstructure==_showstructure); //to now if we are at top level or not.

	if (I.empty()) return;

	I.toroot();
	while (I.iterate())
   {

		_Selected = I.item()->GetSelect();
      Write(I.item());
      //		*this << I.item();
	}
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
            if ((_only_visible) && (!_setup->Get_Visible(a_shape->GetLayer())))
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

   switch (_Element->GetType())
   {
      case BOUNDARY:
           {
            Boundary* a_boundary= (Boundary*) _Element;
            *this << a_boundary;
            break;
           }
      case PATH:
           {
            Path* a_path= (Path*) _Element;
            *this << a_path;
            break;
           }
      case ARCSEG:
           {
            Arcseg* a_arc= (Arcseg*) _Element;
            *this << a_arc;
            break;
           }
      case TEXT:
           {
            Text* a_text= (Text*) _Element;
            *this << a_text;
            break;
           }
      case AREF:
          {
            ArrayReference* a_arrayreference = (ArrayReference*) _Element;
            *this << a_arrayreference;
            break;
          }
      case SREF:
          {
            StructureReference* a_structurereference = (StructureReference*) _Element;
            *this << a_structurereference;
            break;
          }

/*
      case GDS_NODE:
         break; // not implemented
*/
      case BOX:
          {
            Box* a_box= (Box*) _Element;
            *this << a_box;
            break;
          }
      case CIRCLE:
          {
            Circle* a_cir= (Circle*) _Element;
            *this << a_cir;
            break;
          }
      default:
         throw GDS_Error("Unknown element in element list","error",9);
   }

	UnSetLayer();
	 return;
}


// GdsII --------------------------------------------------------------------
void HierDriver::Write(GdsII* _GdsII)
{
   _showstructure=_GdsII->GetShowStructure();
   _drawstructure=_showstructure; //to now if we are at top level or not.

	*_matrix = _matrix->identity(3);

	// begin drawing the first structure if any
	if (_showstructure)
   {
      //setup an array with the layer order as index
		int	layerorder[MAX_LAYERS];
		for (int layer=0; layer < MAX_LAYERS; layer++)
      {  int curorder=_setup->Get_Order(layer);
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
				if ((_setup->Get_Available(_ActiveLayer))&&(_setup->Get_Visible(_ActiveLayer)))
					*this << _showstructure;
			}
		}
 	} else throw GDS_Error("No Structures in this GDS-II file", "Screendriver warning", 5, 0);

}


// Structure ----------------------------------------------------------------
void HierDriver::Write(Structure* _Structure)
{
   Structure* backupdraw=_drawstructure;
   _drawstructure=_Structure; //to now if we are at top level or not.
   bool ident= (_drawstructure==_showstructure); //to now if we are at top level or not.

//	_Structure->SortOnBbox(); //slows it down to much

	// write the boundaries, text, paths etc
    Write(_Structure->GetElementList());
    //basic driver not used
    //	*this	<< _Structure->GetElementList();

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

	bool		SRefSelBackup;

 	// first make a copy in the driver it's matrix
	Matrix<double> *_backup = new Matrix<double>(*_matrix);
	assert(_backup != 0);

	*_matrix *= (*_ArrayReference->GetRelativeMatrix());

   // get row and column distance
   int columns=_ArrayReference->GetColumnsRows()->GetColumns();
   int rows=_ArrayReference->GetColumnsRows()->GetRows();

   Point offset=*_ArrayReference->GetPoint();
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
			*this << _ArrayReference->GetStructure();
         _matrix->Translate2D(dx1,dy1);
      }
      //translate back in dx1*columns and in dy1
      _matrix->Translate2D(-dx1*columns + dx2,-dy1*columns +dy2);
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

	bool		SRefSelBackup;

	// first make a copy in the driver it's matrix
	Matrix<double> *_backup = new Matrix<double>(*_matrix);
	assert(_backup != 0);

	*_matrix *= (*_StructureReference->GetRelativeMatrix());

	*this << _StructureReference->GetStructure();

	// now copy matrix back
	*_matrix = *_backup;
	delete _backup;
}


void HierDriver::Write(Path* _Path)
{
   G_Polygon*			todraw = _AbsPolygon;
   bool arcs 	= _Path->ContainsArcs();
   bool ident= (_drawstructure==_showstructure); //to now if we are at top level or not.

	// Convert path to polygon with width
	if ((_Path->GetWidth() != 0) || (arcs) || (! ident))
   {
		_Path->Convert(_AbsPolygon, DISPLAYABER);
		_AbsPolygon->Transform(_matrix);
      todraw = n;
   } else {
      todraw = (G_Polygon*)_Path->GetPolyline();
   }

	// draw its polyline
	if (_Path->GetWidth())
		*this << todraw;
	else
		*this << (G_Polyline*) todraw;
}





void HierDriver::Write(Arcseg* _Arcseg)
{
	// Convert arcseg to polygon
	_Arcseg->Convert(_AbsPolygon, DISPLAYABER);

	_AbsPolygon->Transform(_matrix);

	// check if it is selected
	_Selected = _Arcseg->GetSelect();

	_AbsPolygon->ConvertArcs(DISPLAYABER);

	// write it's polygon
	*this << _AbsPolygon;
}


void HierDriver::Write(Point* _Point)
{
	GDSDrawPoint(_Point->GetX(), _Point->GetY());
}


void HierDriver::Write(Segment* _Segment)
{
	GDSDrawPoint(_Segment->GetX(), _Segment->GetY());
}


// Polygon ------------------------------------------------------------------
void HierDriver::Write(G_Polygon* _Polygon)
{

	// are there points anyway?
	if (_Polygon->count() < 2 )
   	return;

   _AI->Attach(_Polygon);
   int points=_AI->count();
   // now draw the wireframe
   _AI->tohead();
   double x,y;
   double old_x = _AI->item()->GetX();
   double old_y = _AI->item()->GetY();

   (*_AI)++;
   while (!_AI->hitroot())
   {
      x = _AI->item()->GetX();
      y = _AI->item()->GetY();
      if (_drawstyle != SELECTED) GDSDrawLine(old_x, old_y, x, y);
      if (_Selected ||(_SRefSelected)) DrawLineSelected(old_x, old_y, x, y);
      old_x = x;
      old_y = y;
      (*_AI)++;
   }
   _AI->tohead();
   x = _AI->item()->GetX();
   y = _AI->item()->GetY();
   // draw the line to close the end- and beginpoints
   if (_drawstyle != SELECTED) GDSDrawLine(old_x, old_y, x, y);
   if ( _Selected ||(_SRefSelected)) DrawLineSelected(old_x, old_y, x, y);
   _AI->Detach();
}


// Polyline -----------------------------------------------------------------
void HierDriver::Write(G_Polyline* _Polyline)
{
	// are there points anyway?
	if (_Polyline->count() < 2 )
   	return;

	_AI->Attach(_Polyline);
	int points = _AI->count();


   _AI->tohead();
   double old_x = _AI->item()->GetX();
   double old_y = _AI->item()->GetY();
   double x,y;

   (*_AI)++;
   while (!_AI->hitroot())
   {
      x = _AI->item()->GetX();
      y = _AI->item()->GetY();

      // write the line
      GDSDrawLine(old_x, old_y, x, y);

      old_x = x;
      old_y = y;
      (*_AI)++;
   }
	_AI->Detach();
}

// Text ---------------------------------------------------------------------
void HierDriver::Write(Text* _Text)
{
	Point min_point = _Text->GetMin();
	Point max_point = _Text->GetMax();

   GDSDrawText(_Text->MyText(), min_point.GetX(), min_point.GetY(),
                             max_point.GetX(), max_point.GetY(),
                             _Text->GetPresentation(), &(*_matrix * *_Text->GetRelativeMatrix()));
}





