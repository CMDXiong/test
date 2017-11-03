/*
Program	BBOX.CPP
Purpose	The BoundingBox class stores one boundingbox.
		The boundingbox is defined by two coordiates,
		a upperleft coordinate and a lowerright coordinate.
		The boundingBox will be used by many classes:
		boundary, path, text, sref, aref, structure and
		gdsii. When an error occurs an object of class will be thrown.
Last Update	28-09-1995
*/

// constructor with 4 points
// usage: BoundingBox bbox(0, 0, 100, 100);
//        or BoundingBox bbox();

#ifdef __GNUG__
#pragma implementation 
#endif

#include "bbox.h"

BoundingBox::BoundingBox()
{
	_xmin = 0.0;
	_ymin = 0.0;
	_xmax = 0.0;
	_ymax = 0.0;
	_valid_bbox = false;
}


BoundingBox::BoundingBox(BoundingBox &other)
{
	_xmin = other._xmin;
	_ymin = other._ymin;
	_xmax = other._xmax;
	_ymax = other._ymax;
	_valid_bbox= other._valid_bbox;
}


BoundingBox::BoundingBox(const Point& a)
{
	_xmin = a.GetX();
	_xmax = a.GetX();
	_ymin = a.GetY();
	_ymax = a.GetY();
	_valid_bbox = true;
}

BoundingBox::BoundingBox(double xmin, double ymin, double xmax, double ymax)
{
	_xmin = xmin;
	_ymin = ymin;
	_xmax = xmax;
	_ymax = ymax;
	_valid_bbox = true;
}

// This function checks if two bboxes intersect
bool BoundingBox::And(BoundingBox *_bbox, double Marge)
{
	assert (_valid_bbox == true);
	assert (_bbox->GetValid());
	_xmin = Max(_xmin, _bbox->_xmin);
	_xmax = Min(_xmax, _bbox->_xmax);
	_ymin = Max(_ymin, _bbox->_ymin);
	_ymax = Min(_ymax, _bbox->_ymax);
	return (bool)
			 (
			 ((_xmin - Marge) < (_xmax + Marge)) &&
			 ((_ymin - Marge) < (_ymax + Marge))
			 );
}

// Enlarge the boundingbox with the given marge
void	BoundingBox::EnLarge(const double Marge)
{
	assert (_valid_bbox == true);

	_xmin -= Marge;
	_xmax += Marge;
	_ymin -= Marge;
	_ymax += Marge;
}


// Shrink the boundingbox with the given marge
void	BoundingBox::Shrink(const double Marge)
{
	assert (_valid_bbox == true);

	_xmin += Marge;
	_xmax -= Marge;
	_ymin += Marge;
	_ymax -= Marge;
}


// Expand the boundingbox with another boundingbox
void BoundingBox::Expand(BoundingBox &other)
{
	assert (_valid_bbox == true);
	assert (other.GetValid());

	_xmin = Min(_xmin, other._xmin);
	_xmax = Max(_xmax, other._xmax);
	_ymin = Min(_ymin, other._ymin);
	_ymax = Max(_ymax, other._ymax);
}


// Expand the boundingbox with a point
void BoundingBox::Expand(const Point& a_point)
{
	assert (_valid_bbox == true);

	_xmin = Min(_xmin, a_point.GetX());
	_xmax = Max(_xmax, a_point.GetX());
	_ymin = Min(_ymin, a_point.GetY());
	_ymax = Max(_ymax, a_point.GetY());
}

// Expand the boundingbox with a point
void BoundingBox::Expand(double x,double y)
{
	assert (_valid_bbox == true);

	_xmin = Min(_xmin, x);
	_xmax = Max(_xmax, x);
	_ymin = Min(_ymin, y);
	_ymax = Max(_ymax, y);
}


// Expand the boundingbox with two points
void BoundingBox::Expand(const Point& a, const Point& b)
{
	assert (_valid_bbox == true);

	Expand(a);
	Expand(b);
}


// Calculates if two boundingboxes intersect. If so, the function returns _ON.
// If they do not intersect, two scenario's are possible:
// other is outside this -> return _OUT
// other is inside this -> return _IN
OVERLAP BoundingBox::Intersect(BoundingBox &other, double Marge)
{
	assert (_valid_bbox == true);

	// other boundingbox must exist
	if (!&other)
		throw GDS_Error("There is no boundingbox defined or created\n in function Intersect in class BoundingBox.\nProgram terminates!",
			"Fatal Error", 0, 1);

	if (((_xmin - Marge) > (other._xmax + Marge)) ||
		 ((_xmax + Marge) < (other._xmin - Marge)) ||
		 ((_ymax + Marge) < (other._ymin - Marge)) ||
		 ((_ymin - Marge) > (other._ymax + Marge)))
		return _OUT;
	// Check if other.bbox is inside this bbox
	if ((_xmin <= other._xmin) &&
		 (_xmax >= other._xmax) &&
		 (_ymax >= other._ymax) &&
		 (_ymin <= other._ymin))
		return _IN;

	// Boundingboxes intersect
	return _ON;
}


// Checks if a line intersects the boundingbox
bool BoundingBox::LineIntersect(const Point& begin, const Point& end )
{
	assert (_valid_bbox == true);

	return (bool)
			  !(((begin.GetY() > _ymax) && (end.GetY() > _ymax)) ||
				((begin.GetY() < _ymin) && (end.GetY() < _ymin)) ||
				((begin.GetX() > _xmax) && (end.GetX() > _xmax)) ||
				((begin.GetX() < _xmin) && (end.GetX() < _xmin)));
}


// Is the given point in the boundingbox ??
bool BoundingBox::PointInBox(double x, double y, double Marge)
{
	assert (_valid_bbox == true);

	if (  x >= (_xmin - Marge) && x <= (_xmax + Marge) &&
			y >= (_ymin - Marge) && y <= (_ymax + Marge) )
			return true;
	return false;
}


//
// Is the given point in the boundingbox ??
//
bool BoundingBox::PointInBox(const Point& a, double Marge)
{
	assert (_valid_bbox == true);

	return PointInBox(a.GetX(), a.GetY(), Marge);
}


Point BoundingBox::GetMin()
{
	assert (_valid_bbox == true);

	return Point(_xmin, _ymin);
}


Point BoundingBox::GetMax()
{
	assert (_valid_bbox == true);

	return Point(_xmax, _ymax);
}

bool BoundingBox::GetValid()
{
	return _valid_bbox;
}

void BoundingBox::SetMin(double px, double py)
{
	_xmin = px;
	_ymin = py;
}

void BoundingBox::SetMax(double px, double py)
{
	_xmax = px;
	_ymax = py;
}

void BoundingBox::SetValid(bool value)
{
  _valid_bbox = value;
}

// adds an offset to the boundingbox
// usage : a_boundingbox.Translate(a_point);
void BoundingBox::Translate(Point& offset)
{
	assert (_valid_bbox == true);

	_xmin += offset.GetX();
	_xmax += offset.GetX();
	_ymin += offset.GetY();
	_ymax += offset.GetY();
}


// clears the bounding box settings
void BoundingBox::Reset()
{
	_xmin = 0.0;
	_xmax = 0.0;
	_ymin = 0.0;
	_ymax = 0.0;
	_valid_bbox = false;
}


void BoundingBox::SetBoundingBox(const Point& a_point)
{
	_xmin = a_point.GetX();
	_xmax = a_point.GetX();
	_ymin = a_point.GetY();
	_ymax = a_point.GetY();
}


// Expands the boundingbox with the given point
// usage : a_boundingbox = a_boundingbox + pointer_to_an_offset;
BoundingBox& BoundingBox::operator+(BoundingBox &other)
{
	assert (_valid_bbox == true);
	assert (other.GetValid());

	Expand(other);
	return *this;
}


// makes a boundingbox same a the other
BoundingBox& BoundingBox::operator=(BoundingBox &other)
{
	assert (other.GetValid());

	_xmin = other._xmin;
	_xmax = other._xmax;
	_ymin = other._ymin;
	_ymax = other._ymax;
	_valid_bbox = other._valid_bbox;
	return *this;
}


double BoundingBox::Min(double a, double b)
{
	assert (_valid_bbox == true);

	return (a < b) ? a : b;
}

double BoundingBox::Max(double a, double b)
{
	assert (_valid_bbox == true);

	return (a > b) ? a : b;
}


void BoundingBox::MapBbox(wxTransformMatrix* a_matrix)
{  int i;

	assert (_valid_bbox == true);

	Point	corners[4];
	// corners[0] is left_bottom
	// corners[1] is left_top
	// corners[2] is right_bottom
	// corners[3] is right_top

	corners[0].Set(_xmin, _ymin);
	corners[1].Set(_xmin, _ymax);
	corners[2].Set(_xmax, _ymin);
	corners[3].Set(_xmax, _ymax);

	// Multiply the points with the relative matrix
	for (i = 0; i < 4; i++)
	{
      corners[i].Transform(a_matrix);
	}

	double xmin = corners[0].GetX();
	double ymin = corners[0].GetY();
	double xmax = xmin;
	double ymax = ymin;

	for (i = 1; i < 4; i++)
	{
		// Search for the smallest X-value
		if (xmin > corners[i].GetX())
			xmin = corners[i].GetX();
		// Search for the smallest Y-value
		if (ymin > corners[i].GetY())
			ymin = corners[i].GetY();
		// Search for the largest X-value
		if (xmax < corners[i].GetX())
			xmax = corners[i].GetX();
		// Search for the largest Y-value
		if (ymax < corners[i].GetY())
			ymax = corners[i].GetY();
	}
	// Use these min and max values to set the new boundingbox
	_xmin = xmin;
	_ymin = ymin;
	_xmax = xmax;
	_ymax = ymax;
}

