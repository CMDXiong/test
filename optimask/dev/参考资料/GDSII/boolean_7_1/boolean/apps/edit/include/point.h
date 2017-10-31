/*
Program	POINT.H
Purpose	Definition of GDSII double structure
Last Update	12-12-1995
*/

#ifndef POINTB_H
#define POINTB_H

#ifdef __GNUG__
#pragma interface
#endif


#include "misc.h"
#include "gdserr.h"
#include "wxmatrix.h"

class Point
{
	public:
		Point();
		Point(double const,double const);
		Point(const Point& );

		void			      Set(const double,const double);
		void			      Set(const Point &);

		double	      	GetX() const;
		double	      	GetY() const;
		void 		         SetX(double);
		void			      SetY(double);
		bool				Equal(const Point& a_point, double Marge) const;
		bool				Equal(const double ,const double , double) const;
		bool				ShorterThan(const Point& a_point, double marge) const;
		bool				ShorterThan(const double X, const double Y, double Marge) const;

		Point					&operator=(const Point &);
		Point					&operator+(const Point &);
		Point					&operator-(const Point &);

		Point					&operator*(int);
		Point					&operator/(int);

		int					operator==(const Point &) const;
		int					operator!=(const Point &) const;

		void 					Transform(wxTransformMatrix* _Matrix);

	protected:
		double _x;
		double _y;

};


#endif
