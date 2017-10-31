/*
Program	POINT.CPP
Purpose	Definition of GDSII point type structure
Last Update	12-12-1995
*/

#ifdef __GNUG__
#pragma implementation 
#endif

#include "point.h"
#include <math.h>

// Constructors
Point::Point()
{
	_x = 0.0;
	_y = 0.0;
}


Point::Point(double const X, double const Y)
{
	_x = X;
	_y = Y;
}


Point::Point(const Point& a_point)
{
	_x = a_point._x;
	_y = a_point._y;
}


double Point::GetX() const
{
	return _x;
}

double Point::GetY() const
{
	return _y;
}


void Point::SetX(double a_point_x)
{
	_x = a_point_x;
}


void Point::SetY(double a_point_y)
{
	_y = a_point_y;
}

void Point::Set(const double X,const double Y)
{
	_x = X;
	_y = Y;
}


void Point::Set(const Point &a_point)
{
	_x = a_point._x;
	_y  =a_point._y;
}

bool Point::Equal(const Point& a_point, double Marge) const
{
	double delta_x, delta_y;

	delta_x = fabs((_x - a_point._x));
	delta_y = fabs((_y - a_point._y));

	if ((delta_x <= Marge) && (delta_y <= Marge))
		return true;
	else
		return false;
}


bool Point::Equal(const double X, const double Y, double Marge)  const
{
	return (bool)((fabs(_x - X) <= Marge) && (fabs(_y - Y) <= Marge));
}

bool Point::ShorterThan(const Point& a_point, double Marge)  const
{
	double a,b;
	a = a_point._x - _x;
	a*= a;
	b = a_point._y - _y;
	b*= b;

	return (bool) ( (a+b) <= Marge*Marge ? true : false ) ;
}


bool Point::ShorterThan(const double X, const double Y, double Marge) const
{
	double a,b;
	a = X - _x;
	a*= a;
	b = Y - _y;
	b*= b;

	return (bool) ( a+b <= Marge*Marge ? true : false ) ;
}


// overload the assign (=) operator
// usage : a_point = another_point;

Point &Point::operator=(const Point &other_point)
{
	_x = other_point._x;
	_y = other_point._y;
	return *this;
}


// overload the + operator
// usage : a_point = point1 + point2;

Point &Point::operator+(const Point &other_point)
{
	_x += other_point._x;
	_y += other_point._y;
	return *this;
}



// overload the - operator
// usage : a_point = point1 - point2;

Point &Point::operator-(const Point &other_point)
{
	_x -= other_point._x;
	_y -= other_point._y;
	return *this;
}


// overload the * operator
// usage: a_point = point1 * 100;

Point &Point::operator*(int factor)
{
	_x *= factor;
	_y *= factor;
	return *this;
}


// overload the / operator
// usage: a_point = point1 / 100;

Point &Point::operator/(int factor)
{
	_x /= factor;
	_y /= factor;
	return *this;
}


// overload the compare (==) operator
// usage: if (point1 == point2) { };

int Point::operator==(const Point &other_point) const
{
	return ((other_point._x == _x) && (other_point._y == _y));
}


// overload the diffrent (!=) operator
// usage: if (point1 != point2) { };

int Point::operator!=(const Point &other_point) const
{
	return ((other_point._x != _x) || (other_point._y != _y));
}

// multiplies the relative matrix with the X and Y
// Then there are no relative structures anymore
void Point::Transform(wxTransformMatrix* _Matrix)
{
   double xh,yh;
   xh=_x;
   yh=_y;
	_Matrix->TransformPoint(xh,yh, _x, _y);
}

